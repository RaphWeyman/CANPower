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
Sheet 3 6
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
	5250 4200 5250 4300
$Comp
L +5V #PWR024
U 1 1 59E5361A
P 5250 3250
F 0 "#PWR024" H 5250 3100 50  0001 C CNN
F 1 "+5V" H 5250 3390 50  0000 C CNN
F 2 "" H 5250 3250 50  0001 C CNN
F 3 "" H 5250 3250 50  0001 C CNN
	1    5250 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 3400 5250 3250
Wire Wire Line
	4750 3900 4300 3900
Wire Wire Line
	4300 3900 4300 3450
$Comp
L C C19
U 1 1 59E53651
P 4700 5100
F 0 "C19" H 4725 5200 50  0000 L CNN
F 1 "100nF" H 4725 5000 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 4738 4950 50  0001 C CNN
F 3 "" H 4700 5100 50  0001 C CNN
	1    4700 5100
	1    0    0    -1  
$EndComp
$Comp
L C C20
U 1 1 59E53682
P 5100 5100
F 0 "C20" H 5125 5200 50  0000 L CNN
F 1 "100nF" H 5125 5000 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 5138 4950 50  0001 C CNN
F 3 "" H 5100 5100 50  0001 C CNN
	1    5100 5100
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR025
U 1 1 59E536F4
P 4700 4900
F 0 "#PWR025" H 4700 4750 50  0001 C CNN
F 1 "+5V" H 4700 5040 50  0000 C CNN
F 2 "" H 4700 4900 50  0001 C CNN
F 3 "" H 4700 4900 50  0001 C CNN
	1    4700 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 4950 4700 4900
Wire Wire Line
	5100 4950 5100 4900
Wire Wire Line
	4700 5250 4700 5350
Wire Wire Line
	4700 5300 5100 5300
Wire Wire Line
	5100 5300 5100 5250
Connection ~ 4700 5300
Text HLabel 3850 3600 0    60   Input ~ 0
CAN_Tx
Text HLabel 3850 3700 0    60   Output ~ 0
CAN_Rx
Text HLabel 3850 4000 0    60   Input ~ 0
CAN_STBY
Wire Wire Line
	3850 3600 4750 3600
Wire Wire Line
	4750 3700 3850 3700
Wire Wire Line
	4750 4000 3850 4000
$Comp
L R R8
U 1 1 59E596A4
P 6050 3800
F 0 "R8" V 6130 3800 50  0000 C CNN
F 1 "120R" V 6050 3800 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 5980 3800 50  0001 C CNN
F 3 "" H 6050 3800 50  0001 C CNN
	1    6050 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 3700 5850 3700
Wire Wire Line
	5850 3700 5850 3600
Wire Wire Line
	6050 3600 6050 3650
Wire Wire Line
	6050 3950 6050 4000
Wire Wire Line
	5850 4000 6550 4000
Wire Wire Line
	5850 4000 5850 3900
Wire Wire Line
	5850 3900 5750 3900
$Comp
L GND #PWR026
U 1 1 59EBA7C6
P 5250 4300
F 0 "#PWR026" H 5250 4050 50  0001 C CNN
F 1 "GND" H 5250 4150 50  0000 C CNN
F 2 "" H 5250 4300 50  0001 C CNN
F 3 "" H 5250 4300 50  0001 C CNN
	1    5250 4300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR027
U 1 1 59EBA8B4
P 4700 5350
F 0 "#PWR027" H 4700 5100 50  0001 C CNN
F 1 "GND" H 4700 5200 50  0000 C CNN
F 2 "" H 4700 5350 50  0001 C CNN
F 3 "" H 4700 5350 50  0001 C CNN
	1    4700 5350
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR028
U 1 1 59EBA8C9
P 5100 4900
F 0 "#PWR028" H 5100 4750 50  0001 C CNN
F 1 "+3.3V" H 5100 5040 50  0000 C CNN
F 2 "" H 5100 4900 50  0001 C CNN
F 3 "" H 5100 4900 50  0001 C CNN
	1    5100 4900
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR029
U 1 1 59EBA9EB
P 4300 3450
F 0 "#PWR029" H 4300 3300 50  0001 C CNN
F 1 "+3.3V" H 4300 3590 50  0000 C CNN
F 2 "" H 4300 3450 50  0001 C CNN
F 3 "" H 4300 3450 50  0001 C CNN
	1    4300 3450
	1    0    0    -1  
$EndComp
Connection ~ 6050 4000
Connection ~ 6050 3600
$Comp
L MCP2562-E/SN U3
U 1 1 59EF2A6E
P 5250 3800
F 0 "U3" H 4850 4150 50  0000 L CNN
F 1 "MCP2562-E/SN" H 5350 4150 50  0000 L CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 5250 3300 50  0001 C CIN
F 3 "" H 5250 3800 50  0001 C CNN
	1    5250 3800
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x02 J1
U 1 1 5A19996E
P 7150 3850
F 0 "J1" H 7150 3950 50  0000 C CNN
F 1 "CAN" H 7150 3650 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 7150 3850 50  0001 C CNN
F 3 "" H 7150 3850 50  0001 C CNN
	1    7150 3850
	1    0    0    1   
$EndComp
Wire Wire Line
	6550 4000 6550 3850
Wire Wire Line
	6550 3850 6950 3850
Wire Wire Line
	5850 3600 6550 3600
Wire Wire Line
	6550 3600 6550 3750
Wire Wire Line
	6550 3750 6950 3750
Text Notes 6100 4550 0    60   ~ 0
Fit R8 only if CAN termination required
$EndSCHEMATC
