#include "seq.h"
#include "dogm.h"
#include "generator.h"
#include "envelope.h"
#include "progmem.h"
#include "util.h"
#include "disk.h"
#include "channel.h"

void seqInit(Seq* seq, int clockRate, Channel* channels)
{
    seq->channels = channels;
    seq->clockRate = clockRate;
    seqSetBpm(seq, 60);
    seqClear(seq);
}

void seqClear(Seq* seq)
{
    seq->step = 0;
    seq->changed = true;
    seq->changedPos = true;
    seq->running = true;
    seq->selected = 0;
    seq->chan = 0;
    seq->solo = false;
    seq->songStep = 0;
    seq->songOn = false;
    seq->songLoop = true;
    seq->songChanged = false;
    seq->songChangedPos = false;
    seq->songPlay = false;
    seq->songDraw = false;
    seq->laterChange = false;

    seqClearNotes(seq);
    seqClearSong(seq);
    seqInitInstruments(seq);
}

void seqClearSong(Seq* seq)
{
    int i = 0;
    for (;i<NUM_CHANNELS;i++)
    {
        int j = 0;
        for (;j<NUM_SONG_SHEETS;j++)
        {
            if (j== 0)
            {
                seq->songSheets[i][j] = SEQ_SONG_START;
            }
            else if (j==NUM_SONG_SHEETS-1)
            {
                seq->songSheets[i][j] = SEQ_SONG_END;
            }
            else
            {
                seq->songSheets[i][j] = 0;
            }
        }
    }
    seq->songStep = 0;
    
}

void seqSongSetSelected(Seq* seq, sbyte step)
{
    seq->songStep = step;
    if (seq->songStep < 0)seq->songStep = NUM_SONG_SHEETS - 1;
    if (seq->songStep >= NUM_SONG_SHEETS)seq->songStep = 0;
}

sbyte seqSongGetSelected(Seq* seq)
{
    return seq->songStep;
}

void seqSongOn(Seq* seq, bool on)
{
    seq->songOn = on;
    if (seq->songOn)
    {
        seq->songChanged = true;
        seq->songChangedPos = true;
    }
    else
    {
        //make sure patterns are ok
        int i=0;
        for (;i<NUM_CHANNELS;i++)
        {
            if (seq->sheet[i] < 0)seq->sheet[i] = 0;
        }
    }
}

void seqSongLoop(Seq* seq, bool on)
{
    seq->songLoop = on;
}

void seqSongRewind(Seq* seq)
{
    int ch=0;
    for (;ch<NUM_CHANNELS;ch++)
    {
        int i=0;
        for (;i<NUM_SONG_SHEETS;i++)
        {
            if (seq->songSheets[ch][i] == SEQ_SONG_START)
            {
                seq->songStep = i;
                return;
            }
        }
    }
    seq->songStep = 0;
}

void seqSongModifySelected(Seq* seq, sbyte steps)
{
    seq->songSheets[seq->chan][seq->songStep] += steps;
    
    if (seq->songSheets[seq->chan][seq->songStep] < SEQ_SONG_LAST_MARKER)
    {
        seq->songSheets[seq->chan][seq->songStep] = LAST_SHEET;
    }
    else if (seq->songSheets[seq->chan][seq->songStep] >= LAST_SHEET)
    {
        seq->songSheets[seq->chan][seq->songStep] = SEQ_SONG_LAST_MARKER;
    }
    seq->songChanged = true;
    seq->songChangedPos = true;
}

void seqClearNotes(Seq* seq)
{
    uint i=0;
    for (;i<NUM_CHANNELS;i++)
    {
        seq->sheet[i] = 0;
        uint j=0;
        for (;j<MAX_SHEETS;j++)
        {
            uint k=0;
            for (;k<SHEET_STEPS;k++)
            {
                //seq->notes[i][j][k].trigger = TRIGGER_UNUSED;
                TRIGGER_SET(seq->notes[i][j][k], TRIGGER_UNUSED);
            }
        }
    }
}

void seqSetBpm(Seq* seq, uint bpm)
{
    seq->bpm = bpm;
    seq->stepDelay = seq->clockRate*(uint)60/bpm;
}

