EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:microchip_pic24mcu
LIBS:microchip_pic32mcu
LIBS:microchip_pic16mcu
LIBS:raphlib
LIBS:CANPower-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 6
Title ""
Date ""
Rev "1.0"
Comp "Raph Weyman"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	5500 5950 5500 6300
Wire Wire Line
	5900 6300 5900 5950
Connection ~ 5500 6100
Wire Wire Line
	5700 6100 5700 5950
Wire Wire Line
	5600 6100 5600 5950
Connection ~ 5600 6100
Wire Wire Line
	5500 6100 5700 6100
Wire Wire Line
	5900 1300 5900 1550
Wire Wire Line
	5600 1300 5600 1550
Wire Wire Line
	5600 1500 5700 1500
Wire Wire Line
	5700 1500 5700 1550
Connection ~ 5600 1500
$Comp
L C C5
U 1 1 59E39210
P 5350 7000
F 0 "C5" H 5375 7100 50  0000 L CNN
F 1 "100nF" H 5375 6900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 5388 6850 50  0001 C CNN
F 3 "" H 5350 7000 50  0001 C CNN
	1    5350 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 6800 5350 6850
Wire Wire Line
	5350 7150 5350 7200
Wire Wire Line
	5150 1150 5150 1100
Wire Wire Line
	5150 1100 5400 1100
Wire Wire Line
	5400 1100 5400 1550
Wire Wire Line
	5150 1500 5150 1450
$Comp
L Crystal Y1
U 1 1 59E3A04C
P 3100 2500
F 0 "Y1" H 3100 2650 50  0000 C CNN
F 1 "4MHz" H 3100 2350 50  0000 C CNN
F 2 "Crystals:Crystal_SMD_HC49-SD" H 3100 2500 50  0001 C CNN
F 3 "" H 3100 2500 50  0001 C CNN
	1    3100 2500
	0    1    1    0   
$EndComp
$Comp
L C C1
U 1 1 59E3A0A3
P 2750 2300
F 0 "C1" H 2775 2400 50  0000 L CNN
F 1 "20pF" H 2775 2200 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 2788 2150 50  0001 C CNN
F 3 "" H 2750 2300 50  0001 C CNN
	1    2750 2300
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 2700 3550 2700
Wire Wire Line
	3550 2700 3550 2550
Wire Wire Line
	3550 2550 3900 2550
Wire Wire Line
	2900 2300 3550 2300
Wire Wire Line
	3550 2300 3550 2450
Wire Wire Line
	3550 2450 3900 2450
Wire Wire Line
	3100 2350 3100 2300
Connection ~ 3100 2300
Wire Wire Line
	3100 2650 3100 2700
Connection ~ 3100 2700
Wire Wire Line
	2500 2300 2500 2800
Wire Wire Line
	2500 2300 2600 2300
Wire Wire Line
	2600 2700 2500 2700
Connection ~ 2500 2700
$Comp
L R R1
U 1 1 59E3A0DC
P 2750 3500
F 0 "R1" V 2830 3500 50  0000 C CNN
F 1 "10K" V 2750 3500 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 2680 3500 50  0001 C CNN
F 3 "" H 2750 3500 50  0001 C CNN
	1    2750 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 3650 2750 3800
$Comp
L R R2
U 1 1 59E4F88A
P 3400 2500
F 0 "R2" V 3480 2500 50  0000 C CNN
F 1 "1M" V 3400 2500 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3330 2500 50  0001 C CNN
F 3 "" H 3400 2500 50  0001 C CNN
	1    3400 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 2350 3400 2300
Connection ~ 3400 2300
Wire Wire Line
	3400 2650 3400 2700
Connection ~ 3400 2700
$Comp
L Conn_01x06 J3
U 1 1 59E4F9BD
P 2400 4000
F 0 "J3" H 2400 4300 50  0000 C CNN
F 1 "Debug" H 2400 3600 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06_Pitch2.54mm" H 2400 4000 50  0001 C CNN
F 3 "" H 2400 4000 50  0001 C CNN
	1    2400 4000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2600 3800 2900 3800
Connection ~ 2750 3800
Wire Wire Line
	3900 4350 3100 4350
Wire Wire Line
	3100 4350 3100 4200
Wire Wire Line
	3100 4200 2600 4200
