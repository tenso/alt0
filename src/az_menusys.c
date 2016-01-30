#include "az_menusys.h"

#include "menu.h"
#include "seq.h"
#include "error.h"
#include "util.h"
#include "dogm.h"
#include "disk.h"

extern Seq seq;

#define FILE_OK 0
#define FILE_ERROR_LOAD 1
#define FILE_ERROR_SAVE 2
#define FILE_ERROR_EMPTY 3
#define FILE_NO_MESSAGE 4

bool _fileConfirm;
bool _fileConfirmOk;
char _fileName[FILE_NAME_LEN + 1];
int _fileFd;
byte _fileError;
bool _menuMainAltKeyDown;
bool _menuMainAltMenu;
sbyte _menuMainCurrentAlt;
Menu mainMenu;
Menu menus[NUM_MENUS];

void menuSystemInit()
{
    _fileConfirm = false;
    _fileConfirmOk = false;

    byte i=0;
    for(;i<FILE_NAME_LEN;i++)
    {
        _fileName[i] = '-';
    }

    _fileName[FILE_NAME_LEN] = '\0';
    _fileFd = -1;
    _fileError = FILE_NO_MESSAGE;
    _menuMainAltKeyDown = false;
    _menuMainAltMenu = false;
    _menuMainCurrentAlt = 0;

    menuInit(&mainMenu, &menuMainDraw, &menuMainEncMove, &menuMainKeyDown, &menuMainKeyUp, 0);
    menuInit(&menus[MENU_GEN], &menuGenDraw, &menuGenEncMove, &menuGenKeyDown, 0, 0);
    menuInit(&menus[MENU_ADSR], &menuAdsrDraw, &menuAdsrEncMove, &menuAdsrKeyDown, 0, 0);
    menuInit(&menus[MENU_HZENV], &menuHzEnvDraw, &menuHzEnvEncMove, &menuHzEnvKeyDown, 0, 0);
    menuInit(&menus[MENU_SHEET], &menuSheetDraw, &menuSheetEncMove, &menuSheetKeyDown, 0, &menuSheetRefresh);
    menuInit(&menus[MENU_WS], &menuWsDraw, &menuWsEncMove, &menuWsKeyDown, 0, 0);
    menuInit(&menus[MENU_LFO], &menuLfoDraw, &menuLfoEncMove, &menuLfoKeyDown, 0, 0);
    menuInit(&menus[MENU_ERRORS], &menuErrorsDraw, &menuErrorsEncMove, &menuErrorsKeyDown, 0, 0);
    menuInit(&menus[MENU_SONG], &menuSongDraw, &menuSongEncMove, &menuSongKeyDown, 0, &menuSongRefresh);
    menuInit(&menus[MENU_SETUP], &menuSetupDraw, &menuSetupEncMove, &menuSetupKeyDown, 0, 0);
    menuInit(&menus[MENU_FILE], &menuFileDraw, &menuFileEncMove, &menuFileKeyDown, 0, 0);
}

void menuSystemDraw()
{
    if (menus[mainMenu.self.currentSub].self.needRefresh)
    {
        menus[mainMenu.self.currentSub].draw( &(menus[mainMenu.self.currentSub].self) );
            
        mainMenu.self.needRefresh = true;
    }
        
    if (mainMenu.self.show)
    {
        if (mainMenu.self.needRefresh)
        {
            mainMenu.draw( &(mainMenu.self));
        }
    }
}

void menuSystemPageRefresh(byte menu, byte what)
{
    menus[menu].refresh( &(menus[menu].self), what );
}

void menuSystemKeyDown(int id)
{
    if (!mainMenu.self.show)
    {
        if (id == 1)
        {
            _menuMainAltKeyDown = true;
            mainMenu.self.show = true;
            mainMenu.self.needRefresh = true;
            return;
        }
        else
        {
            if (menus[mainMenu.self.currentSub].keyDown != 0)
            {
                menus[mainMenu.self.currentSub].keyDown(&menus[mainMenu.self.currentSub].self, id);
            }
        }
    }
    else //if mainMenu.self.show
    {
        mainMenu.keyDown(&mainMenu.self, id);
    }
}

