#include "main.h"

//TODOS:
//SW: lfo's have massive destortion
//SW: save filter params (in seq file)
//SW: keep stepping other channels while pausing song on one channel for edit
//SW: added cent offset for oscs, add this internaly in genSetHz, genSetCentOffset(sbyte)
//SW: only AM waveshaper is cool
//add paramLfo to menus and possibility to route to params
//SW: make a Channel type that contains gen/env/lfo etc for that channel

//FIXMES:
//SW: disaply draw hinders sequencer somehow!? i.e if drawing takes to long sound is changed!
//SW: input.h -> make internal data in struct
//SW: make amp a last mult instead of osc ooutput. much better
//HW/SW: loud click when changeing lp/hp, mute sound before switch, route output to remaining dac?
//HW: alot of noise in lp circuit: compare filters vs off !!
//sw:gen-osc's are not right...guess its addition overflow that messes up


///////////////
//BSS GLOBALS//
///////////////

Channel channels[NUM_CHANNELS];
Envelope lpCutEnv;
DelayFb dfb;
RingBufferAudio ring;
RingBufferAudio lpCutRing;
extern smp_t _genBuff[];
smp_t mixBuff[RINGBUFF_AUDIO_SIZE];
RingBuffer32 usartRing;
byte response[CMD_BYTES];
uint usartBuffIndex = 0;
byte usartBuff[USART_BUFF_SIZE];
uint parseGetVal;
Param param;
int encVal = 0;
Seq seq;
int seqDelay = 0;


/////////////
//NTERRUPTS//
/////////////

//SOUND PLAY
//it takes about 31ms to swallow a buffer

ISR(TIMER1_OVF_vect)
{
    //HEAD
    cli();
    byte sreg = SREG;

    if (TCNT1 > 2500)errorIncCount(ERROR_SND_ISR_LAG);

    TCNT1 = TIMER1_FREQ_TO_CNT(SMP_FREQ);
    
    //BODY
    
    smp_t smp;
    if (ringAudioRead(&ring, &smp))
    {
        dacOut(DAC_CHIP_1, DAC_A, smp);

        if(lpCutEnv.state != ENV_BYPASS)
        {
            ringAudioRead(&lpCutRing, &smp);
            dacOut(0, DAC_LPCUT, smp);
        }
    }
    else
    {
        errorIncCount(ERROR_SND_ISR_UNDERRUN);
    }
    
    //FOOT
    SREG = sreg;
    sei();
}

//SEQUENCER STEP

ISR(TIMER0_OVF_vect)
{
    cli();
    byte sreg = SREG;
    
    //one fault first time
    if (TCNT0 != 0)errorIncCount(ERROR_SEQ_ISR_LAG);

    TCNT0 = TIMER0_FREQ_TO_CNT(SEQ_FREQ);

    seqDelay++;
    if (seqDelay >= seq.stepDelay)
    {
        seqDelay = 0;
        seqProcess(&seq); //note: isr should be quick, but this is rather tempo critical, more than sound?
        
        if (seq.songDraw)menuSystemPageRefresh(MENU_SONG, REFRESH_SHEET_POS);
        else 
        {
            menuSystemPageRefresh(MENU_SHEET, REFRESH_SHEET_POS);
        }
                
    }

    SREG = sreg;
    sei();
}

//USART INT

ISR(USART0_RX_vect)
{
    cli();
    byte sreg = SREG;
    
    if(!ring32Write(&usartRing, UDR0))
    {
        errorIncCount(ERROR_USART_OVERFLOW);
    }
    
    SREG = sreg;

    sei();
}


////////
//MAIN//
////////

int main()
{
    init();
    
    //start the noise
    sei(); 
    while(true)
    {
        synthesize();
        
        inputPoll();
        
        menuSystemDraw();

        handleUsart();
    }
}



//////////////
//SYNTHESIZE//
//////////////

