/* 
 * File:   EEPROM.c
 * Author: Raph Weyman
 *
 * Created on 01 December 2017, 10:43
 * 
 * EEPROM emulation in flash program memory.
 * Copied and modified from Microchip AN1095.
 * Apart from general tidy up and better coding practices, the modifications
 * were to remove multiple banks, better handle errors here were possible (no
 * point in passing errors back up to a layer that's in no position to deal with
 * the resultant mess) and not look for them where a graceful recovery is unlikely anyway.
 * Also made the initialization a bit more likely to recover from a gross corruption.
 * Erroneous page flags removed and write cycles unlimited - no point stopping and producing
 * artificial errors that the higher layers can't handle - nothing to lose by continuing into genuine errors.
 *
 * InitializeEEPROM must be invoked before anything else. It can take a few ms to complete so call before any
 * time critical services have started.
 * DataEERead and DataEEWrite can then be used to read and write data from emulated EEPROM addresses according
 * to the DATA_EE_SIZE. Writes to invalid addresses are ignored and reads from either invalid or unwritten
 * addresses return the ERASED_WORD_VALUE.
 * 
 * Writes cause the CPU to stall - can be for a while if the active flash page needs to be packed into a new one.
 *
 * A defined number of flash pages are used. The linker must be configured to reserve some program memory space
 * for them and, if they're to be preserved, the programmer must avoid them too.
 * Each page has the first location as a status word (high byte) and a cycle counter (low word). The remaining
 * locations in each page are written successively until the page fills up - at which point the page content is
 * packed into a new page and the current page erased.
 * EEPROM addresses are emulated in the high byte of the flash locations with data content in the low word.
 * A write of a new value to the emulated EEPROM will write to the next available location in the page - so on read
 * only the last written value for an emulated address is taken into account with the previous written values for that
 * emulated address ignored. Scheme is unchanged from the microchip application note.
 * 
 * The status flag for each page indicates whether the page is the active one, available, or invalid (anything else).
 * There should be only one active page at any time apart from when a page is full and is being packed into a new page. There
 * will be two pages marked active briefly during this process. If a reset happens at a bad time then the initialization
 * routine may discover more than one active page in which case it will erase all but one of them.
 * 
 * A cycle counter is maintained in order to track how many times the pages have been erased and re-written - but
 * no action is taken as a result of it. PIC24HJ EEPROM is specified to 10000 writes which is very unlikely in
 * practice due to the load balancing that's inherent in the emulation but, if it is reached, there's not much point
 * stopping.
 * 
 */


#include "EEPROM.h"


// User defined constants
#define NUM_DATA_EE_PAGES       20
#define INITIALIZATION_PACK_COUNT 10 // Initialization will pack if less than this number of free locations in the current page


#if DATA_EE_SIZE > 255
    #error Maximum data EE size is 255
#endif

#if NUM_DATA_EE_PAGES < 2
    #error Minimum number of program memory pages is 2
#endif


// NVMCON values
#define ERASE               0x4042
#define PROGRAM_ROW         0x4001
#define PROGRAM_WORD        0x4003



// Internal constants
#define	NUMBER_OF_INSTRUCTIONS_IN_PAGE  512
#define	NUMBER_OF_INSTRUCTIONS_IN_ROW   64
#define NUMBER_OF_ROWS_IN_PAGE          (NUMBER_OF_INSTRUCTIONS_IN_PAGE / NUMBER_OF_INSTRUCTIONS_IN_ROW)


//Data EE info stored in PM in following format
//  Status in first two locations of PM page,
//  8-bit DEE Address (odd address, low byte) 16-bit DEE data (even address)
uint8_t emulationPages[NUM_DATA_EE_PAGES][NUMBER_OF_INSTRUCTIONS_IN_PAGE * 2]
    __attribute__ ((space(prog), aligned(NUMBER_OF_INSTRUCTIONS_IN_PAGE * 2), section(".eeprom"), noload));

#define DEE_PAGE_SIZE (sizeof(emulationPages[0]))

#define DEE_PAGE_TBL(page) ((__builtin_tbladdress(&emulationPages) + (DEE_PAGE_SIZE * (page))) >> 16)
#define DEE_PAGE_OFFSET(page) ((__builtin_tbladdress(&emulationPages) + (DEE_PAGE_SIZE * (page))) & 0xFFFF)