void menuSystemKeyUp(int id)
{
    if (!mainMenu.self.show)
    {
        if (menus[mainMenu.self.currentSub].keyUp != 0)
        {
            menus[mainMenu.self.currentSub].keyUp(&menus[mainMenu.self.currentSub].self, id);
        }
    }
    else //if mainMenu.self.show
    {
        mainMenu.keyUp(&mainMenu.self, id);
    }
}

void menuSystemEncMove(int id, int speed)
{
    if (!mainMenu.self.show) 
    {    
        menus[mainMenu.self.currentSub].encMove(&(menus[mainMenu.self.currentSub].self), id, speed);
    }
    else 
    {
        mainMenu.encMove(&(mainMenu.self), id, speed);
    }
}

/*MAIN MENU*/

#define MENU_MAIN_ALT_COPY 0
#define MENU_MAIN_ALT_PASTE 1
#define MENU_MAIN_ALT_CLEAR 2
#define MENU_MAIN_ALT_CANCEL 3
#define MENU_MAIN_ALT_LAST 3

#define QUICKMENU_CHANNEL 0
#define QUICKMENU_SHEET 1
#define QUICKMENU_INST 2
#define QUICKMENU_SONG 3
#define QUICKMENU_LAST 3

void menuMainDraw(MenuData* self)
{
    dmClear(0,2, 15, 2);
    
    if (_menuMainAltMenu && self->currentSub == MENU_SHEET)
    {
        if (_menuMainCurrentAlt == MENU_MAIN_ALT_COPY)dmPrint(0,2, ":copy");
        else if (_menuMainCurrentAlt == MENU_MAIN_ALT_PASTE)dmPrint(0,2, ":paste");
        else if (_menuMainCurrentAlt == MENU_MAIN_ALT_CLEAR)dmPrint(0,2, ":clear");
        else if (_menuMainCurrentAlt == MENU_MAIN_ALT_CANCEL)dmPrint(0,2, ":cancel");
    }
    else if( !_menuMainAltKeyDown)
    {
        if (self->currentSub == MENU_SHEET)dmPrint(0,2, ">sheet");
        if (self->currentSub == MENU_ADSR)dmPrint(0,2, ">adsr");
        if (self->currentSub == MENU_GEN)dmPrint(0,2, ">gen");
        if (self->currentSub == MENU_HZENV)dmPrint(0,2, ">hzenv");
        if (self->currentSub == MENU_WS)dmPrint(0,2, ">ws");
        if (self->currentSub == MENU_LFO)dmPrint(0,2, ">wslfo");
        if (self->currentSub == MENU_ERRORS)dmPrint(0,2, ">log");
        if (self->currentSub == MENU_SONG)dmPrint(0,2, ">song");
        if (self->currentSub == MENU_SETUP)dmPrint(0,2, ">setup");
        if (self->currentSub == MENU_FILE)dmPrint(0,2, ">file");
                
        if (self->currentQuick == QUICKMENU_CHANNEL)
        {
            dmPrint(7,2, " >ch:");
            dmPrint(14, 2, itoa(seq.chan));
        }
        if (self->currentQuick == QUICKMENU_SHEET)
        {
            dmPrint(7,2, " >pat:");
            dmPrint(14, 2, itoa(seq.sheet[seq.chan]));
        }
        /*if (self->currentQuick == QUICKMENU_SOLO)
          {
          dmPrint(7,2, " >sol:");
          dmPrint(14, 2, itoa(seq.solo));
          }*/
        if (self->currentQuick == QUICKMENU_INST)
        {
            dmPrint(7,2, " >inst:");
            dmPrint(14, 2, itoa(seq.selectedInst[seq.chan]));
        }
        if (self->currentQuick == QUICKMENU_SONG)
        {
            dmPrint(7,2, " >song:");
            dmPrint(14, 2, itoa(seq.songOn));
        }
    }
    else
    {
        dmPrint(0,2, "enc=alt,rel=main");
    }

    if (self->currentSub == MENU_SHEET)
    {
        seq.changedPos = true;
        seqDraw(&seq);
    }
    if (self->currentSub == MENU_SONG)
    {
        seq.songChangedPos = true;
        seqDraw(&seq);
    }
            
    self->needRefresh = false;
}