void synthesize()
{
    uint toDo = ringAudioNumFree(&ring); //only thing that can happen is that numFree increases, no biggie

    if(toDo)
    {
        genBuffSet(mixBuff, toDo, 0);
        
        uint i=0;
        for(i=0;i<NUM_CHANNELS;i++)
        {
            channelSynthesizeAndMix(&channels[i], mixBuff, toDo);
        }

        delayFbBuffApply(&dfb, mixBuff, toDo);

#if SMP_MAX > 0xff
        genBuffRotateRight(mixBuff, 8, toDo);
#endif

        if(lpCutEnv.state != ENV_BYPASS)
        {
            envBuffGen(&lpCutEnv, _genBuff, toDo, MIN_LP_CUT, 155);
        
            //pass data to audio isr
            for(i=0; i < toDo ; i++)
            {   
                byte sreg = SREG;
                cli();
                ringAudioWrite(&ring, mixBuff[i]);
                ringAudioWrite(&lpCutRing, _genBuff[i]);
                SREG = sreg;
            }
        }
        else
        {
            //pass data to audio isr
            for(i=0; i < toDo ; i++)
            {   
                byte sreg = SREG;
                cli();
                ringAudioWrite(&ring, mixBuff[i]);
                SREG = sreg;
            }
        }
    }
}


/////////
//INIT//
////////

void init()
{
    errorInit();
    //enable extended io
    xioInit();
    
    dmInit();
    dmDisplayMode(true, true, false);
    dmClearDisplay();

    /*inputAddButton(3, 7);
    inputAddButton(3, 4);
    inputAddButton(1, 4);
    inputAddEncoder(2, 6, 7);
    inputAddEncoder(3, 5, 6);
    inputAddEncoder(3, 2, 3);
    */

    inputAddButton(0, 6);
    inputAddButton(0, 7);
    inputAddButton(1, 0);

    inputAddEncoder(0, 1, 0);
    inputAddEncoder(0, 3, 2);
    inputAddEncoder(0, 5, 4);
    

    inputSetCallbacks(&keyHandleDown, &keyHandleUp, &encHandleMove);
    
    usartInit();
    ring32Init(&usartRing);
    usartSetInt(true, false);
    
    //fixme: make initSampleTimer() or something
    TCNT1 = TIMER1_FREQ_TO_CNT(SMP_FREQ);
    //TIFR1 = (1<<TOV1); //enable interrupts for timer1
    TIMSK1 = (1<<TOIE1); //enable overflow interrupt
    
    //TCCR1B = 0b00000010; //ck/8
    //TCCR1B = 0b00000011; //ck/64
    TCCR1B = 0b00000100; //ck/256
    
    //init seq timer
    TCNT0 = TIMER0_FREQ_TO_CNT(SEQ_FREQ);
    TIMSK0 = (1<<TOIE0);
    TCCR0B = 0b00000101; //ck/1024

    ringAudioInit(&ring);
    
    uint i=0;
    
    for (i=0;i<NUM_CHANNELS;i++)
    {
        channelInit(&channels[i]);
    }
    
    regCommands();
    
    dacInit();
    
    envInit(&lpCutEnv);
    lpCutEnv.state = ENV_BYPASS; // ENV_IDLE;
    envSetA(&lpCutEnv, 50000);
    envSetD(&lpCutEnv, 10000);
    envSetS(&lpCutEnv, 50);
    envSetR(&lpCutEnv, 10000);
    
    delayFbInit(&dfb);
    
    //last:
    paramInit(&param);
    
    cmdMake(CMD_RET_BOOT_OK, 0,0,0,0, response);
    usartSendBytes(response, CMD_BYTES);
    
    seqInit(&seq, SEQ_FREQ, channels);
    seqSetBpm(&seq, 240);

    menuSystemInit();
    
    fileInit(EEPROM_SIZE);
    
    /*
    dmPrint(0,0, "formating...");
    fileFormatDisk();
    dmPrint(0,0,"formating...ok");
    dmPrint(0,1,"recompile");
    while(1);*/
}


////////////////
//HANDLE_USART//
////////////////