// Page status flags values
#define PAGE_ACTIVE_STATUS 0xAC                 // page is active - should be only one active page
#define PAGE_AVAILABLE_STATUS ERASED_BYTE_VALUE // Page is available for use
#define PAGE_STATUS_INVALID 0


static void     UnlockWrite(void);
static uint8_t  GetPageStatus(const uint8_t page);
static void     ErasePage(const uint8_t page);
static uint8_t  ActivePage(void);
static uint16_t FreeInPage(const uint8_t page);
static void     PackEE(void);


/************************************************************************
UnlockWrite

This routine saves the current CPU priority and sets it the highest
user level of 7. It calls an assembly routine to perform an unlock
sequence and sets the WR bit in NVMCON. The WR bit is polled until it
clears indicating the flash operation is complete. The previous CPU
priority is restored.

Parameters:		None
Return:			None
Side Effects:	None
************************************************************************/
static void UnlockWrite(void)
{
    __builtin_disi(5);
    __builtin_write_NVM();
}


/************************************************************************
 * FreeInPage
 * 
 * Returns number of free locations in the page
 * 
 * 
************************************************************************/
static uint16_t FreeInPage(const uint8_t page)
{
    uint16_t offset;      //Current page offset
    uint16_t count, i;
    count = 0;
    if (page < NUM_DATA_EE_PAGES)
    {
        TBLPAG = DEE_PAGE_TBL(page);
        offset = DEE_PAGE_OFFSET(page);
        for (i=1; i < NUMBER_OF_INSTRUCTIONS_IN_PAGE; ++i)
        {
            if (__builtin_tblrdh(offset+(i*2)) == ERASED_BYTE_VALUE)
            {
                ++count;
            }
        }   
    }
    return count;
}


/************************************************************************
GetPageStatus

This routine returns the page status byte for the selected page.

Parameters:		Page number
Return:			page status byte - compare with the page status bitmasks
                and values to make sense of it.
************************************************************************/
static uint8_t GetPageStatus(const uint8_t page)
{
    uint16_t statusOffset;
    uint8_t statusByte = PAGE_STATUS_INVALID;

    if (page < NUM_DATA_EE_PAGES)
    {
        // Point to proper TBLPAG and offset
        TBLPAG = DEE_PAGE_TBL(page);
        statusOffset = DEE_PAGE_OFFSET(page);
        statusByte = __builtin_tblrdh(statusOffset);
    }
    return statusByte;
}

/************************************************************************
ErasePage

This routine erases the selected page.

Parameters:		Page number
Return:			None
************************************************************************/
static void ErasePage(const uint8_t page)
{
    uint16_t pmOffset;           //Current array (page) offset of selected element (PM 16-bit word)
    // Point to proper TBLPAG and offset
    TBLPAG = DEE_PAGE_TBL(page);

    NVMCON = ERASE;

    pmOffset = DEE_PAGE_OFFSET(page);

    __builtin_tblwtl(pmOffset, pmOffset);

    UnlockWrite();
}

/************************************************************************
 * ActivePage
 *
 * returns either the active page or NUM_DATA_EE_PAGES if no active page
 * is found
 * 
 ************************************************************************/
static uint8_t ActivePage(void)
{
    uint8_t page;
    for (page = 0;
      (page < NUM_DATA_EE_PAGES) &&
      (GetPageStatus(page) != PAGE_ACTIVE_STATUS);
      ++page);

    return page;
}


