# High Score Breathalyzer

An Atmega328p based embedded systems project to create your very own High Score Breathalyzer!

Delight your friends by having them compete to set the high score BAC for your breathalyzer.  As a fun party toy, this circuit is meant to augment the enjoyment of drinking with friends by throwing in some friendly competition.

## Features:
- Measure your BAC with the MQ-3: an accurate, analog ethanol concentration sensor.
- Control your breathalyzer using the High Score Breathalyzer app on your phone.
- Save your score to the scoreboard on the app.
- Save your high scores across devices by saving to the Atmega328p's FIFO.

## Disclaimers:
- This code is very preliminary, and there are a few known bugs.
- I will include circuit diagrams and pin-outs.
- I will create a step-by-step instruction manual on how I assembled and flashed the circuit.
- **This is not an excuse for heavy drinking, and I do not accept any liability for whoever uses these resources**.
- _However, this toy **could** be used as a tool to measure BAC and promote healthy drinking habits (e.g. stop someone from driving home)._
- Enjoy! These resources are for anybody! Modify, create, or reference, but I would appreciate a little credit when necessary :wink:.

## Project Overview
### Module Layout
- Microcontroller (Atmega8)
- Bluetooth (HC-06)
- Ethanol Sensor (MQ-3)
- Power Source (5VDC)
- Flashing Port (USBASP)

#### Microcontroller
At the heart of the board is the **Atmega8** made by Microchip:

- [Atmega8](https://www.microchip.com/wwwproducts/en/ATmega8)
- [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2486-8-bit-AVR-microcontroller-ATmega8_L_datasheet.pdf)

On-board is the 8-bit microprocessor, 8kb Flash memory, 1kb SRAM, and 512b EEPROM.  The 512b EEPROM is used to store score data between power cycles.

#### Bluetooth
The Bluetooth communication on the high score breathalyzer is done by the HC-06 (I got mine from Amazon):

- [Datasheet](http://silabs.org.ua/bc4/hc06.pdf)

The HC-06 is a widely-used, low power Bluetooth module.  This module is controlled by the Atmega8 via the USART protocol.  The default configuration (as specified in the code):

- BAUD: 9600Hz
- Name / ID: "HiScoreBreathalyzer"
- Pairing code: 0000

However, you can change these settings by modifying some macros in the "./Include/BluetoothControl.h" header file.

#### Ethanol Sensor
The ethanol sensor consists of the analog MQ-3 paired with a voltage divider, and it is fed into the Atmega8's second analog to digital converter.  It is read into the program and is converted to BAC using a conversion algorithm I found from another breathalyzer project [here](https://www.hackster.io/ShawnHymel/diy_breathalyzer-1efe13).

- [Datasheet](https://cdn.sparkfun.com/datasheets/Sensors/Biometric/MQ-3%20ver1.3%20-%20Manual.pdf)

##### Conversion Algorithm
Assuming a 4.7kOhm resistor from B1 to GND and a 5V reference voltage.
```
Let v = (The measured voltage)
PPM = 150.4351049 * v^5 - 2244.75988 * v^4 + 13308.5139 * v^3 - 39136.08594 * v^2 + 57082.6258 * v - 32982.05333
BAC = PPM / 2600
if BAC < 70
    BAC = 0
else if BAC > 500
    return ERROR
return BAC
```

One trick I found after testing this High Score Breathalyzer at multiple parties is since really drunk people (those most likely to get the high score) tend to also drool _a lot_.  In my initial tests, one individual drooled into the MQ-3 sensor which ruined it.  To prevent this problem, I've found it best to use some sort of tubing that directs the air to the MQ-3 instead of just having people blow directly onto it.  Personally, I use the plastic casing of an empty rescue inhaler, but anything that redirects airflow should work.

#### Power Source
For the power source, you can use anything from a 9V battery to a LiPO as long as it is regulated through a 5V voltage regulator so that it plays well with the Atmega8.

This project is also programmed to handle battery levels which is collected upon command and outputted via the Bluetooth module.  I use a voltage divider from the source battery to the first analog to digital converter on the Atmega8.

#### Flashing Port
You may choose to have this feature on the High Score Breathalyzer or not, but I like to add some header pins on the Atmega8 leading the pins required by my flashing tool (I use a USBASP).  This just makes it easier to upload updated code to the flash memory on the Atmega8.  However, this project works just fine without it.
