#!/bin/bash

#disable jtag (use port C for gpio)
#avrdude -p m644p -c dapa -U hfuse:w:0xD9:m

#internal 8mhz CKDIV8 disable
#avrdude -p m644p -c dapa -U lfuse:w:0xE2:m

#int 128khz osc/8
#avrdude -p m644p -c dapa -U lfuse:w:0x63:m


#USING

#ext > 20mhz crystal
avrdude -p m644p -c dapa -U lfuse:w:0xF7:m

#disable jtag + enable eeprom save
avrdude -p m644p -c dapa -U hfuse:w:0xD1:m