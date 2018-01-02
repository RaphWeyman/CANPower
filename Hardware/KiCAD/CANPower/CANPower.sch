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
Sheet 1 6
Title "CANPower"
Date ""
Rev "1.0"
Comp "Raph Weyman"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 8350 4550 2100 1650
U 59E38B1C
F0 "Power" 60
F1 "CANPower-Power.sch" 60
F2 "Power_On" I L 8350 5850 60 
$EndSheet
$Sheet
S 2150 5700 2000 1300
U 59E53567
F0 "CAN" 60
F1 "CANPower-CAN.sch" 60
F2 "CAN_Tx" I L 2150 5900 60 
F3 "CAN_Rx" O L 2150 6050 60 
F4 "CAN_STBY" I L 2150 6200 60 
$EndSheet
Wire Wire Line
	2200 4150 2000 4150
Wire Wire Line
	2000 4150 2000 5900
Wire Wire Line
	2000 5900 2150 5900
Wire Wire Line
	2150 6050 1900 6050
Wire Wire Line
	1900 6050 1900 4050
Wire Wire Line
	1900 4050 2200 4050
Wire Wire Line
	2150 6200 1800 6200
Wire Wire Line
	1800 6200 1800 3950
Wire Wire Line
	1800 3950 2200 3950
$Sheet
S 8350 2200 2300 1750
U 59E9EC5C
F0 "Switches" 60
F1 "CANPower-Switches.sch" 60
F2 "SPI_Clock" I L 8350 3600 60 
F3 "SPI_In" I L 8350 3800 60 
F4 "SPI_Out" O L 8350 3700 60 
F5 "~CS0~" I L 8350 3100 60 
F6 "~Reset~" I L 8350 2800 60 
F7 "~Fault~" O L 8350 3200 60 
F8 "PWM" I L 8350 3500 60 
F9 "Sync" O L 8350 3300 60 
F10 "CSNS" O L 8350 3400 60 
$EndSheet
Wire Wire Line
	8350 3800 7450 3800
Wire Wire Line
	7450 3800 7450 3400
Wire Wire Line
	7450 3400 4300 3400
Wire Wire Line
	8350 3700 7500 3700
Wire Wire Line
	7500 3700 7500 3300
Wire Wire Line
	7500 3300 4300 3300
Wire Wire Line
	8350 3600 7550 3600
Wire Wire Line
	7550 3600 7550 3200
Wire Wire Line
	7550 3200 4300 3200
Wire Wire Line
	8350 3500 7600 3500
Wire Wire Line
	7600 3500 7600 3100
Wire Wire Line
	7600 3100 4300 3100
Wire Wire Line
	8350 3400 7650 3400
Wire Wire Line
	7650 3400 7650 3000
Wire Wire Line
	7650 3000 4300 3000
Wire Wire Line
	8350 3300 7700 3300
Wire Wire Line
	7700 3300 7700 2900
Wire Wire Line
	7700 2900 4300 2900
Wire Wire Line
	8350 3200 7750 3200
Wire Wire Line
	7750 3200 7750 2800
Wire Wire Line
	7750 2800 4300 2800
Wire Wire Line
	8350 3100 7800 3100
Wire Wire Line
	7800 3100 7800 2700
Wire Wire Line
	7800 2700 4300 2700
Wire Wire Line
	8350 2800 7850 2800
Wire Wire Line
	7850 2800 7850 2500
Wire Wire Line
	7850 2500 4300 2500
$Sheet
S 5950 4500 900  900 
U 59FDA547
F0 "LED" 60
F1 "LED.sch" 60
F2 "LED0" I L 5950 5050 60 
F3 "LED1" I L 5950 4950 60 
$EndSheet
Wire Wire Line
	4300 4000 4950 4000
Wire Wire Line
	4950 4000 4950 5050
Wire Wire Line
	4950 5050 5950 5050
Wire Wire Line
	5950 4950 5000 4950
Wire Wire Line
	5000 4950 5000 3900
Wire Wire Line
	5000 3900 4300 3900
Wire Wire Line
	4300 4200 4650 4200
Wire Wire Line
	4650 4200 4650 5850
Wire Wire Line
	4650 5850 8350 5850
$Sheet
S 2200 1900 2100 2450
U 59E38AF5
F0 "CPU" 60
F1 "CANPower-CPU.sch" 60
F2 "CAN_Tx" O L 2200 4150 60 
F3 "CAN_Rx" I L 2200 4050 60 
F4 "CAN_STBY" O L 2200 3950 60 
F5 "SPI_Out" O R 4300 3400 60 
F6 "SPI_In" I R 4300 3300 60 
F7 "SPI_Clock" O R 4300 3200 60 
F8 "PWM" O R 4300 3100 60 
F9 "CSNS" I R 4300 3000 60 
F10 "Sync" I R 4300 2900 60 
F11 "~Fault~" I R 4300 2800 60 
F12 "~CS0~" O R 4300 2700 60 
F13 "Output_Wake" O R 4300 2500 60 
F14 "LED0" O R 4300 4000 60 
F15 "LED1" O R 4300 3900 60 
F16 "Power_On" O R 4300 4200 60 
$EndSheet
$EndSCHEMATC
