# High Score Breathalyzer

An Atmega328p based embedded systems project to create your very own High Score Breathalyzer!

Delight your friends by having them compete to set the high score BAC for your breathalyzer.  As a fun party toy, this circuit is meant to augment the enjoyment of drinking with friends by throwing in some friendly competition.

## Features:
- Measure your BAC with the MQ-3: an accurate, analog ethanol concentration sensor.
- Test your score against the reigning champion, and celebrate their defeat with a fun, 5-LED animation and OLED display.
- Save your high score between power cycles by saving to the Atmega328p's FIFO.

## Disclaimers:
- This code is very preliminary, and there are a few known bugs.
  - E.G. At the moment, the high score does not yet save to FIFO
- This code is very poorly organized.  I plan on pushing some updates in the future completely reorganizing the program (This was my very first Atmel based project, and I was just beginning to teach myself C)
- I will include circuit diagrams and pin-outs
- I will create a step-by-step instruction manuel on how I assembled and flashed the circuit
- **This is not an excuse for heavy drinking, and I do not accept any liability for whoever uses these resources**
- _However, this toy **could** be used as a tool to measure BAC and promote healthy drinking habits (stop someone from driving home)._
- Enjoy! These resources are for anybody! Modify, create, or reference, but I would appreciate a little credit when necessary :wink:.


#### Note:
I used Tinusaur's SSD1306xLED I2C OLED framework to control my OLED display.

Check it out [here](https://tinusaur.org/projects/ssd1306xled/)!
