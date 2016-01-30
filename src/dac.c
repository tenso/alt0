#include "dac.h"

#include <avr/interrupt.h>
#include <util/delay.h>
/*dac handling*/

void dacInit()
{
    //enabe ctrl outputs
    DAC_CTRL_DDR |= _BV(DAC_CS_1_PIN) | _BV(DAC_WRE_PIN) | _BV(DAC_SEL_PIN);
    DAC_CTRL_DDR_CS_0 |=  _BV(DAC_CS_0_PIN);

    //all output
    DAC_DATA_DDR = 0xff;

    dacSetChip(DAC_CHIP_NONE);
    dacSetWrite(false);
    dacSetSelected(DAC_A);
}

void dacWrite(byte data)
{
    DAC_DATA_PORT = data;
}

void dacSetChip(byte chip)
{
    if (chip == DAC_CHIP_0)
    {
        DAC_CTRL_PORT_CS_0 &= ~_BV(DAC_CS_0_PIN);
        DAC_CTRL_PORT |= _BV(DAC_CS_1_PIN);
    }
    else if (chip == DAC_CHIP_1)
    {
        DAC_CTRL_PORT &= ~_BV(DAC_CS_1_PIN);
        DAC_CTRL_PORT_CS_0 |= _BV(DAC_CS_0_PIN);
    }
    else //NONE
    {
        DAC_CTRL_PORT_CS_0 |= _BV(DAC_CS_0_PIN);
        DAC_CTRL_PORT |= _BV(DAC_CS_1_PIN);
    }
}

void dacSetSelected(byte dac)
{
    if (dac == DAC_A)
        DAC_CTRL_PORT &= ~_BV(DAC_SEL_PIN);
    else
        DAC_CTRL_PORT |= _BV(DAC_SEL_PIN);
}

void dacSetWrite(bool on)
{
    if (on)
    {
        DAC_CTRL_PORT &= ~_BV(DAC_WRE_PIN);
    }
    else
    {
        DAC_CTRL_PORT |= _BV(DAC_WRE_PIN);
    }
}
void dacOut(byte chip, byte dac, byte data)
{
    //cli();
    /*dacSetWrite(false);
    dacSetChip(chip);
    dacSetSelected(dac);
    dacWrite(data);
    dacSetWrite(true);*/
    //sei();
    
    dacSetChip(chip);
    dacSetSelected(dac);
    dacSetWrite(true);
    dacWrite(data);    
    dacSetWrite(false);
}