Wire Wire Line
	3900 4250 3200 4250
Wire Wire Line
	3200 4250 3200 4100
Wire Wire Line
	3200 4100 2600 4100
Wire Wire Line
	2750 3200 2750 3350
Wire Wire Line
	3900 3500 2900 3500
Wire Wire Line
	2900 3500 2900 3800
Wire Wire Line
	2600 3900 2650 3900
Wire Wire Line
	2650 3900 2650 3300
Wire Wire Line
	2650 3300 2750 3300
Connection ~ 2750 3300
Wire Wire Line
	2600 4000 2750 4000
Wire Wire Line
	2750 4000 2750 4450
$Comp
L C C3
U 1 1 59E51337
P 5000 7000
F 0 "C3" H 5025 7100 50  0000 L CNN
F 1 "100nF" H 5025 6900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 5038 6850 50  0001 C CNN
F 3 "" H 5000 7000 50  0001 C CNN
	1    5000 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 6750 5000 6850
Wire Wire Line
	5000 7150 5000 7250
Connection ~ 5000 6800
Connection ~ 5000 7200
NoConn ~ 2600 4300
Text HLabel 3600 4650 0    60   Output ~ 0
CAN_Tx
Text HLabel 3600 4550 0    60   Input ~ 0
CAN_Rx
Text HLabel 3600 4750 0    60   Output ~ 0
CAN_STBY
Wire Wire Line
	3900 4750 3600 4750
Wire Wire Line
	3600 4650 3900 4650
Wire Wire Line
	3600 4850 3900 4850
$Comp
L C C2
U 1 1 59E9FB94
P 2750 2700
F 0 "C2" H 2775 2800 50  0000 L CNN
F 1 "20pF" H 2775 2600 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 2788 2550 50  0001 C CNN
F 3 "" H 2750 2700 50  0001 C CNN
	1    2750 2700
	0    1    1    0   
$EndComp
$Comp
L +3.3V #PWR051
U 1 1 59EBAD8E
P 5600 1300
F 0 "#PWR051" H 5600 1150 50  0001 C CNN
F 1 "+3.3V" H 5600 1440 50  0000 C CNN
F 2 "" H 5600 1300 50  0001 C CNN
F 3 "" H 5600 1300 50  0001 C CNN
	1    5600 1300
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR052
U 1 1 59EBADD5
P 5900 1300
F 0 "#PWR052" H 5900 1150 50  0001 C CNN
F 1 "+3.3V" H 5900 1440 50  0000 C CNN
F 2 "" H 5900 1300 50  0001 C CNN
F 3 "" H 5900 1300 50  0001 C CNN
	1    5900 1300
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR053
U 1 1 59EBB64E
P 5000 6750
F 0 "#PWR053" H 5000 6600 50  0001 C CNN
F 1 "+3.3V" H 5000 6890 50  0000 C CNN
F 2 "" H 5000 6750 50  0001 C CNN
F 3 "" H 5000 6750 50  0001 C CNN
	1    5000 6750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR054
U 1 1 59EBB6FB
P 5500 6300
F 0 "#PWR054" H 5500 6050 50  0001 C CNN
F 1 "GND" H 5500 6150 50  0000 C CNN
F 2 "" H 5500 6300 50  0001 C CNN
F 3 "" H 5500 6300 50  0001 C CNN
	1    5500 6300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR055
U 1 1 59EBB773
P 5900 6300
F 0 "#PWR055" H 5900 6050 50  0001 C CNN
F 1 "GND" H 5900 6150 50  0000 C CNN
F 2 "" H 5900 6300 50  0001 C CNN
F 3 "" H 5900 6300 50  0001 C CNN
	1    5900 6300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR056
U 1 1 59EBB7B3
P 5000 7250
F 0 "#PWR056" H 5000 7000 50  0001 C CNN
F 1 "GND" H 5000 7100 50  0000 C CNN
F 2 "" H 5000 7250 50  0001 C CNN
F 3 "" H 5000 7250 50  0001 C CNN
	1    5000 7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR057
U 1 1 59EBBDBE
P 2750 4450
F 0 "#PWR057" H 2750 4200 50  0001 C CNN
F 1 "GND" H 2750 4300 50  0000 C CNN
F 2 "" H 2750 4450 50  0001 C CNN
F 3 "" H 2750 4450 50  0001 C CNN
	1    2750 4450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR058