void menuMainEncMove(MenuData* self, int id, int speed)
{
    if (id == 1)
    {
        if (_menuMainAltKeyDown)
        {
            _menuMainAltMenu = true;
        }
        
        if (_menuMainAltMenu)
        {
            self->needRefresh = true;
            if (_menuMainAltKeyDown)
            {
                _menuMainAltKeyDown = false;
            }
            else
            {
                _menuMainCurrentAlt += speed;
            }
            
            if (_menuMainCurrentAlt < 0)_menuMainCurrentAlt = MENU_MAIN_ALT_LAST;
            if (_menuMainCurrentAlt > MENU_MAIN_ALT_LAST)_menuMainCurrentAlt = 0;
        }
        else
        {
            self->needRefresh = true;
            self->currentSub += speed;

            if (self->currentSub < 0)self->currentSub = MENU_LAST;
            if (self->currentSub > MENU_LAST)self->currentSub = 0;
        
            menus[mainMenu.self.currentSub].refresh( &(menus[mainMenu.self.currentSub].self), REFRESH_ALL);
        }
    }

    if (id == 2)
    {
        self->needRefresh = true;

        menus[mainMenu.self.currentSub].refresh( &(menus[mainMenu.self.currentSub].self), REFRESH_ALL);
            
        if (self->currentQuick == QUICKMENU_CHANNEL)
        {
            seqModifyChan(&seq, speed);
        }
        else if (self->currentQuick == QUICKMENU_SHEET)
        {
            seqModifySheet(&seq, speed);
        }
        /*else if (self->currentQuick == QUICKMENU_SOLO)
          {
          seqModifySolo(&seq, speed);
          }*/
        else if (self->currentQuick == QUICKMENU_INST)
        {
            seqModifyInstrument(&seq, speed);
        }
        else if (self->currentQuick == QUICKMENU_SONG)
        {
            seqSongOn(&seq, !seq.songOn);
        }
    }
}

void menuMainKeyDown(MenuData* self, int id)
{
    if (id == 1)
    {
        //this must be done in menusystem:
        //_menuMainAltKeyDown = true;

        //enter submenu
        menus[self->currentSub].refresh( &(menus[self->currentSub].self), REFRESH_ALL);
        self->show = false;
        dmClearDisplay(); //fresh display for new menu
    }
    if (id == 2)
    {
        self->currentQuick++;
        self->needRefresh = true;
        if (self->currentQuick > QUICKMENU_LAST)
        {
            self->currentQuick = 0;
        }
    }
}

void menuMainKeyUp(MenuData* self, int id)
{
    if (id == 1)
    {
        _menuMainAltKeyDown = false;

        if (_menuMainAltMenu)
        {
            if (_menuMainCurrentAlt == MENU_MAIN_ALT_COPY)
            {
                seqCopySelectedSheet(&seq);
            }
            else if (_menuMainCurrentAlt == MENU_MAIN_ALT_PASTE)
            {
                seqPasteSelectedSheet(&seq);
            }
            else if (_menuMainCurrentAlt == MENU_MAIN_ALT_CLEAR)
            {
                seqClearSelectedSheet(&seq);
            }
            else if (_menuMainCurrentAlt == MENU_MAIN_ALT_CANCEL)
            {
            }

            _menuMainAltMenu = false;
            
            //re-enter submenu
            menus[self->currentSub].refresh( &(menus[self->currentSub].self), REFRESH_ALL);
            self->show = false;
            dmClearDisplay();
        }
        else
        {
            self->needRefresh = true;
        }
    }
}


/*GEN MENU*/

#define MENU_GEN_WAVE 0
#define MENU_GEN_AMP 1
#define MENU_GEN_GLIDE 2
#define MENU_GEN_HZENV 3
#define MENU_GEN_LAST 3

