#include "input.h"
//#include "dogm.h"
#include "xio.h"
bool _buttonOn = false;
uint _keyTicks = 0;

bool _encLastA = 0;
bool _encLastB = 0;
int _encTicks = 0;
uint _encSpeed = 1;


void (*_keyHandleDown)(int id) = 0;
void (*_keyHandleUp)(int id) = 0;
void (*_encHandleMove)(int id, int speed) = 0;

Button _buttons[MAX_BUTTONS];
Encoder _encoders[MAX_ENCODERS];
int _numButtons = 0;
int _numEncoders = 0;

//fixme: might as well map these to _BV directly
void inputEnable(byte portNr, byte pin)
{
    return;

    if (portNr == 0)
    {
        DDRA &= ~_BV(pin);
        PORTA |= _BV(pin);
    }
    else if (portNr == 1)
    {
        DDRB &= ~_BV(pin);
        PORTB |= _BV(pin);
    }
    else if (portNr == 2)
    {
        DDRC &= ~_BV(pin);
        PORTC |= _BV(pin);
    }
    if (portNr == 3)
    {
        DDRD &= ~_BV(pin);
        PORTD |= _BV(pin);
    }
}

bool inputRead(byte portNr, byte pin)
{
    return xIn(pin);
    
    if (portNr == 0)
    {
        return PINA & _BV(pin);
    }
    else if (portNr == 1)
    {
        return PINB & _BV(pin);
    }
    else if (portNr == 2)
    {
        return PINC & _BV(pin);
    }
    if (portNr == 3)
    {
        return PIND & _BV(pin);
    }
    return 0;
}

byte inputMapToPin(byte portNr, byte p)
{
    return portNr * 8 + p;

    if (portNr == 0)
    {
        if (p == 0)return PA0;
        if (p == 1)return PA1;
        if (p == 2)return PA2;
        if (p == 3)return PA3;
        if (p == 4)return PA4;
        if (p == 5)return PA5;
        if (p == 6)return PA6;
        if (p == 7)return PA7;
    }
    else if (portNr == 1)
    {
        if (p == 0)return PB0;
        if (p == 1)return PB1;
        if (p == 2)return PB2;
        if (p == 3)return PB3;
        if (p == 4)return PB4;
        if (p == 5)return PB5;
        if (p == 6)return PB6;
        if (p == 7)return PB7;
    }
    else if (portNr == 2)
    {
        if (p == 0)return PC0;
        if (p == 1)return PC1;
        if (p == 2)return PC2;
        if (p == 3)return PC3;
        if (p == 4)return PC4;
        if (p == 5)return PC5;
        if (p == 6)return PC6;
        if (p == 7)return PC7;
    }
    else if (portNr == 3)
    {
        if (p == 0)return PD0;
        if (p == 1)return PD1;
        if (p == 2)return PD2;
        if (p == 3)return PD3;
        if (p == 4)return PD4;
        if (p == 5)return PD5;
        if (p == 6)return PD6;
        if (p == 7)return PD7;
    }
    return 0;
}

void inputAddButton(byte portNr, byte p)
{
    _buttons[_numButtons].portNr = portNr;
    _buttons[_numButtons].p = inputMapToPin(portNr, p);
    _buttons[_numButtons].down = false;
    
    inputEnable(portNr, p);
    
    _numButtons++;
}
void inputAddEncoder(byte portNr, byte pA, byte pB)
{
    _encoders[_numEncoders].portNr = portNr;
    _encoders[_numEncoders].pA = inputMapToPin(portNr, pA);
    _encoders[_numEncoders].pB = inputMapToPin(portNr, pB);
    
    inputEnable(portNr, pA);
    inputEnable(portNr, pB);

    _numEncoders++;
}

void inputSetCallbacks(void (*keyHandleDown)(int), void (*keyHandleUp)(int), void (*encHandleMove)(int, int))
{
    _keyHandleDown = keyHandleDown;
    _keyHandleUp = keyHandleUp;
    _encHandleMove = encHandleMove;
}

void inputPoll()
{
    //poll keys
    _keyTicks++;
    if(_keyTicks > 100)
    {
        int i=0;
        for (;i<_numButtons;i++)
        {
            bool push;
            push = inputRead(_buttons[i].portNr, _buttons[i].p);
            //push = !push;

            if (push)
            {
                if( ! _buttons[i].down )
                {
                    _buttons[i].down = true;
                    if (_keyHandleDown != 0)(*_keyHandleDown)(i);
                }
            }
            else if(_buttons[i].down) //wait for release
            {
                if (_keyHandleUp != 0)_keyHandleUp(i);
                _buttons[i].down = false;
            }
        }
        
        _keyTicks = 0;
    }
    

    //poll encoders
    
    _encTicks++;
    if (_encTicks > 10) 
    {
        _encTicks = 0;
        
        int i=0;
        for (;i<_numEncoders;i++)
        {
            bool encA = inputRead(_encoders[i].portNr, _encoders[i].pA);
            bool encB = inputRead(_encoders[i].portNr, _encoders[i].pB);

            bool changed = false;
            bool increase = false;
            int speed;

            if(_encoders[i].lastA && !encA && _encoders[i].lastB && !encB)
            {
                changed = true;
                increase = true;
            }
        
            if(_encoders[i].lastA && !encA && !_encoders[i].lastB && !encB)
            {
                changed = true;
                increase = false;
            }
        
            if (changed)
            {
                speed = _encoders[i].speed>>2;
                if (speed <= 0 )speed = 1;

                if (increase)
                {
                    if (_encHandleMove != 0) _encHandleMove(i, speed);
                }
                else
                {
                    if (_encHandleMove != 0) _encHandleMove(i, -speed);
                }

                _encoders[i].delay = 50;
                _encoders[i].speed+=5;
            }

            _encoders[i].lastA = encA;
            _encoders[i].lastB = encB;
        
            if(_encoders[i].delay)
                _encoders[i].delay--;
            else _encoders[i].speed = 1;
        }
    }
}