void seqProcess(Seq* seq)
{
    if (!seq->running)return;
    
    if (seq->laterChange)
    {
        seq->changed = true;
        seq->laterChange = false;
    }

    //do things...
    int ch = 0;
    for (;ch< NUM_CHANNELS;ch++)
    {
        if (seq->songOn)
        {
            if (seq->songSheets[ch][seq->songStep] == SEQ_SONG_START)
            {
                seq->sheet[ch] = seq->songSheets[ch][seq->songStep];
                envSetOn(&(seq->channels[ch].env), false);
                continue;
            }
            else if (seq->songSheets[ch][seq->songStep] == SEQ_SONG_END)
            {
                if (seq->songPlay)
                {
                    seqSongRewind(seq);

                    if (seq->songLoop)
                    {
                        //find first pattern not start/stop and start playing it
                        int first = 0;
                        bool found = false;
                        for (;first < NUM_SONG_SHEETS;first++)
                        {
                            if (seq->songSheets[ch][first] != SEQ_SONG_START && seq->songSheets[ch][first] != SEQ_SONG_END)
                            {
                                found = true;
                                break;
                            }
                        }
                        if (found)
                        {
                            seq->songStep = found;
                        }
                        else
                        {
                            int i=0;
                            for (;i<NUM_CHANNELS;i++)envSetOn(&(seq->channels[i].env), false);
                            seqSongPlay(seq, false);
                            break;
                        }
                    }
                    else
                    {
                        seqSongPlay(seq, false);
                        int i=0;
                        for (;i<NUM_CHANNELS;i++)envSetOn(&(seq->channels[i].env), false);
                        break;
                    }

                }
            }
            seq->sheet[ch] = seq->songSheets[ch][seq->songStep];
        }

        //apply instrument
        //if (seq->notes[ch][seq->sheet[ch]][seq->step].trigger != TRIGGER_UNUSED)
        if (TRIGGER_GET(seq->notes[ch][seq->sheet[ch]][seq->step]) != TRIGGER_UNUSED)
        {
            //FIXME: optimization to be done here:
            //if (seq->notes[ch][seq->sheet[ch]][seq->step].inst != seq->lastInst[ch])
            {
                //seqLoadInstrumentToChannel(seq, ch, seq->notes[ch][seq->sheet[ch]][seq->step].inst);
                seqLoadInstrumentToChannel(seq, ch, INST_GET(seq->notes[ch][seq->sheet[ch]][seq->step]));
                //seq->lastInst[ch] = seq->notes[ch][seq->sheet[ch]][seq->step].inst;
            }
        }

        seqLoadNoteToChannel(seq, ch);
    }
    
    //for next step
    seq->step++;
    if (seq->step >= SHEET_STEPS)
    {
        seq->step = 0;

        if (seq->songOn)
        {
            if (seq->songPlay)
            {
                seq->songChangedPos = true;
                seq->laterChange = true;
                
                seq->songStep++;
                
                if (seq->songStep >= NUM_SONG_SHEETS)
                {
                    seqSongRewind(seq);

                    if (seq->songLoop)
                    {
                    }
                    else
                    {
                        seqSongPlay(seq, false);
                    }
                }
            }
        }
    }
    
    seq->changedPos = true;
}

void seqSongPlay(Seq* seq, bool on)
{
    seq->songPlay = on;
    if (seq->songPlay)seq->step = 0;
}

void seqLoadNoteToChannel(Seq* seq, byte ch)
{
    //hz + gates on/off
    //if (seq->notes[ch][seq->sheet[ch]][seq->step].trigger == TRIGGER_ONLY_OFF)
    byte trigger = TRIGGER_GET(seq->notes[ch][seq->sheet[ch]][seq->step]);

    if (trigger == TRIGGER_ONLY_OFF)
    {
        envSetOn(&(seq->channels[ch].env), false);
    }
    else if (trigger == TRIGGER_ON)
    {
        //genSetHz(&channels[ch].gen, seq->notes[ch][seq->sheet[ch]][seq->step].hz);
        genSetHz(&(seq->channels[ch].gen), keyToHz( seq->notes[ch][seq->sheet[ch]][seq->step].note )*10 );

        envSetOn(&(seq->channels[ch].env), true);

        if (seq->channels[ch].ad_hz_env.state != ENV_BYPASS)
        {
            //hardcut:
            seq->channels[ch].ad_hz_env.val = 0;
                
            envSetOn(&(seq->channels[ch].ad_hz_env), true);
            genSync(&(seq->channels[ch].gen));
        }

    }
    else if (trigger == TRIGGER_OFF)
    {
        genSetHz(&(seq->channels[ch].gen), keyToHz( seq->notes[ch][seq->sheet[ch]][seq->step].note )*10 );

        envSetOn(&(seq->channels[ch].env), false);   
    }
    else if (trigger == TRIGGER_KEEP)
    {
        if (!envGetOn(&(seq->channels[ch].env)))
        {
            envSetOn(&(seq->channels[ch].env), true);   
        }
        genSetHz(&(seq->channels[ch].gen), keyToHz( seq->notes[ch][seq->sheet[ch]][seq->step].note )*10 );
    }
}