void menuGenDraw(MenuData* self)
{
    dmClear(0,0,15,1);
                
    OscWave wave = seqInstrumentGetWave(&seq, CURRENT);
    if (wave == OSC_SQU) dmPrint(0,0,"wav:squ");
    else if (wave == OSC_SAW) dmPrint(0,0,"wav:saw");
    else if (wave == OSC_TRI) dmPrint(0,0,"wav:tri");
    else if (wave == OSC_NOISE) dmPrint(0,0,"wav:noi");
    else if (wave == OSC_DC) dmPrint(0,0,"wav:dc");

    dmPrint(10,0,"vl:"); dmPrint(13,0,itoa_u(seqInstrumentGetAmp(&seq, CURRENT)));
    dmPrint(0,1,"gli:"); dmPrint(4,1,itoa_u(0xffff - seqInstrumentGetGlide(&seq, CURRENT)));
                
    bool adHzOn = seqInstrumentGetAdHzUse(&seq, CURRENT);
    dmPrint(10,1,"hze:");
    dmPrint(14,1, itoa(adHzOn));
                
    if (self->currentSub == MENU_GEN_WAVE)dmSetCursor(4,0);
    if (self->currentSub == MENU_GEN_AMP)dmSetCursor(13,0);
    if (self->currentSub == MENU_GEN_GLIDE)dmSetCursor(4,1);
    if (self->currentSub == MENU_GEN_HZENV)dmSetCursor(14,1);

    self->needRefresh = false;
}
void menuGenEncMove(MenuData* self, int id, int speed)
{
    if (id == 1)
    {
        self->currentSub += speed;
        if (self->currentSub > MENU_GEN_LAST)self->currentSub = 0;
        if (self->currentSub < 0)self->currentSub = MENU_GEN_LAST;
        self->needRefresh = true;
    }
        
    if (id == 2)
    {
        bool neg = false;
        if (speed < 0)neg = true;
        int inc = (speed*speed);
        if (neg)inc = -inc;

        if (self->currentSub == MENU_GEN_WAVE)
        {
            seqInstrumentModifyWave(&seq, inc);
        }
        if (self->currentSub == MENU_GEN_AMP)
        {
            seqInstrumentModifyAmp(&seq, inc);
        }
        if (self->currentSub == MENU_GEN_GLIDE)
        {
            seqInstrumentModifyGlide(&seq, inc);
        }
        if (self->currentSub == MENU_GEN_HZENV)
        {
            seqInstrumentModifyAdHzUse(&seq, inc);
        }
        self->needRefresh = true;
    }
}

void menuGenKeyDown(MenuData* self, int id)
{
}

/*ADSR MENU*/

#define MENU_ADSR_A 0
#define MENU_ADSR_D 1
#define MENU_ADSR_S 2
#define MENU_ADSR_R 3
#define MENU_ADSR_LAST 3

void menuAdsrDraw(MenuData* self)
{
    dmClear(0,0,15,1);

    dmPrint(0,0,"a:"); dmPrint(2,0,itoa_u(seqInstrumentGetA(&seq, CURRENT)));
    dmPrint(8,0,"d:"); dmPrint(10,0,itoa_u(seqInstrumentGetD(&seq, CURRENT)));
    dmPrint(0,1,"s:"); dmPrint(2,1,itoa_u(seqInstrumentGetS(&seq, CURRENT)));
    dmPrint(8,1,"r:"); dmPrint(10,1,itoa_u( seqInstrumentGetR(&seq, CURRENT)));
                
    if (self->currentSub == MENU_ADSR_A)dmSetCursor(0,0);
    if (self->currentSub == MENU_ADSR_D)dmSetCursor(8,0);
    if (self->currentSub == MENU_ADSR_S)dmSetCursor(0,1);
    if (self->currentSub == MENU_ADSR_R)dmSetCursor(8,1);

    self->needRefresh = false;
}
void menuAdsrEncMove(MenuData* self, int id, int speed)
{
    if (id == 1)
    {
        self->currentSub+=speed;
        if (self->currentSub > MENU_ADSR_LAST)
        {
            self->currentSub = 0;
        }
        if (self->currentSub < 0)
        {
            self->currentSub = MENU_ADSR_LAST;
        }
        self->needRefresh = true;
    }
        
    if (id == 2)
    {
        bool neg = false;
        if (speed < 0)neg = true;
        int inc = (speed*speed);
        if (neg)inc = -inc;

        if (self->currentSub == MENU_ADSR_A)
        {
            seqInstrumentModifyA(&seq, inc);
        }
        if (self->currentSub == MENU_ADSR_D)
        {
            seqInstrumentModifyD(&seq, inc);
        }
        if (self->currentSub == MENU_ADSR_S)
        {
            seqInstrumentModifyS(&seq, inc);
        }
        if (self->currentSub == MENU_ADSR_R)
        {
            seqInstrumentModifyR(&seq, inc);
        }
        self->needRefresh = true;
    }
}

void menuAdsrKeyDown(MenuData* self, int id)
{
}