/************************************************************************
PackEE

This routine finds the active page and an available page. The most
recent data EEPROM values are located for each address using ReadEE
function and written into write latches. Page status is read from active
page and erase/write count is incremented if page 0 is packed. After all
information is programmed and verified, the current page is erased. The
packed page becomes the active page. This function can be called at any-
time by the user to schedule the CPU stall.

Parameters:		None
Return:			None
Side Effects:	Generates CPU stall during program/erase operations and
                overwrites program memory write latches. Data EE flags
                may be updated
************************************************************************/
static void PackEE(void)
{
    uint16_t currentPage;        //Array row (PM page) of active DEE page
    uint16_t packedPage;         //Array row (PM page) of packed page
    uint16_t savedTBLPAG;        //Context save of TBLPAG value. Current and packed page are on same page.
    uint16_t currentOffset;      //Current page offset
    uint16_t packedOffset;       //Packed page offset
    uint16_t i;
    uint8_t latchAddr;
    uint16_t latchData;
    savedTBLPAG = TBLPAG;

    currentPage = ActivePage();

    if (currentPage < NUM_DATA_EE_PAGES)
    {
        // Find the next page to use
        packedPage = currentPage + 1;
        if (packedPage == NUM_DATA_EE_PAGES)
        {
            packedPage = 0;
        }

        ErasePage(packedPage);

        // Point to first location in packed page
        TBLPAG = DEE_PAGE_TBL(packedPage);
        packedOffset = DEE_PAGE_OFFSET(packedPage);

        i = 0;
        NVMCON = PROGRAM_ROW;

        __builtin_tblwtl(packedOffset, ERASED_WORD_VALUE);
        __builtin_tblwth(packedOffset, ERASED_BYTE_VALUE);
        packedOffset += 2;

        latchAddr = 0;
        ++i;

        do
        {
            while((latchAddr < DATA_EE_SIZE) && (i < NUMBER_OF_INSTRUCTIONS_IN_ROW))
            {
                latchData = DataEERead(latchAddr);
                if(latchData != ERASED_WORD_VALUE)       //if address is unwritten, skip to next address
                {
                    __builtin_tblwtl(packedOffset, latchData);
                    __builtin_tblwth(packedOffset, latchAddr);
                    packedOffset += 2;
                    ++i;
                }
                latchAddr++;
            }
            while(i < NUMBER_OF_INSTRUCTIONS_IN_ROW)
            {
                __builtin_tblwtl(packedOffset, ERASED_WORD_VALUE);
                __builtin_tblwth(packedOffset, ERASED_BYTE_VALUE);
                packedOffset += 2;
                ++i;
            }
            UnlockWrite();
            i = 0;
        }
        while(latchAddr < DATA_EE_SIZE);

        //Program page status

        // update the cycle count and make the packed page active
        currentOffset = DEE_PAGE_OFFSET(currentPage);
        packedOffset = DEE_PAGE_OFFSET(packedPage);
        TBLPAG = DEE_PAGE_TBL(currentPage);
        latchData = __builtin_tblrdl(currentOffset);
        if((packedPage == 0) && (latchData < UINT16_MAX))
        {
            ++latchData;        //Increment E/W counter
        }
        TBLPAG = DEE_PAGE_TBL(packedPage);
        __builtin_tblwth(packedOffset, PAGE_ACTIVE_STATUS);
        __builtin_tblwtl(packedOffset,latchData);

        NVMCON = PROGRAM_WORD;
        UnlockWrite();

        //Erase active page
        ErasePage(currentPage);
    }
    TBLPAG = savedTBLPAG;
}

/************************************************************************
InitializeEEPROM

This routine finds a page to become an active page. It then
counts the number of active pages. If no active pages are found, the
first page is initialized for emulation. If one active page is
found, it is assumes a reset occurred and the function does nothing. If more than one
active pages are found, it is assumes a reset occurred during a pack - all but the one
are erased.
Any page that doesn't have a valid status flag is erased.

Parameters:		None
Return:			None
************************************************************************/
void InitializeEEPROM(void)
{
    uint16_t count;
    uint16_t savedTBLPAG;        //Context save of TBLPAG value. Current and packed page are on same page.
    uint16_t currentPage;
    uint16_t pageOffset;

    savedTBLPAG = TBLPAG;

    count = 0;

    // Count active page(s).
    for (currentPage = 0; currentPage < NUM_DATA_EE_PAGES; ++currentPage)
    {
        switch (GetPageStatus(currentPage))
        {
            case PAGE_ACTIVE_STATUS:
                if (count > 0)
                {
                    ErasePage(currentPage);
                }
                count++;
                break;
            case PAGE_AVAILABLE_STATUS:
                break;
            default:
                ErasePage(currentPage);
                break;
        }
    }
 
    //If no active pages found, initialize page 0
    if(count == 0)
    {
        // Point the table page pointer to the emulation pages
        ErasePage(0);

        // Point to proper TBLPAG and offset
        TBLPAG = DEE_PAGE_TBL(0);
        pageOffset = DEE_PAGE_OFFSET(0);

        NVMCON = PROGRAM_WORD;

        __builtin_tblwtl(pageOffset, 0);    //New page: unavailable, active, reset count
        __builtin_tblwth(pageOffset, PAGE_ACTIVE_STATUS);
        UnlockWrite();
    }
    
    // if there are few remaining free locations in the active page then pack it now
    // so as to save run time later
    if (FreeInPage(ActivePage()) < INITIALIZATION_PACK_COUNT)
    {
        PackEE();
    }
    TBLPAG = savedTBLPAG;
}