void seqDraw(Seq* seq)
{
    if (seq->songDraw)
    {
        seqDrawSong(seq);
    }
    else
    {
        seqDrawSheet(seq);
    }
}

void seqDrawSong(Seq* seq)
{
    if (seq->songChanged)
    {
        dmClear(0,2,15,2);
        
        int i = 0;
        for(;i<NUM_SONG_SHEETS;i++)
        {
            char c;
            if (seq->songSheets[seq->chan][i] == SEQ_SONG_START)
            {
                c = '>';
            }
            else if (seq->songSheets[seq->chan][i] == SEQ_SONG_END)
            {
                c = '<';
            }
            else if (seq->songSheets[seq->chan][i] <= 9)
            {
                c = '0' + seq->songSheets[seq->chan][i];
            }
            else
            {
                c = 'a' + (seq->songSheets[seq->chan][i] - 10);
            }
            dmPutc(i%16, i/16, c);
        }
        
        seq->songChanged = false;
    }
    if (seq->songChangedPos)
    {
        dmSetCursor(seq->songStep%16, seq->songStep/16);
        
        seq->songChangedPos = false;
    }
}

void seqDrawSheet(Seq* seq)
{
    if (seq->changed)
    {
        dmClear(0,2,15,2);
        
        if (seq->sheet[seq->chan] == SEQ_SONG_END)
        {
            dmClear(0,0,15,1);
            dmPrint(0,0,"< end pattern");
            seq->changed = false;
            return;
        }
        else if (seq->sheet[seq->chan] == SEQ_SONG_START)
        {
            dmClear(0,0,15,1);
            dmPrint(0,0,"> start pattern");
            seq->changed = false;
            return;
        }

        byte i=0;
        for (;i<SHEET_STEPS;i++)
        {
            byte trigger = TRIGGER_GET(seq->notes[seq->chan][seq->sheet[seq->chan]][i]);

            if (trigger == TRIGGER_UNUSED)
            {
                dmPrint(2*i, 0, "-_");
                dmPrint(2*i, 1, "__");
                continue;
            }
            else if (trigger == TRIGGER_ONLY_OFF)
            {
                dmPrint(2*i, 0, "-_");
                dmPrint(2*i, 1, "_x");
                continue;
            }
        
            char name = 'A';
            char mode = ' ';
            byte octave = 0;

            byte note = seq->notes[seq->chan][seq->sheet[seq->chan]][i].note;
        
            octave = note/12;
            note -= octave*12;
        
            if (note == 1)
            {
                name = 'A';
                mode = '#';
            }
            else if (note == 2)name = 'B';
            else if (note == 3)name = 'C';
            else if (note == 4)
            {
                name = 'C';
                mode = '#';
            }
            else if (note == 5)name = 'D';
            else if (note == 6)
            {
                name = 'D';
                mode = '#';
            }
            else if (note == 7)name = 'E';
            else if (note == 8)name = 'F';
            else if (note == 9)
            {
                name = 'F';
                mode = '#';
            }
            else if (note == 10)name = 'G';
            else if (note == 11)
            {
                name = 'G';
                mode = '#';
            }

            dmPutc(2*i, 0, name);
            dmPutc(2*i+1, 0, mode);
            dmPrint(2*i, 1, itoa(octave));
        
            if (trigger == TRIGGER_OFF)
            {
                dmPutc(2*i+1, 1, 'x');
            }
            else if (trigger == TRIGGER_ON)
            {
                dmPutc(2*i+1, 1, 'o');
            }
            else if (trigger == TRIGGER_KEEP)
            {
                dmPutc(2*i+1, 1, '-');
            }

            //dmPrint(2*i+1, 2, itoa(seq->notes[seq->chan][seq->sheet[seq->chan]][i].inst));
            dmPrint(2*i+1, 2, itoa( INST_GET(seq->notes[seq->chan][seq->sheet[seq->chan]][i])));
        }
        
        /*dmPutc(13,2, 'c');
          dmPrint(15,2, itoa(seq->chan));*/
        /*
          if (seq->solo)dmPutc(13, 2, 's');
        
          dmPutc(9, 2, 'p');
          dmPrint(11, 2, itoa(seq->sheet[seq->chan]));*/

        dmPutc(2*seq->selected, 2, '|');

        seq->changed = false;
    }
    if (seq->changedPos)
    {
        dmSetCursor(2*seq->step, 0);
        seq->changedPos = false;
    }
}