/*HZENV MENU*/
#define MENU_HZENV_A 0
#define MENU_HZENV_D 1
#define MENU_HZENV_MIN 2
#define MENU_HZENV_MAX 3
#define MENU_HZENV_LAST 3

void menuHzEnvDraw(MenuData* self)
{
    //dmClearDisplay();
    dmClear(0,0,15,1);

    dmPrint(0,0,"a:"); dmPrint(2, 0, itoa_u(seqInstrumentGetAdHzA(&seq, CURRENT)));
    dmPrint(8,0,"d:"); dmPrint(10, 0, itoa_u(seqInstrumentGetAdHzD(&seq, CURRENT)));
    dmPrint(0,1,"s:"); dmPrint(2, 1, itoa_u(seqInstrumentGetAdHzMin(&seq, CURRENT)/10));
    dmPrint(8,1,"e:"); dmPrint(10, 1, itoa_u(seqInstrumentGetAdHzMax(&seq, CURRENT)/10));
                
    if (self->currentSub == MENU_HZENV_A)dmSetCursor(0,0);
    if (self->currentSub == MENU_HZENV_D)dmSetCursor(8,0);
    if (self->currentSub == MENU_HZENV_MIN)dmSetCursor(0,1);
    if (self->currentSub == MENU_HZENV_MAX)dmSetCursor(8,1);

    self->needRefresh = false;
            
}

void menuHzEnvEncMove(MenuData* self, int id, int speed)
{
    if (id == 1)
    {
        self->currentSub+=speed;
        if (self->currentSub > MENU_HZENV_LAST)self->currentSub = 0;
        if (self->currentSub < 0)self->currentSub = MENU_HZENV_LAST;
        self->needRefresh = true;
    }
        
    if (id == 2)
    {
        bool neg = false;
        if (speed < 0)neg = true;
        int inc = (speed*speed);
        if (neg)inc = -inc;

        if (self->currentSub == MENU_HZENV_A)
        {
            seqInstrumentModifyAdHzA(&seq, inc);
        }
        if (self->currentSub == MENU_HZENV_D)
        {
            seqInstrumentModifyAdHzD(&seq, inc);
        }
        if (self->currentSub == MENU_HZENV_MIN)
        {
            seqInstrumentModifyAdHzMin(&seq, inc);
        }
        if (self->currentSub == MENU_HZENV_MAX)
        {
            seqInstrumentModifyAdHzMax(&seq, inc);
        }
        self->needRefresh = true;
    }
}

void menuHzEnvKeyDown(MenuData* self, int id)
{

}

void menuSheetDraw(MenuData* self)
{
    seq.songDraw = false;
    seqDraw(&seq);
    
    self->needRefresh = false;
}

void menuSheetEncMove(MenuData* self, int id, int speed)
{
    if (id == 1)
    {
        self->needRefresh = true;    
        seqSetSelected(&seq, seqGetSelected(&seq)+speed);
    }
    else if (id == 2)
    {
        self->needRefresh = true;    
        seqModifySelected(&seq, speed, 0);
    }
}
void menuSheetKeyDown(MenuData* self, int id)
{
    if (id == 2)
    {
        seqModifySelected(&seq, 0, 1);
        self->needRefresh = true;
    }
}

void menuSheetRefresh(MenuData* self, byte what)
{
    if(what == REFRESH_ALL)
    {
        seq.changedPos = true;
        seq.changed = true;
    }
    else if (what == REFRESH_SHEET_POS)
    {
        seq.changedPos = true;
    }
    self->needRefresh = true;
}

/*song*/

void menuSongDraw(MenuData* self)
{
    seq.songDraw = true;
    seqDraw(&seq);
    self->needRefresh = false;
}

void menuSongEncMove(MenuData* self, int id, int speed)
{
    if (id == 1)
    {
        self->needRefresh = true;    
        seqSongSetSelected(&seq, seqSongGetSelected(&seq)+speed);
        seqSongPlay(&seq, false);
    }
    else if (id == 2)
    {
        self->needRefresh = true;    
        seqSongModifySelected(&seq, speed);
        seqSongPlay(&seq, false);
    }
}
void menuSongKeyDown(MenuData* self, int id)
{
    if (id == 2)
    {
        seqSongPlay(&seq, !seq.songPlay);
    }
}

