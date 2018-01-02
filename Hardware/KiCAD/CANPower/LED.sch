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
Text HLabel 2600 3050 0    60   Input ~ 0
LED0
$Comp
L Conn_01x03 J8
U 1 1 59FDAD2C
P 5950 3700
F 0 "J8" H 5950 3900 50  0000 C CNN
F 1 "LED" H 5950 3500 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 5950 3700 50  0001 C CNN
F 3 "" H 5950 3700 50  0001 C CNN
	1    5950 3700
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR046
U 1 1 59FDAD60
P 4550 4600
F 0 "#PWR046" H 4550 4450 50  0001 C CNN
F 1 "+5V" H 4550 4740 50  0000 C CNN
F 2 "" H 4550 4600 50  0001 C CNN
F 3 "" H 4550 4600 50  0001 C CNN
	1    4550 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 3600 5750 3600
$Comp
L R R17
U 1 1 59FDAD7D
P 4800 3450
F 0 "R17" V 4880 3450 50  0000 C CNN
F 1 "150R" V 4800 3450 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 4730 3450 50  0001 C CNN
F 3 "" H 4800 3450 50  0001 C CNN
	1    4800 3450
	-1   0    0    1   
$EndComp
$Comp
L R R16
U 1 1 59FDADC6
P 3100 3450
F 0 "R16" V 3180 3450 50  0000 C CNN
F 1 "150R" V 3100 3450 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3030 3450 50  0001 C CNN
F 3 "" H 3100 3450 50  0001 C CNN
	1    3100 3450
	-1   0    0    1   
$EndComp
Wire Wire Line
	4800 3700 5750 3700
Wire Wire Line
	3100 3800 5750 3800
$Comp
L C C29
U 1 1 59FDAE25
P 4550 4850
F 0 "C29" H 4575 4950 50  0000 L CNN
F 1 "100nF" H 4575 4750 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 4588 4700 50  0001 C CNN
F 3 "" H 4550 4850 50  0001 C CNN
	1    4550 4850
	1    0    0    -1  
$EndComp
Text HLabel 4300 3050 0    60   Input ~ 0
LED1
$Comp
L GND #PWR047
U 1 1 5A15E1AA
P 5500 3350
F 0 "#PWR047" H 5500 3100 50  0001 C CNN
F 1 "GND" H 5500 3200 50  0000 C CNN
F 2 "" H 5500 3350 50  0001 C CNN
F 3 "" H 5500 3350 50  0001 C CNN
	1    5500 3350
	1    0    0    -1  
$EndComp
$Comp
L BSS83P Q1
U 1 1 5A15E473
P 3000 3050
F 0 "Q1" H 3200 3125 50  0000 L CNN
F 1 "BSS83P" H 3200 3050 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 3200 2975 50  0001 L CIN
F 3 "" H 3000 3050 50  0001 L CNN
	1    3000 3050
	1    0    0    1   
$EndComp
$Comp
L BSS83P Q2
U 1 1 5A15E58A
P 4700 3050
F 0 "Q2" H 4900 3125 50  0000 L CNN
F 1 "BSS83P" H 4900 3050 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 4900 2975 50  0001 L CIN
F 3 "" H 4700 3050 50  0001 L CNN
	1    4700 3050
	1    0    0    1   
$EndComp
$Comp
L R R19
U 1 1 5A15E6E2
P 2700 2850
F 0 "R19" V 2780 2850 50  0000 C CNN
F 1 "1M" V 2700 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 2630 2850 50  0001 C CNN
F 3 "" H 2700 2850 50  0001 C CNN
	1    2700 2850
	-1   0    0    1   
$EndComp
$Comp
L R R20
U 1 1 5A15E72C
P 4400 2850
F 0 "R20" V 4480 2850 50  0000 C CNN
F 1 "1M" V 4400 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 4330 2850 50  0001 C CNN
F 3 "" H 4400 2850 50  0001 C CNN
	1    4400 2850
	-1   0    0    1   
$EndComp
$Comp
L +5V #PWR048
U 1 1 5A15EA1B
P 3100 2600
F 0 "#PWR048" H 3100 2450 50  0001 C CNN
F 1 "+5V" H 3100 2740 50  0000 C CNN
F 2 "" H 3100 2600 50  0001 C CNN
F 3 "" H 3100 2600 50  0001 C CNN
	1    3100 2600
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR049
U 1 1 5A15EA8A
P 4800 2600
F 0 "#PWR049" H 4800 2450 50  0001 C CNN
F 1 "+5V" H 4800 2740 50  0000 C CNN
F 2 "" H 4800 2600 50  0001 C CNN
F 3 "" H 4800 2600 50  0001 C CNN
	1    4800 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 2600 4800 2850
Wire Wire Line
	3100 2600 3100 2850
Wire Wire Line
	2600 3050 2800 3050
Wire Wire Line
	2700 3050 2700 3000
Wire Wire Line
	2700 2700 2700 2650
Wire Wire Line
	2700 2650 3100 2650
Connection ~ 3100 2650
Wire Wire Line
	4300 3050 4500 3050
Wire Wire Line
	4400 3050 4400 3000
Wire Wire Line
	4400 2700 4400 2650
Wire Wire Line
	4400 2650 4800 2650
Connection ~ 4800 2650
Wire Wire Line
	3100 3300 3100 3250
Wire Wire Line
	4800 3250 4800 3300
Wire Wire Line
	4800 3600 4800 3700
Wire Wire Line
	3100 3800 3100 3600
Connection ~ 4400 3050
Connection ~ 2700 3050
$Comp
L GND #PWR050
U 1 1 5A161139
P 4550 5100
F 0 "#PWR050" H 4550 4850 50  0001 C CNN
F 1 "GND" H 4550 4950 50  0000 C CNN
F 2 "" H 4550 5100 50  0001 C CNN
F 3 "" H 4550 5100 50  0001 C CNN
	1    4550 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 4600 4550 4700
Wire Wire Line
	4550 5000 4550 5100
Wire Wire Line
	5650 3600 5650 3250
Wire Wire Line
	5650 3250 5500 3250
Wire Wire Line
	5500 3250 5500 3350
$EndSCHEMATC