void handleUsart()
{
    //command parsing is not "that" time crit. use polling
    //although usart recieve is time crit
    byte data;
    bool newData;
    int maxToDo=3; 
    while (maxToDo)
    {
        byte sreg = SREG;
        cli();
        newData = ring32Read(&usartRing, &data);
        SREG = sreg;
        
        if (newData)
        {
            usartBuff[usartBuffIndex] = data;
        }
        else
        {
            return;
        }
        
        if( usartBuffIndex == CMD_BYTES-1)
        {
            //parse command?
            CmdCode code = cmdExec(usartBuff, &parseGetVal);
            
            if (code == CMD_GET_OK)
            {
                //response in cmdExec...
                cmdMake(CMD_RET_GET, usartBuff[1], usartBuff[2], usartBuff[3], parseGetVal, response);
                usartSendBytes(response, CMD_BYTES);
            }
            else if (code == CMD_SET_OK)
            {
                //usartSendStr(constStr(CS_OK));
            }
            else 
            {
                //command corrupt clear buffer
                byte sreg = SREG;
                cli();
                ring32Init(&usartRing);
                usartFlush();
                SREG = sreg;

                //send error response
                if (code == CMD_FAILED)
                {
                    cmdMake(CMD_RET_CMD_FAILED, usartBuff[1], usartBuff[2], usartBuff[3], usartBuff[4]<<8 | usartBuff[5], response);
                }
                else if (code == CMD_SET_FAILED)
                {
                    cmdMake(CMD_RET_CMD_SET_FAILED, usartBuff[1], usartBuff[2], usartBuff[3], usartBuff[4]<<8 | usartBuff[5], response);
                }
                else if (code == CMD_GET_FAILED)
                {
                    cmdMake(CMD_RET_CMD_GET_FAILED, usartBuff[1], usartBuff[2], usartBuff[3], usartBuff[4]<<8 | usartBuff[5], response);
                }
                else if (code == CMD_NOT_FOUND)
                {
                    cmdMake(CMD_RET_CMD_FAILED, usartBuff[1], usartBuff[2], usartBuff[3], usartBuff[4]<<8 | usartBuff[5], response);
                }

                usartSendBytes(response, CMD_BYTES);
                errorIncCount(ERROR_USART_PARSEFAIL);
            }
            
            //only do one command at a time
            usartBuffIndex = 0;
            maxToDo--;
            //return;
        }
        else
        {
            usartBuffIndex++;
        }
    }
}


void paramInit(Param* param)
{
    param->selected = LP_CUTOFF;
    param->lpCutoff = 127;
    param->lpRes = 0;

    //cli();
    dacOut(0, DAC_LPCUT, param->lpCutoff);
    dacOut(0, DAC_LPRES, param->lpRes);
    //sei();

    //init analog out selector
    AOUT_DDR |= _BV(AOUT_PIN_A) | _BV(AOUT_PIN_B);
    param->analogOut = ANALOG_OUT_LP24;
    //param->analogOut = ANALOG_OUT_RAW;
    paramUpdateAnalogOut(param);

    encVal = param->lpCutoff;
}

void paramUpdateDac(Param* param)
{
    //cli();
    if(param->selected == LP_CUTOFF)
    {
        dacOut(0, DAC_LPCUT, param->lpCutoff);
    }
    else if(param->selected == LP_RES)
    {
        dacOut(0, DAC_LPRES, param->lpRes);
    }
    //sei();
}

void paramUpdateAnalogOut(Param* param)
{
    if (param->analogOut == ANALOG_OUT_LP24)
    {
        AOUT_PORT &= ~_BV(AOUT_PIN_A);
        AOUT_PORT &= ~_BV(AOUT_PIN_B);
    }
    else if (param->analogOut == ANALOG_OUT_HP24)
    {
        AOUT_PORT |= _BV(AOUT_PIN_A);
        AOUT_PORT &= ~_BV(AOUT_PIN_B);
    }
    else if (param->analogOut == ANALOG_OUT_BP24)
    {
        AOUT_PORT &= ~_BV(AOUT_PIN_A);
        AOUT_PORT |= _BV(AOUT_PIN_B);
    }
    else if (param->analogOut == ANALOG_OUT_RAW)
    {
        AOUT_PORT |= _BV(AOUT_PIN_A);
        AOUT_PORT |= _BV(AOUT_PIN_B);
    }
}


void keyHandleDown(int id)
{
    if (id == 0)
    {
        param.selected++;
        if (param.selected  >= NUM_PARAM)param.selected = 0;
                
                
        if (param.selected == LP_CUTOFF)
        {
            encVal = param.lpCutoff;

            cmdMake(CMD_RET_INFO, CMD_HW_INPUT, CMD_HW_IN_LP_CUT, 0, encVal, response);
            usartSendBytes(response, CMD_BYTES);
        }
        else if (param.selected == LP_RES)
        {
            encVal = param.lpRes;

            cmdMake(CMD_RET_INFO, CMD_HW_INPUT, CMD_HW_IN_LP_RES, 0, encVal, response);
            usartSendBytes(response, CMD_BYTES);
        }
        else if (param.selected == DFB_DELAY)
        {
            encVal = dfb.delay;

            cmdMake(CMD_RET_INFO, CMD_HW_INPUT, CMD_HW_IN_DFB_DELAY, 0, encVal, response);
            usartSendBytes(response, CMD_BYTES);
        }
        else if (param.selected == DFB_GAIN)
        {
            encVal = dfb.gain;

            cmdMake(CMD_RET_INFO, CMD_HW_INPUT, CMD_HW_IN_DFB_GAIN, 0, encVal, response);
            usartSendBytes(response, CMD_BYTES);
        }
        else if (param.selected == ANALOG_OUT)
        {
            encVal = param.analogOut;
                    
            cmdMake(CMD_RET_INFO, CMD_HW_INPUT, CMD_HW_IN_ANALOG_OUT, 0, encVal, response);
            usartSendBytes(response, CMD_BYTES);
        }
        showParamSplash();
    }

    menuSystemKeyDown(id);

}

