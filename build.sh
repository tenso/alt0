#!/bin/bash
scons -j3 &&\
avr-objcopy -j .text -j .data -O ihex prog prog.hex && \
avrdude -p m644p -c dapa -U flash:w:prog.hex:a && avr-size -C prog