U 1 1 59EBBE36
P 2500 2800
F 0 "#PWR058" H 2500 2550 50  0001 C CNN
F 1 "GND" H 2500 2650 50  0000 C CNN
F 2 "" H 2500 2800 50  0001 C CNN
F 3 "" H 2500 2800 50  0001 C CNN
	1    2500 2800
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR059
U 1 1 59EBBE6B
P 2750 3200
F 0 "#PWR059" H 2750 3050 50  0001 C CNN
F 1 "+3.3V" H 2750 3340 50  0000 C CNN
F 2 "" H 2750 3200 50  0001 C CNN
F 3 "" H 2750 3200 50  0001 C CNN
	1    2750 3200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR060
U 1 1 59EBC4B0
P 5150 1500
F 0 "#PWR060" H 5150 1250 50  0001 C CNN
F 1 "GND" H 5150 1350 50  0000 C CNN
F 2 "" H 5150 1500 50  0001 C CNN
F 3 "" H 5150 1500 50  0001 C CNN
	1    5150 1500
	1    0    0    -1  
$EndComp
Text HLabel 3600 4850 0    60   Output ~ 0
SPI_Out
Text HLabel 8300 2350 2    60   Input ~ 0
SPI_In
Text HLabel 3600 5050 0    60   Output ~ 0
SPI_Clock
Text HLabel 3600 5250 0    60   Output ~ 0
PWM
Text HLabel 3600 4450 0    60   Input ~ 0
~Fault~
Text HLabel 3600 4950 0    60   Output ~ 0
~CS0~
Text HLabel 3600 3250 0    60   Output ~ 0
Output_Wake
Wire Wire Line
	3600 3250 3900 3250
Wire Wire Line
	8300 2350 7700 2350
Wire Wire Line
	3600 5050 3900 5050
Wire Wire Line
	3600 2950 3900 2950
Wire Wire Line
	3600 4450 3900 4450
Text HLabel 3600 5150 0    60   Input ~ 0
CSNS
Wire Wire Line
	3600 5150 3900 5150
Wire Wire Line
	3900 4950 3600 4950
Wire Wire Line
	3900 4550 3600 4550
Text HLabel 8300 2750 2    60   Output ~ 0
LED0
Text HLabel 8300 2850 2    60   Output ~ 0
LED1
$Comp
L C C4
U 1 1 59EF6095
P 5150 1300
F 0 "C4" H 5175 1400 50  0000 L CNN
F 1 "10uF" H 5175 1200 50  0000 L CNN
F 2 "Capacitors_SMD:C_1210" H 5188 1150 50  0001 C CNN
F 3 "" H 5150 1300 50  0001 C CNN
	1    5150 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 6800 5000 6800
Wire Wire Line
	5350 7200 5000 7200
NoConn ~ 3900 4150
NoConn ~ 3900 4050
NoConn ~ 7700 3050
NoConn ~ 7700 3150
NoConn ~ 3900 3950
NoConn ~ 7700 2950
Text HLabel 3600 2950 0    60   Input ~ 0
Sync
$Comp
L PIC24HJ128GP504 U5
U 1 1 59E38B9D
P 5800 3750
F 0 "U5" H 4100 5900 50  0000 L CNN
F 1 "PIC24HJ128GP504" H 4100 5800 50  0000 L CNN
F 2 "Housings_QFP:PQFP-44_10x10mm_Pitch0.8mm" H 5800 3850 50  0001 C CNN
F 3 "" H 5800 3950 50  0001 C CNN
	1    5800 3750
	1    0    0    -1  
$EndComp
NoConn ~ 3900 2650
NoConn ~ 3900 3150
Wire Wire Line
	8300 2750 7700 2750
Wire Wire Line
	8300 2850 7700 2850
Text HLabel 8300 2650 2    60   Output ~ 0
Power_On
Wire Wire Line
	7700 2650 8300 2650
NoConn ~ 3900 3750
NoConn ~ 3900 3850
NoConn ~ 3900 2250
NoConn ~ 3900 2350
NoConn ~ 7700 2550
NoConn ~ 3900 3050
Wire Wire Line
	3600 5250 3900 5250
NoConn ~ 7700 2450
NoConn ~ 7700 2250
$EndSCHEMATC
