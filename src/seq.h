#ifndef SEQ_H
#define SEQ_H

#include "defines.h"
#include "alt0cmd.h"

#define SEQ_FILE_ID 0
#define SEQ_FILE_VERSION 0

#include "channel.h"

/*CONVENTION:
  all seqModifyAAA() operate on current channel/instrument/step/sheet selection
*/

struct Instrument
{
    uint a; 
    uint d;
    uint s; 
    uint r;

    smp_t amp; 
    OscWave wave;
    WsType wsType;
    uint glide;

    bool ad_hz_use;
    uint ad_hz_a; 
    uint ad_hz_d;
    uint ad_hz_min;
    uint ad_hz_max;

    smp_t lfoAmp[NUM_CHAN_LFO];
    uint lfoHz[NUM_CHAN_LFO];
    OscWave lfoWave[NUM_CHAN_LFO];
    byte lfoMin[NUM_CHAN_LFO];
}; //32byte + 2byte
typedef struct Instrument Instrument;

#define TRIGGER_ON   0
#define TRIGGER_KEEP 1
#define TRIGGER_OFF  2
#define TRIGGER_ONLY_OFF 3
#define TRIGGER_UNUSED 4
#define TRIGGER_LAST 4

#define TRIGGER_GET(note)      (note.trig_inst & 0x0f)
#define INST_GET(note)         ((note.trig_inst>>4) & 0x0f)
#define TRIGGER_SET(note, val) note.trig_inst = (note.trig_inst&0xf0) | val
#define INST_SET(note, val)    note.trig_inst = (note.trig_inst&0x0f) | (val<<4)

//this Struct renders alot of ram/eeprom usage i.e keep it small:
//5byte = 1200byte
//3byte = 720byte
//2byte = 480byte
struct Note
{
    sbyte note;
    //save as much as possible here:
    byte trig_inst;
}; 
typedef struct Note Note;



#define NUM_INSTRUMENTS  6
#define INSTRUMENT_LAST 5
#define MAX_SHEETS 10
#define LAST_SHEET 9
#define SHEET_STEPS 8
#define LAST_STEP 7
#define LAST_CHANNEL (NUM_CHANNELS - 1)

#define NUM_SONG_SHEETS 48

struct Seq
{
    //DATA ORDER IS IMPORTANT
    //file will read first bytes
    byte id;
    byte version;
    uint bpm;
    sbyte songSheets[NUM_CHANNELS][NUM_SONG_SHEETS];
    Note notes[NUM_CHANNELS][MAX_SHEETS][SHEET_STEPS];
    Instrument inst[NUM_INSTRUMENTS];
    //2 + 2 + 2*3*10*8 + 34*6 + 3*48 = 832 of 990, lpcut etc in after this

    //NOT SAVED
    byte _firstVolatile;

    sbyte selectedInst[NUM_CHANNELS];
    int stepDelay;
    bool changed;
    bool changedPos;
    bool running;
    sbyte chan;
    sbyte selected;
    bool solo;
    sbyte songStep;
    bool songOn;
    bool songLoop;
    bool songPlay;
    bool songChanged;
    bool songChangedPos;
    bool songDraw;
    bool laterChange;
    uint clockRate;
    sbyte sheet[NUM_CHANNELS];
    byte step;

    Note sheetCopy[SHEET_STEPS];
    
    //channels to operate on
    Channel* channels;

    //unused:
    //sbyte lastInst[NUM_CHANNELS];

}; 
typedef struct Seq Seq;

#define SEQ_SONG_START -1
#define SEQ_SONG_END -2
#define SEQ_SONG_LAST_MARKER -2

void seqClearSong(Seq* seq);
void seqSongOn(Seq* seq, bool on); //songOn enables song sheet load, play enables song stepping
void seqSongPlay(Seq* seq, bool on);
void seqSongLoop(Seq* seq, bool on);
void seqSongRewind(Seq* seq); //revinds to start marker, or 0 if not present
void seqSongModifySelected(Seq* seq, sbyte steps);
void seqSongSetSelected(Seq* seq, sbyte step);
sbyte seqSongGetSelected(Seq* seq);

void seqCopySelectedSheet(Seq* seq);
void seqPasteSelectedSheet(Seq* seq);
void seqClearSelectedSheet(Seq* seq);

void seqInit(Seq* seq, int clockRate, Channel* channels);
void seqSetBpm(Seq* seq, uint bpm);
void seqProcess(Seq* seq);
void seqDraw(Seq* seq);
void seqDrawSong(Seq* seq);
void seqDrawSheet(Seq* seq);

void seqLoadNoteToChannel(Seq* seq, byte ch);

//calc notes as described in pgmmem.h e.g As + 12*octave
void seqNoteOn(Seq* seq, byte sheet, byte step, byte note, byte trigger);
bool seqGetNoteOn(Seq* seq, byte sheet, byte step);

void seqNoteOff(Seq* seq, byte sheet, byte step);

void seqSetRunning(Seq* seq, bool on);

bool seqGetSelectedOn(Seq* seq);
byte seqGetSelected(Seq* seq);
void seqSetSelected(Seq* seq, sbyte index);
void seqModifySelected(Seq* seq, sbyte steps, sbyte trigger);