void encHandleMove(int id, int speed)
{
    if (id == 0)
    {
        encVal += speed;
        if (encVal > 255)encVal = 255;
        if (encVal < 0)encVal = 0;

        if(param.selected == LP_CUTOFF)
        {
            param.lpCutoff = encVal;
            if(param.lpCutoff < MIN_LP_CUT)
            {
                param.lpCutoff = MIN_LP_CUT;
                encVal = param.lpCutoff;
            }
        
            byte sreg = SREG;
            cli();
            paramUpdateDac(&param);
            SREG = sreg;

            cmdMake(CMD_RET_INFO, CMD_LP, CMD_LP_CUT, 0, encVal, response);
            usartSendBytes(response, CMD_BYTES);
        }
        else if(param.selected == LP_RES)
        {
            param.lpRes = encVal;
            if(param.lpRes > MAX_LP_RES)
            {
                param.lpRes = MAX_LP_RES;
                encVal = param.lpRes;
            }        

            byte sreg = SREG;
            cli();
            paramUpdateDac(&param);
            SREG = sreg;

            cmdMake(CMD_RET_INFO, CMD_LP, CMD_LP_RES, 0, encVal, response);
            usartSendBytes(response, CMD_BYTES);
        }
        else if(param.selected == DFB_DELAY)
        {
            delayFbSetDelay(&dfb, encVal);
                
            cmdMake(CMD_RET_INFO, CMD_DFB, CMD_DFB_DELAY, 0, encVal, response);
            usartSendBytes(response, CMD_BYTES);
        }
        else if(param.selected == DFB_GAIN)
        {
            delayFbSetGain(&dfb, encVal);
            cmdMake(CMD_RET_INFO, CMD_DFB, CMD_DFB_GAIN, 0, encVal, response);
            usartSendBytes(response, CMD_BYTES);
        }
        else if(param.selected == ANALOG_OUT)
        {
            if(encVal>=ANALOG_NUM_OUT)
            {
                encVal = 0;
            }
            param.analogOut = encVal;
            paramUpdateAnalogOut(&param);

            cmdMake(CMD_RET_INFO, CMD_ANALOG, CMD_ANALOG_OUT, 0, encVal, response);
            usartSendBytes(response, CMD_BYTES);
        }
        showParamSplash();
    }
    
    menuSystemEncMove(id, speed);
}

void showParamSplash()
{
    dmClear(0,0,15,2);
                    
    if (param.analogOut == ANALOG_OUT_LP24) dmPrint(0,0,"lp");
    if (param.analogOut == ANALOG_OUT_HP24) dmPrint(0,0,"hp");
    if (param.analogOut == ANALOG_OUT_BP24) dmPrint(0,0,"bp");
    if (param.analogOut == ANALOG_OUT_RAW) dmPrint(0,0,"off");

    dmPrint(4,0,"c:"); dmPrint(6,0,itoa_u(param.lpCutoff));
    dmPrint(10,0,"r:"); dmPrint(12,0,itoa_u(param.lpRes));
               
    dmPrint(0,1,"d-t:"); dmPrint(4,1,itoa_u(dfb.delay));
    dmPrint(9,1,"d-g:"); dmPrint(13,1,itoa_u(dfb.gain));

    if (param.selected == LP_CUTOFF) dmPrint(0,2,"mod: cutoff");
    if (param.selected == LP_RES) dmPrint(0,2,"mod: resonance");
    if (param.selected == DFB_DELAY) dmPrint(0,2,"mod: delay-time");
    if (param.selected == DFB_GAIN) dmPrint(0,2,"mod: dealy-gain");
    if (param.selected == ANALOG_OUT) dmPrint(0,2,"mod: filter mode");

}

void keyHandleUp(int id)
{
    menuSystemKeyUp(id);
}