void menuSongRefresh(MenuData* self, byte what)
{
    if(what == REFRESH_ALL)
    {
        seq.songChangedPos = true;
        seq.songChanged = true;
    }
    else if (what == REFRESH_SHEET_POS)
    {
        seq.songChangedPos = true;
    }
    self->needRefresh = true;
}


#define MENU_WS_WAVE 0
#define MENU_WS_LAST 0

void menuWsDraw(MenuData* self)
{
    dmClear(0,0,15,1);

    WsType wave = seqInstrumentGetWsType(&seq, CURRENT);
    if (wave == WS_NONE) dmPrint(0,0,"ws:off");
    else if (wave == WS_AM) dmPrint(0,0,"ws:am");
    else if (wave == WS_HARDCLIP) dmPrint(0,0,"ws:hardclip");
    else if (wave == WS_HARDCLIP_OFF) dmPrint(0,0,"ws:hc_offset");
    else if (wave == WS_TOPINVERT) dmPrint(0,0,"ws:topinvert");
    else if (wave == WS_OVERDRIVE) dmPrint(0,0,"ws:overdrive");
    else if (wave == WS_DESTROY) dmPrint(0,0,"ws:destroy");
                
    if (self->currentSub == MENU_WS_WAVE)dmSetCursor(0,0);

    self->needRefresh = false;
}
void menuWsEncMove(MenuData* self, int id, int speed)
{
    if (id == 1)
    {
        self->currentSub += speed;
        if (self->currentSub > MENU_WS_LAST)self->currentSub = 0;
        if (self->currentSub < 0)self->currentSub = MENU_WS_LAST;
        self->needRefresh = true;
    }
        
    if (id == 2)
    {
        bool neg = false;
        if (speed < 0)neg = true;
        int inc = (speed*speed);
        if (neg)inc = -inc;

        if (self->currentSub == MENU_WS_WAVE)
        {
            seqInstrumentModifyWsType(&seq, inc);
        }
        self->needRefresh = true;
    }
}
void menuWsKeyDown(MenuData* self, int id)
{
}

#define MENU_LFO_WAVE 0
#define MENU_LFO_AMP 1
#define MENU_LFO_HZ 2
#define MENU_LFO_MIN 3
#define MENU_LFO_LAST 3

void menuLfoDraw(MenuData* self)
{
    dmClear(0,0,15,1);
                
    OscWave wave = seqInstrumentGetLfoWave(&seq, CURRENT, 0);
    if (wave == OSC_SQU) dmPrint(0,0,"wav:squ");
    else if (wave == OSC_SAW) dmPrint(0,0,"wav:saw");
    else if (wave == OSC_TRI) dmPrint(0,0,"wav:tri");
    else if (wave == OSC_NOISE) dmPrint(0,0,"wav:noi");
    else if (wave == OSC_DC) dmPrint(0,0,"wav:dc");

    dmPrint(10,0,"vl:"); dmPrint(13,0,itoa_u(seqInstrumentGetLfoAmp(&seq, CURRENT, 0)));
    dmPrint(0,1,"hz:"); dmPrint(4,1,itoa_u(seqInstrumentGetLfoHz(&seq, CURRENT, 0)));
    dmPrint(10,1,"z:"); dmPrint(12,1,itoa_u(seqInstrumentGetLfoMin(&seq, CURRENT, 0)));
                
                
    if (self->currentSub == MENU_LFO_WAVE)dmSetCursor(0,0);
    if (self->currentSub == MENU_LFO_AMP)dmSetCursor(10,0);
    if (self->currentSub == MENU_LFO_HZ)dmSetCursor(0,1);
    if (self->currentSub == MENU_LFO_MIN)dmSetCursor(10,1);

    self->needRefresh = false;
}
void menuLfoEncMove(MenuData* self, int id, int speed)
{
    if (id == 1)
    {
        self->currentSub += speed;
        if (self->currentSub > MENU_LFO_LAST)self->currentSub = 0;
        if (self->currentSub < 0)self->currentSub = MENU_LFO_LAST;
        self->needRefresh = true;
    }
    else if (id == 2)
    {
        bool neg = false;
        if (speed < 0)neg = true;
        int inc = (speed*speed);
        if (neg)inc = -inc;

        if (self->currentSub == MENU_LFO_WAVE)
        {
            seqInstrumentModifyLfoWave(&seq, inc, 0);
        }
        if (self->currentSub == MENU_LFO_AMP)
        {
            seqInstrumentModifyLfoAmp(&seq, inc, 0);
        }
        if (self->currentSub == MENU_LFO_HZ)
        {
            seqInstrumentModifyLfoHz(&seq, inc, 0);
        }
        if (self->currentSub == MENU_LFO_MIN)
        {
            seqInstrumentModifyLfoMin(&seq, inc, 0);
        }
        self->needRefresh = true;
    }
}