//calc notes as described in pgmmem.h e.g As + 12*octave
void seqNoteOn(Seq* seq, byte sheet, byte step, byte note, byte trigger)
{
    seq->notes[seq->chan][sheet][step].note = note;
    //seq->notes[seq->chan][sheet][step].trigger = trigger;
    TRIGGER_SET(seq->notes[seq->chan][sheet][step], trigger);

    //seq->notes[seq->chan][sheet][step].inst = seq->selectedInst[seq->chan];
    INST_SET(seq->notes[seq->chan][sheet][step], seq->selectedInst[seq->chan]);
    seq->changed = true;
}

void seqNoteOff(Seq* seq, byte sheet, byte step)
{
    //seq->notes[seq->chan][sheet][step].trigger = TRIGGER_UNUSED;
    TRIGGER_SET(seq->notes[seq->chan][sheet][step], TRIGGER_UNUSED);
    seq->changed = true;
}

void seqSetRunning(Seq* seq, bool on)
{
    seq->running = on;

    //make sure to turn off all envelopes here:
    
}

bool seqGetNoteOn(Seq* seq, byte sheet, byte step)
{
    //return seq->notes[seq->chan][sheet][step].trigger != TRIGGER_UNUSED;

    return TRIGGER_GET(seq->notes[seq->chan][sheet][step]) != TRIGGER_UNUSED;
}

void seqSetSelected(Seq* seq, sbyte index)
{
    seq->selected = index;
    if (seq->selected > 7)seq->selected = 0;
    if (seq->selected < 0)seq->selected = 7;
    
    seq->changed = true;
}

byte seqGetSelected(Seq* seq)
{
    return seq->selected;
}

bool seqGetSelectedOn(Seq* seq)
{
    //return seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].trigger != TRIGGER_UNUSED;
    return TRIGGER_GET(seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected]) != TRIGGER_UNUSED;
}


void seqModifySelected(Seq* seq, sbyte steps, sbyte triggerSteps)
{
    seq->changed = true;
    
    seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].note += steps;
    if (seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].note > LAST_NOTE)
    {
        seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].note = LAST_NOTE;
    }


    if (seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].note < 0)
    {
        seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].note = 0;
    }
    //seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].hz = keyToHz( seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].note ) * 10;
    
    //seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].trigger += triggerSteps;

    byte trigger = TRIGGER_GET(seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected]) + triggerSteps;
    if ( trigger > TRIGGER_LAST)
    {
        //seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].trigger = 0;
        trigger = 0; //first again
    }
    
    TRIGGER_SET(seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected], trigger);

    //seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected].inst = seq->selectedInst[seq->chan];
    INST_SET(seq->notes[seq->chan][seq->sheet[seq->chan]][seq->selected], seq->selectedInst[seq->chan]);

}

void seqModifyChan(Seq* seq, sbyte steps)
{
    //seqSaveChannelToInstrument(seq, seq->chan, seq->selectedInst[seq->chan]);

    seq->chan += steps;

    if (seq->chan < 0)seq->chan = LAST_CHANNEL;
    if (seq->chan > LAST_CHANNEL)seq->chan = 0;


    //seqLoadInstrumentToChannel(seq, seq->chan, seq->selectedInst[seq->chan]);
    
    //carry solo to next chan
    //if (seq->solo)seqSetSolo(seq, seq->solo);
    
    seq->changed = true;

}

