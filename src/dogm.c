#include "dogm.h"
#include <util/delay.h>

bool dmInit()
{
    DM_DDR |= _BV(DM_SI) | _BV(DM_CLK) | _BV(DM_RS) | _BV(DM_CSB);

    DM_PORT |= _BV(DM_CSB);
    DM_PORT |= _BV(DM_CLK);
    DM_PORT &= ~_BV(DM_RS);
    DM_PORT &= ~_BV(DM_SI);

    if(DM_MODE == SPI)return dmInitSPI();
    return false;
}

bool dmInitSPI()
{
    bool threeLine = true;
    bool doubleHight = false;
    bool iconOn = true;
    bool boosterOn = true;
    
    bool c5 = true;
    bool c4 = false;
    bool c3 = true;
    bool c2 = true;
    bool c1 = false;
    bool c0 = false;
    


    bool followerOn = true;
    bool Rab2 = false;
    bool Rab1 = true;
    bool Rab0 = false;
    
    bool displayOn = true;
    bool cursorOn = true;
    bool blinkOn = true;

    _delay_ms(40);
    
    //function set 0
    dmSendCommand( bits(0,0,1,1,  threeLine, doubleHight, 0, 0)); //last 2: is[2:1]
    
    //function set 1
    dmSendCommand( bits(0,0,1,1,  threeLine, doubleHight, 0, 1)); //last 2: is[2:1]
    
    //bias
    dmSendCommand( bits(0,0,0,1,  0,1,0,0)); 

    //contrast
    dmSendCommand( bits(0,1,1,1,  c3, c2, c1, c0));

    //power/icon/contrast control
    dmSendCommand( bits(0,1,0,1,  iconOn, boosterOn, c5, c4));
    
    //Follower control
    dmSendCommand( bits(0,1,1,0,  followerOn, Rab2, Rab1, Rab0));
    _delay_ms(210); 
    
    dmDisplayMode( displayOn, cursorOn, blinkOn);
    
    dmEntryMode(true, false);
    
    return true;
}

/*

  RS H: data, 
  RS L: command

  send data:
  CSB LOW

  DATA PIN SET
  CLK PIN LOW
  CLK PIN HIGH
  X8
  
  CSB HIGH
 */

void dmSendString(const char* str)
{
    byte i = 0;
    while(str[i] != '\0')
    {
        dmSendData(str[i++]);
    }
}


void dmSendData(byte data)
{
    DM_PORT |= _BV(DM_RS);
    dmSendByte(data);
    _delay_us(30); 
}

void dmSendCommand(byte data)
{
    DM_PORT &= ~_BV(DM_RS);
    dmSendByte(data);
    _delay_us(30);
}


void dmSendByte(byte data)
{
    //CSB LOW
    DM_PORT &= ~_BV(DM_CSB);

    //SEND bits
    byte i = 0;
    for (;i<8;i++)
    {
        bool bit = data & (0b10000000 >> i);
        if(bit)
            DM_PORT |= _BV(DM_SI);
        else
            DM_PORT &= ~_BV(DM_SI);
        
        //clock it in
        DM_PORT &= ~_BV(DM_CLK);
        _delay_us(1);
        DM_PORT |= _BV(DM_CLK);
    }

    //CSB HIGH
    DM_PORT |= _BV(DM_CSB);
}

byte bits(bool b7, bool b6, bool b5, bool b4, bool b3, bool b2, bool b1, bool b0)
{
    return (b7<<7 | b6<<6 | b5<<5 | b4<<4 | b3<<3 | b2<<2 | b1<<1 | b0<<0);
}

void dmSetCursor(byte x, byte y)
{
    byte addr = x + (y<<4);
    if (addr >=48)addr=47;
    addr |= 0b10000000; //1<<7;

    dmSendCommand(addr);
}

void dmClear(byte fromX, byte fromY, byte toX, byte toY)
{
    int y = fromY;
    for (;y<=toY;y++)
    {
        int x = fromX;
        for (;x<=toX;x++)
        {
            dmPutc(x,y,' ');
        }
    }
}

void dmClearDisplay()
{
    //clear display
    dmSendCommand(1); 
    _delay_ms(2);
    
    //return home
    dmSendCommand( bits(0,0,0,0,  0, 0,1,0));
    _delay_ms(2);
}

void dmEntryMode(bool moveCursorRight, bool shiftDisplay)
{
    dmSendCommand( bits(0,0,0,0,  0, 1, moveCursorRight, shiftDisplay));
}

void dmDoubleFont(bool on, bool doubleOnFirstRow)
{
    dmSendCommand( bits(0,0,1,1,  true, on, 1, 0)); 
    dmSendCommand( bits(0,0,0,1,  doubleOnFirstRow, 0, 0, 0));
}

void dmPrint(byte x, byte y, const char* str)
{
    dmSetCursor(x,y);
    dmSendString(str);
}

void dmPutc(byte x, byte y, const char c)
{
    dmSetCursor(x,y);
    dmSendData(c);
}

void dmDisplayMode(bool displayOn, bool cursorOn, bool blinkOn)
{
    dmSendCommand( bits(0,0,0,0,  1, displayOn, cursorOn, blinkOn));
}