void menuLfoKeyDown(MenuData* self, int id)
{
}

#define MENU_ERROR_ABOUT 0
#define MENU_ERROR_ERROR 1
#define MENU_ERROR_LAST 1

void menuErrorsDraw(MenuData* self)
{
    dmClear(0,0,15,2);
    
    if (self->currentSub == MENU_ERROR_ERROR)
    {
        byte i = 0;
        for (;i<=ERROR_LAST;i++)
        {
            dmPrint( (i*4) % 16, (i*4)/16, itoa(errorGetCount(i)));
            dmPrint( (i*4+3)%16, (i*4)/16, ",");
        }
        dmPrint(0, 2, "errors");
    }
    if (self->currentSub == MENU_ERROR_ABOUT)
    {
        dmPrint(0, 0, "az soft v."); dmSendString(itoa(VERSION));
        dmPrint(0, 1, "(c) 2010");
        dmPrint(0, 2, "Anton Olofsson");
    }
    self->needRefresh = false;

}

void menuErrorsEncMove(MenuData* self, int id, int speed)
{
    if (id == 1)
    {
        self->currentSub += speed;
        if (self->currentSub > MENU_ERROR_LAST)self->currentSub = 0;
        if (self->currentSub < 0)self->currentSub = MENU_ERROR_LAST;
        self->needRefresh = true;
    }
}

void menuErrorsKeyDown(MenuData* self, int id)
{
}


#define MENU_SETUP_SOLO 0
#define MENU_SETUP_BPM 1
#define MENU_SETUP_LAST 1

void menuSetupDraw(MenuData* self)
{
    dmClear(0,0,15,1);

    dmPrint(0,0,"sol:"); dmPrint(4,0,itoa_u(seq.solo));
    dmPrint(6,0,"bpm:"); dmPrint(10,0,itoa_u(seq.bpm));
                
    if (self->currentSub == MENU_SETUP_SOLO)dmSetCursor(4,0);
    if (self->currentSub == MENU_SETUP_BPM)dmSetCursor(10,0);

    self->needRefresh = false;
}
void menuSetupEncMove(MenuData* self, int id, int speed)
{
    if (id == 1)
    {
        self->currentSub+=speed;
        if (self->currentSub > MENU_SETUP_LAST)
        {
            self->currentSub = 0;
        }
        if (self->currentSub < 0)
        {
            self->currentSub = MENU_SETUP_LAST;
        }
        self->needRefresh = true;
    }
        
    if (id == 2)
    {
        bool neg = false;
        if (speed < 0)neg = true;
        int inc = (speed*speed);
        if (neg)inc = -inc;

        if (self->currentSub == MENU_SETUP_SOLO)
        {
            seqModifySolo(&seq, inc);
        }
        if (self->currentSub == MENU_SETUP_BPM)
        {
            int bpm = seq.bpm + inc;
            if (bpm > 5000)bpm = 5000;
            if (bpm < 0)bpm = 0;
            seqSetBpm(&seq, bpm);
        }

        self->needRefresh = true;
    }
}

void menuSetupKeyDown(MenuData* self, int id)
{
}

/*FILE MENU*/

#define MENU_FILE_FILE 0
#define MENU_FILE_SAVE 1
#define MENU_FILE_LOAD 2
#define MENU_FILE_ERASE 3
#define MENU_FILE_LAST 3