void seqModifySheet(Seq* seq, sbyte steps)
{
    seq->sheet[seq->chan] += steps;

    if (seq->sheet[seq->chan] < 0)seq->sheet[seq->chan] = LAST_SHEET;
    if (seq->sheet[seq->chan] > LAST_SHEET)seq->sheet[seq->chan] = 0;

    seq->changed = true;
}

void seqModifySolo(Seq* seq, sbyte steps)
{
    seq->solo = !seq->solo;

    seqSetSolo(seq, seq->solo);
}

void seqSetSolo(Seq* seq, bool on)
{
    seq->solo = on;

    if(seq->solo)
    {
        byte i=0;
        for (;i<NUM_CHANNELS;i++)
        {
            if (i == seq->chan)
            {
                genSetMute(&(seq->channels[i].gen), false);
            }
            else
            {
                genSetMute(&(seq->channels[i].gen), true);
            }
        }
    }
    else
    {
        byte i=0;
        for (;i<NUM_CHANNELS;i++)
        {
            genSetMute(&(seq->channels[i].gen), false);
        }
    }
}

void seqInitInstruments(Seq* seq)
{
#if NUM_INSTRUMENTS != 6 
#error "setup for 6 instruments"
#endif

    seqSetSelectedInstrument(seq, 0, 0);
    seqSetSelectedInstrument(seq, 1, 2);
    seqSetSelectedInstrument(seq, 2, 4);

    //apply first time...
    /*seq->lastInst[0] = -1;
    seq->lastInst[1] = -1;
    seq->lastInst[2] = -1;
    */
    
    int i=0;
    for(;i<NUM_INSTRUMENTS;i++)
    {
        seq->inst[i].a = 250;
        seq->inst[i].d = 1000;
        seq->inst[i].s = 30000;
        seq->inst[i].r = 5000;
        
        seq->inst[i].amp = GEN_DEF_AMP;
        seq->inst[i].wave = OSC_SAW;
        seq->inst[i].wsType = WS_NONE;
        seq->inst[i].glide = 0xffff;
        seq->inst[i].ad_hz_use = false;
        seq->inst[i].ad_hz_a = 25; 
        seq->inst[i].ad_hz_d = 500;
        seq->inst[i].ad_hz_min = 200;
        seq->inst[i].ad_hz_max = 2100;

        int j=0;
        for (;j<NUM_CHAN_LFO;j++)
        {
            seq->inst[i].lfoAmp[j] = 100;
            seq->inst[i].lfoHz[j] = 2500;
            seq->inst[i].lfoWave[j] = OSC_TRI;
            seq->inst[i].lfoMin[j] = 50;
        }
    }
    
    seqLoadInstrumentToChannel(seq, 0, 0);
    seqLoadInstrumentToChannel(seq, 1, 2);
    seqLoadInstrumentToChannel(seq, 2, 4);
}

void seqModifyInstrument(Seq* seq, sbyte steps)
{
    sbyte inst = seq->selectedInst[seq->chan] + steps;
    if (inst < 0)inst = INSTRUMENT_LAST;
    if (inst > INSTRUMENT_LAST)inst = 0;
    
    seqSetSelectedInstrument(seq, seq->chan, inst);
}

void seqSetSelectedInstrument(Seq* seq, sbyte chan, byte inst)
{
    seq->selectedInst[chan] = inst;
}