void seqModifyChan(Seq* seq, sbyte steps);
void seqModifySheet(Seq* seq, sbyte steps);

void seqClearNotes(Seq* seq);
void seqSetSolo(Seq* seq, bool on);
void seqModifySolo(Seq* seq, sbyte steps);

//instrument
void seqModifyInstrument(Seq* seq, sbyte steps);
void seqSetSelectedInstrument(Seq* seq, sbyte chan, byte inst);
void seqLoadInstrumentToChannel(Seq* seq, sbyte chan, byte inst);
void seqSaveChannelToInstrument(Seq* seq, sbyte chan, byte inst);
void seqInitInstruments(Seq* seq);
void seqClear(Seq* seq);

//file
bool seqSaveToFile(Seq* seg, int fd);
bool seqLoadFromFile(Seq* seg, int fd);

/*********************/
/*INSTRUMENT MODIFY'S*/
/*********************/

#define CURRENT -1

void seqInstrumentSetA(Seq* seq, sbyte inst, uint val);
uint seqInstrumentGetA(Seq* seq, sbyte inst);
void seqInstrumentModifyA(Seq* seq, int steps);
//
void seqInstrumentSetD(Seq* seq, sbyte inst, uint val);
uint seqInstrumentGetD(Seq* seq, sbyte inst);
void seqInstrumentModifyD(Seq* seq, int steps);
//
void seqInstrumentSetS(Seq* seq, sbyte inst, uint val);
uint seqInstrumentGetS(Seq* seq, sbyte inst);
void seqInstrumentModifyS(Seq* seq, int steps);
//
void seqInstrumentSetR(Seq* seq, sbyte inst, uint val);
uint seqInstrumentGetR(Seq* seq, sbyte inst);
void seqInstrumentModifyR(Seq* seq, int steps);
//
void seqInstrumentSetAmp(Seq* seq, sbyte inst, smp_t val);
smp_t seqInstrumentGetAmp(Seq* seq, sbyte inst);
void seqInstrumentModifyAmp(Seq* seq, int steps);
//
void seqInstrumentSetWave(Seq* seq, sbyte inst, OscWave val);
OscWave seqInstrumentGetWave(Seq* seq, sbyte inst);
void seqInstrumentModifyWave(Seq* seq, int steps);
//
void seqInstrumentSetWsType(Seq* seq, sbyte inst, WsType val);
WsType seqInstrumentGetWsType(Seq* seq, sbyte inst);
void seqInstrumentModifyWsType(Seq* seq, int steps);
//
void seqInstrumentSetGlide(Seq* seq, sbyte inst, uint val);
uint seqInstrumentGetGlide(Seq* seq, sbyte inst);
void seqInstrumentModifyGlide(Seq* seq, int steps);
//
void seqInstrumentSetAdHzUse(Seq* seq, sbyte inst, bool val);
bool seqInstrumentGetAdHzUse(Seq* seq, sbyte inst);
void seqInstrumentModifyAdHzUse(Seq* seq, int steps);
//
void seqInstrumentSetAdHzA(Seq* seq, sbyte inst, uint val);
uint seqInstrumentGetAdHzA(Seq* seq, sbyte inst);
void seqInstrumentModifyAdHzA(Seq* seq, int steps);
//
void seqInstrumentSetAdHzD(Seq* seq, sbyte inst, uint val);
uint seqInstrumentGetAdHzD(Seq* seq, sbyte inst);
void seqInstrumentModifyAdHzD(Seq* seq, int steps);
//
void seqInstrumentSetAdHzMin(Seq* seq, sbyte inst, uint val);
uint seqInstrumentGetAdHzMin(Seq* seq, sbyte inst);
void seqInstrumentModifyAdHzMin(Seq* seq, int steps);
//
void seqInstrumentSetAdHzMax(Seq* seq, sbyte inst, uint val);
uint seqInstrumentGetAdHzMax(Seq* seq, sbyte inst);
void seqInstrumentModifyAdHzMax(Seq* seq, int steps);

//
void seqInstrumentSetLfoAmp(Seq* seq, sbyte inst, smp_t val, byte lfoNr);
smp_t seqInstrumentGetLfoAmp(Seq* seq, sbyte inst, byte lfoNr);
void seqInstrumentModifyLfoAmp(Seq* seq, int steps, byte lfoNr);
//
void seqInstrumentSetLfoHz(Seq* seq, sbyte inst, uint val, byte lfoNr);
uint seqInstrumentGetLfoHz(Seq* seq, sbyte inst, byte lfoNr);
void seqInstrumentModifyLfoHz(Seq* seq, int steps, byte lfoNr);
//
void seqInstrumentSetLfoWave(Seq* seq, sbyte inst, OscWave val, byte lfoNr);
OscWave seqInstrumentGetLfoWave(Seq* seq, sbyte inst, byte lfoNr);
void seqInstrumentModifyLfoWave(Seq* seq, int steps, byte lfoNr);

//
void seqInstrumentSetLfoMin(Seq* seq, sbyte inst, byte val, byte lfoNr);
byte seqInstrumentGetLfoMin(Seq* seq, sbyte inst, byte lfoNr);
void seqInstrumentModifyLfoMin(Seq* seq, int steps, byte lfoNr);
#endif