void menuFileDraw(MenuData* self)
{
    dmClear(0,0,15,2);

    dmPrint(0,0,"file:"); dmPrint(6,0, _fileName);
    dmPrint(0,1,"save"); dmPrint(5,1,"load"); dmPrint(10,1,"erase"); 
    
    if (_fileError == FILE_OK)
    {
        dmPrint(0,2, "file: ok");
    }
    else if (_fileError == FILE_ERROR_EMPTY)
    {
        dmPrint(0,2, "error: empty");
    }
    else if (_fileError == FILE_ERROR_LOAD)
    {
        dmPrint(0,2, "error: load");
    }
    else if (_fileError == FILE_ERROR_SAVE)
    {
        dmPrint(0,2, "error: save");
    }

    if (_fileConfirm)
    {
        if (!_fileConfirmOk)dmPrint(0,2,"confirm: no"); 
        else dmPrint(0,2,"confirm: yes"); 
    }

    if (self->currentSub == MENU_FILE_FILE)dmSetCursor(0,0);
    if (self->currentSub == MENU_FILE_SAVE)dmSetCursor(0,1);
    if (self->currentSub == MENU_FILE_LOAD)dmSetCursor(5,1);
    if (self->currentSub == MENU_FILE_ERASE)dmSetCursor(10,1);
    
    self->needRefresh = false;
}
void menuFileEncMove(MenuData* self, int id, int speed)
{
    _fileError = FILE_NO_MESSAGE;

    if (id == 1)
    {
        if (_fileConfirm)
        {
        }
        else
        {
            self->currentSub+=speed;
            if (self->currentSub > MENU_FILE_LAST)
            {
                self->currentSub = 0;
            }
            if (self->currentSub < 0)
            {
                self->currentSub = MENU_FILE_LAST;
            }
            self->needRefresh = true;
        }
    }
        
    if (id == 2)
    {
        if (_fileConfirm)
        {
            _fileConfirmOk = !_fileConfirmOk;
        }
        else
        {
        }
        self->needRefresh = true;
    }
}

void menuFileKeyDown(MenuData* self, int id)
{
    _fileError = FILE_NO_MESSAGE;

    if (id == 2)
    {
        if (_fileConfirm)
        {
            if (_fileConfirmOk)
            {
                //do it
                if (self->currentSub == MENU_FILE_LOAD)
                {
                    if (_fileFd == 0 || _fileFd == -1)
                    {
                        _fileError = FILE_ERROR_LOAD;
                    }
                    else if (fileReadErased(_fileFd))
                    {
                        _fileError = FILE_ERROR_EMPTY;
                    }
                    else if (!seqLoadFromFile(&seq, _fileFd))
                    {
                        _fileError = FILE_ERROR_LOAD;
                    }
                    else
                    {
                        _fileError = FILE_OK;
                        fileReadName(_fileFd, _fileName);
                    }
                }
                if (self->currentSub == MENU_FILE_SAVE)
                {
                    if (_fileFd == 0 || _fileFd == -1)
                    {
                        _fileError = FILE_ERROR_SAVE;
                    }
                    else if (!seqSaveToFile(&seq, _fileFd))
                    {
                        _fileError = FILE_ERROR_SAVE;
                    }
                    else
                    {
                        _fileError = FILE_OK;
                    }
                    fileReadName(_fileFd, _fileName);
                }
                if (self->currentSub == MENU_FILE_ERASE)
                {
                    fileErase(_fileFd);
                    _fileError = FILE_OK;
                    _fileName[0] = '*';
                    _fileName[1] = '*';
                    _fileName[2] = 'f';
                    _fileName[3] = 'r';
                    _fileName[4] = 'e';
                    _fileName[5] = 'e';
                    _fileName[6] = '*';
                    _fileName[7] = '*';
                    _fileName[8] = '\0';
                }
            }
            else
            {
                //forget it
            }

            _fileConfirm = false;
            _fileConfirmOk = false;
        }
        else
        {
            if (self->currentSub == MENU_FILE_FILE)
            {
                if (!fileSeekNext(&_fileFd))
                {
                    fileSeekBegin();
                    fileSeekNext(&_fileFd);
                }
                
                if (_fileFd == 0)
                {
                    fileSeekNext(&_fileFd);
                }
                
                if (fileReadErased(_fileFd))
                {
                    _fileName[0] = '*';
                    _fileName[1] = '*';
                    _fileName[2] = 'f';
                    _fileName[3] = 'r';
                    _fileName[4] = 'e';
                    _fileName[5] = 'e';
                    _fileName[6] = '*';
                    _fileName[7] = '*';
                    _fileName[8] = '\0';
                }
                else
                {
                    fileReadName(_fileFd, _fileName);
                }
            }
            else
            {
                _fileConfirm = true;
                _fileConfirmOk = false;
                _fileError = FILE_NO_MESSAGE;
            }
        }
        self->needRefresh = true;
    }
}