void seqSaveChannelToInstrument(Seq* seq, sbyte chan, byte inst)
{
    seq->inst[inst].a = seq->channels[chan].env.a;
    seq->inst[inst].d = seq->channels[chan].env.d;
    seq->inst[inst].s = seq->channels[chan].env.s;
    seq->inst[inst].r = seq->channels[chan].env.r;

    seq->inst[inst].amp = seq->channels[chan].gen.amp;
    seq->inst[inst].wave = seq->channels[chan].gen.wave;
    seq->inst[inst].wsType = seq->channels[chan].gen.wsType;
    seq->inst[inst].glide = seq->channels[chan].gen.glide;

    seq->inst[inst].ad_hz_use = (seq->channels[chan].ad_hz_env.state != ENV_BYPASS) ? true : false;
    seq->inst[inst].ad_hz_a = seq->channels[chan].ad_hz_env.a;
    seq->inst[inst].ad_hz_d = seq->channels[chan].ad_hz_env.d;
    seq->inst[inst].ad_hz_min = seq->channels[chan].ad_hz_min;
    seq->inst[inst].ad_hz_max = seq->channels[chan].ad_hz_max;

    seq->inst[inst].lfoAmp[0] = seq->channels[chan].lfo[0].amp;
    seq->inst[inst].lfoAmp[1] = seq->channels[chan].lfo[1].amp;
    seq->inst[inst].lfoHz[0] = seq->channels[chan].lfo[0].hz;
    seq->inst[inst].lfoHz[1] = seq->channels[chan].lfo[1].hz;
    seq->inst[inst].lfoWave[0] = seq->channels[chan].lfo[0].wave;
    seq->inst[inst].lfoWave[1] = seq->channels[chan].lfo[1].wave;
    seq->inst[inst].lfoMin[0] = seq->channels[chan].lfoMin[0];
    seq->inst[inst].lfoMin[1] = seq->channels[chan].lfoMin[1];
}

void seqLoadInstrumentToChannel(Seq* seq, sbyte chan, byte inst)
{
    envSetADSR(&(seq->channels[chan].env), seq->inst[inst].a, seq->inst[inst].d, seq->inst[inst].s, seq->inst[inst].r);

    genSetAmp(&(seq->channels[chan].gen), seq->inst[inst].amp);
    genSetWave(&(seq->channels[chan].gen), seq->inst[inst].wave);
    genSetWsType(&(seq->channels[chan].gen), seq->inst[inst].wsType);
    genSetGlide(&(seq->channels[chan].gen), seq->inst[inst].glide);
    
    if (seq->inst[inst].ad_hz_use)
    {
        seq->channels[chan].ad_hz_env.state = ENV_IDLE;
    }
    else seq->channels[chan].ad_hz_env.state = ENV_BYPASS;

    envSetADSR(&(seq->channels[chan].ad_hz_env), seq->inst[inst].ad_hz_a, seq->inst[inst].ad_hz_d, 0, 0);
    seq->channels[chan].ad_hz_min = seq->inst[inst].ad_hz_min;
    seq->channels[chan].ad_hz_max = seq->inst[inst].ad_hz_max;

    //load lfo's
    genSetAmp(&(seq->channels[chan].lfo[0]), seq->inst[inst].lfoAmp[0]);
    genSetAmp(&(seq->channels[chan].lfo[1]), seq->inst[inst].lfoAmp[1]);

    genSetHz(&(seq->channels[chan].lfo[0]), seq->inst[inst].lfoHz[0]);
    genSetHz(&(seq->channels[chan].lfo[1]), seq->inst[inst].lfoHz[1]);

    genSetWave(&(seq->channels[chan].lfo[0]), seq->inst[inst].lfoWave[0]);
    genSetWave(&(seq->channels[chan].lfo[1]), seq->inst[inst].lfoWave[1]);

    seq->channels[chan].lfoMin[0] = seq->inst[inst].lfoMin[0];
    seq->channels[chan].lfoMin[1] = seq->inst[inst].lfoMin[1];
}

/*********************/
/*INSTRUMENT MODIFY'S*/
/*********************/

void seqInstrumentSetA(Seq* seq, sbyte inst, uint val)
{
    seq->inst[inst].a = val;
}

uint seqInstrumentGetA(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].a;
    else return seq->inst[inst].a;
}

void seqInstrumentModifyA(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].a += steps;
}

//

void seqInstrumentSetD(Seq* seq, sbyte inst, uint val)
{
    seq->inst[inst].d = val;
}

uint seqInstrumentGetD(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].d;
    else return seq->inst[inst].d;
}

void seqInstrumentModifyD(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].d += steps;
}

//

void seqInstrumentSetS(Seq* seq, sbyte inst, uint val)
{
    seq->inst[inst].s = val;
}

uint seqInstrumentGetS(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].s;
    else return seq->inst[inst].s;
}

void seqInstrumentModifyS(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].s += steps;
}

//

void seqInstrumentSetR(Seq* seq, sbyte inst, uint val)
{
    seq->inst[inst].r = val;
}

uint seqInstrumentGetR(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].r;
    else return seq->inst[inst].r;
}