/************************************************************************
DataEERead

This routine verifies the address is valid. If not, the Illegal Address
flag is set and 0xFFFF is returned. It then finds the active page. If an
active page can not be found, the Page Corrupt status bit is set and
0xFFFF is returned. A reverse search of the active page attempts to find
the matching address in the program memory MSB (odd address). If a match
is found, the corresponding data EEPROM data (even address) is returned,
otherwise 0xFFFF is returned. This function can be called by the user.

Parameters:		Data EE address
Return:			Data EE data or 0xFFFF if address not found
Side Effects:	Data EE flags may be updated.
************************************************************************/
uint16_t DataEERead(const uint8_t addr)
{
    uint16_t savedTBLPAG;        //Context save of TBLPAG value. Current and packed page are on same page.
    uint8_t page;
    uint16_t pageOffset;         //Current array (page) offset of selected page (PM 16-bit word)
    uint16_t pmOffset;           //Current array (page) offset of selected element (PM 16-bit word)
    uint8_t latchAddr;
    uint16_t i;
    uint16_t return_value;

    savedTBLPAG = TBLPAG;

    return_value = ERASED_WORD_VALUE;

    if(addr < DATA_EE_SIZE)
    {
        page = ActivePage();

        if (page < NUM_DATA_EE_PAGES)
        {
            // Point to proper TBLPAG and offset
            TBLPAG = DEE_PAGE_TBL(page);
            pageOffset = DEE_PAGE_OFFSET(page);

            for (i=NUMBER_OF_INSTRUCTIONS_IN_PAGE; i>0;--i)
            {
                pmOffset = pageOffset+(i*2);
                latchAddr = __builtin_tblrdh(pmOffset);
                if (latchAddr == addr)
                {
                    return_value = __builtin_tblrdl(pmOffset);
                    break;
                }
            }
        }
    }
    TBLPAG = savedTBLPAG;
    return return_value;
}

/************************************************************************
DataEEWrite

This routine verifies the address is valid. If not, the Illegal Address
flag is set and an error code is returned. It then finds the active page.
If an active page can not be found, the Page Corrupt status bit is set
and an error code is returned. A read is performed, if the data was not
changed, the function exits. If the last location is programmed, the Pack
Skipped error flag is set (one location should always be available). The
data EE information (MSB = address, LSW = data) is programmed and
verified. If the verify fails, the Write Error flag is set. If the write
went into the last location of the page, pack is called. This function
can be called by the user.

Parameters:		Data EE address and data
Return:			Pass or fail status (0 = Pass)
Side Effects:	Data EE flags may be updated. CPU stall occurs for flash
                programming. Pack may be generated.
************************************************************************/
void DataEEWrite(const uint16_t data, const uint8_t addr)
{
    uint16_t savedTBLPAG;        //Context save of TBLPAG value. Current and packed page are on same page.
    uint16_t page;
    uint16_t offset;           //Current array (page) offset of selected element (PM 16-bit word)
    uint16_t i;
    
    savedTBLPAG = TBLPAG;

    //Do not write data if it did not change
    if ((addr < DATA_EE_SIZE) && (DataEERead(addr) != data))
    {
        page = ActivePage();
        if (page < NUM_DATA_EE_PAGES)
        {
            if (FreeInPage(page) == 0)
            {
                PackEE();
                page = ActivePage();
            }
        }
        if (page < NUM_DATA_EE_PAGES)
        {
            TBLPAG = DEE_PAGE_TBL(page);
            offset = DEE_PAGE_OFFSET(page);

            for (i=1; i<NUMBER_OF_INSTRUCTIONS_IN_PAGE; ++i)
            {
                offset += 2;
                if (__builtin_tblrdh(offset) == ERASED_BYTE_VALUE)
                {
                    NVMCON = PROGRAM_WORD;
                    __builtin_tblwtl(offset, data);
                    __builtin_tblwth(offset, addr);

                    UnlockWrite();

                    Nop();
                    Nop();
                    break;
                }
            }
        }
    }
    TBLPAG = savedTBLPAG;
}