void seqInstrumentModifyR(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].r += steps;
}

//

void seqInstrumentSetAmp(Seq* seq, sbyte inst, smp_t val)
{
    seq->inst[inst].amp = val;
}

smp_t seqInstrumentGetAmp(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].amp;
    else return seq->inst[inst].amp;
}

void seqInstrumentModifyAmp(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].amp += steps;
}

//

void seqInstrumentSetWave(Seq* seq, sbyte inst, OscWave val)
{
    seq->inst[inst].wave = val;
}

OscWave seqInstrumentGetWave(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].wave;
    else return seq->inst[inst].wave;
}

void seqInstrumentModifyWave(Seq* seq, int steps)
{
    sbyte w = seq->inst[seq->selectedInst[seq->chan]].wave + steps;
    if (w >= OSC_GROUP_SIZE)w = 0;
    if (w < 0)w = OSC_GROUP_SIZE - 1;
    seq->inst[seq->selectedInst[seq->chan]].wave = w;
}

//

void seqInstrumentSetWsType(Seq* seq, sbyte inst, WsType val)
{
    seq->inst[inst].wsType = val;
}

WsType seqInstrumentGetWsType(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].wsType;
    else return seq->inst[inst].wsType;
}

void seqInstrumentModifyWsType(Seq* seq, int steps)
{
    sbyte w = seq->inst[seq->selectedInst[seq->chan]].wsType + steps;
    if (w >= WS_GROUP_SIZE) w = 0;
    if (w < 0)w = WS_GROUP_SIZE - 1;

    seq->inst[seq->selectedInst[seq->chan]].wsType = w;
}

//

void seqInstrumentSetGlide(Seq* seq, sbyte inst, uint val)
{
    seq->inst[inst].glide = val;
}

uint seqInstrumentGetGlide(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].glide;
    else return seq->inst[inst].glide;
}

void seqInstrumentModifyGlide(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].glide += steps;
}

//

void seqInstrumentSetAdHzUse(Seq* seq, sbyte inst, bool val)
{
    seq->inst[inst].ad_hz_use = val;
}

bool seqInstrumentGetAdHzUse(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].ad_hz_use;
    else return seq->inst[inst].ad_hz_use;
}

void seqInstrumentModifyAdHzUse(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].ad_hz_use = !seq->inst[seq->selectedInst[seq->chan]].ad_hz_use;
}

//

void seqInstrumentSetAdHzA(Seq* seq, sbyte inst, uint val)
{
    seq->inst[inst].ad_hz_a = val;
}

uint seqInstrumentGetAdHzA(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].ad_hz_a;
    else return seq->inst[inst].ad_hz_a;
}

void seqInstrumentModifyAdHzA(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].ad_hz_a += steps;
}

//

void seqInstrumentSetAdHzD(Seq* seq, sbyte inst, uint val)
{
    seq->inst[inst].ad_hz_d = val;
}

uint seqInstrumentGetAdHzD(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].ad_hz_d;
    else return seq->inst[inst].ad_hz_d;
}

void seqInstrumentModifyAdHzD(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].ad_hz_d += steps;
}

//

void seqInstrumentSetAdHzMin(Seq* seq, sbyte inst, uint val)
{
    seq->inst[inst].ad_hz_min = val;
}

uint seqInstrumentGetAdHzMin(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].ad_hz_min;
    else return seq->inst[inst].ad_hz_min;
}

void seqInstrumentModifyAdHzMin(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].ad_hz_min += steps;
}

//

void seqInstrumentSetAdHzMax(Seq* seq, sbyte inst, uint val)
{
    seq->inst[inst].ad_hz_max = val;
}

uint seqInstrumentGetAdHzMax(Seq* seq, sbyte inst)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].ad_hz_max;
    else return seq->inst[inst].ad_hz_max;
}

void seqInstrumentModifyAdHzMax(Seq* seq, int steps)
{
    seq->inst[seq->selectedInst[seq->chan]].ad_hz_max += steps;
}

//

void seqInstrumentSetLfoAmp(Seq* seq, sbyte inst, smp_t val, byte lfoNr)
{
    seq->inst[inst].lfoAmp[lfoNr] = val;
}
smp_t seqInstrumentGetLfoAmp(Seq* seq, sbyte inst, byte lfoNr)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].lfoAmp[lfoNr];
    else return seq->inst[inst].lfoAmp[lfoNr];
}
void seqInstrumentModifyLfoAmp(Seq* seq, int steps, byte lfoNr)
{
    seq->inst[seq->selectedInst[seq->chan]].lfoAmp[lfoNr] += steps;
}

//

void seqInstrumentSetLfoHz(Seq* seq, sbyte inst, uint val, byte lfoNr)
{
    seq->inst[inst].lfoHz[lfoNr] = val;
}

uint seqInstrumentGetLfoHz(Seq* seq, sbyte inst, byte lfoNr)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].lfoHz[lfoNr];
    else return seq->inst[inst].lfoHz[lfoNr];
}

void seqInstrumentModifyLfoHz(Seq* seq, int steps, byte lfoNr)
{
    seq->inst[seq->selectedInst[seq->chan]].lfoHz[lfoNr] += steps;
}

//

void seqInstrumentSetLfoWave(Seq* seq, sbyte inst, OscWave val, byte lfoNr)
{
    seq->inst[inst].lfoWave[lfoNr] = val;
}

OscWave seqInstrumentGetLfoWave(Seq* seq, sbyte inst, byte lfoNr)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].lfoWave[lfoNr];
    else return seq->inst[inst].lfoWave[lfoNr];
}
void seqInstrumentModifyLfoWave(Seq* seq, int steps, byte lfoNr)
{
    sbyte w = seq->inst[seq->selectedInst[seq->chan]].lfoWave[lfoNr] + steps;
    if (w >= OSC_GROUP_SIZE)w = 0;
    if (w < 0)w = OSC_GROUP_SIZE - 1;

    seq->inst[seq->selectedInst[seq->chan]].lfoWave[lfoNr] = w;
}

void seqInstrumentSetLfoMin(Seq* seq, sbyte inst, byte val, byte lfoNr)
{
    seq->inst[inst].lfoMin[lfoNr] = val;
}

byte seqInstrumentGetLfoMin(Seq* seq, sbyte inst, byte lfoNr)
{
    if (inst < 0) return seq->inst[seq->selectedInst[seq->chan]].lfoMin[lfoNr];
    else return seq->inst[inst].lfoMin[lfoNr];
}
void seqInstrumentModifyLfoMin(Seq* seq, int steps, byte lfoNr)
{
    seq->inst[seq->selectedInst[seq->chan]].lfoMin[lfoNr] += steps;
}

//SAVE/LOAD

bool seqSaveToFile(Seq* seq, int fd)
{
    byte* data = (byte*)seq;
    
    seq->id = SEQ_FILE_ID;
    seq->version = SEQ_FILE_VERSION;

    uint dataLen = (int)(&(seq->_firstVolatile)) - (int)seq;
    return fileWriteData(fd, data, dataLen);
}

bool seqLoadFromFile(Seq* seq, int fd)
{
    seqClear(seq);

    uint dataLen = (int)(&(seq->_firstVolatile)) - (int)seq;
    int read = fileReadData(fd, (byte*)seq, dataLen);
    
    if (read == dataLen)
    {
        seqSetBpm(seq, seq->bpm);
        seqLoadInstrumentToChannel(seq, 0, 0);
        seqLoadInstrumentToChannel(seq, 1, 2);
        seqLoadInstrumentToChannel(seq, 2, 4);
        seqSongOn(seq, true);
    }
    else
    {
        seqClear(seq);
    }

    return (read == dataLen);
}

void seqCopySelectedSheet(Seq* seq)
{
    byte i = 0;
    for (;i<SHEET_STEPS;i++)
    {
        seq->sheetCopy[i] = seq->notes[seq->chan][seq->sheet[seq->chan]][i];
    }
}

void seqPasteSelectedSheet(Seq* seq)
{
    byte i = 0;
    for (;i<SHEET_STEPS;i++)
    {
        seq->notes[seq->chan][seq->sheet[seq->chan]][i] = seq->sheetCopy[i];
    }
}

void seqClearSelectedSheet(Seq* seq)
{
    byte i = 0;
    for (;i<SHEET_STEPS;i++)
    {
        TRIGGER_SET(seq->notes[seq->chan][seq->sheet[seq->chan]][i], TRIGGER_UNUSED);
    }
}
