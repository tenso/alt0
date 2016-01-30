#ifndef AZ_MENUSYS_H
#define AZ_MENUSYS_H

#include "menu.h"

//USER INTERFACE IS THIS:

void menuSystemInit();
void menuSystemDraw();
void menuSystemPageRefresh(byte menu, byte what);
void menuSystemKeyDown(int id);
void menuSystemKeyUp(int id);
void menuSystemEncMove(int id, int speed);

//used for refresh and coming jumptomenu function
#define MENU_SHEET 0
#define MENU_SETUP 1
#define MENU_ADSR 2
#define MENU_GEN 3
#define MENU_HZENV 4
#define MENU_WS 5
#define MENU_LFO 6
#define MENU_ERRORS 7
#define MENU_FILE 8
#define MENU_SONG 9
#define MENU_LAST 9
#define NUM_MENUS 10



//THE MENUS

void menuMainDraw(MenuData* self);
void menuMainEncMove(MenuData* self, int id, int speed);
void menuMainKeyDown(MenuData* self, int id);
void menuMainKeyUp(MenuData* self, int id);

void menuGenDraw(MenuData* self);
void menuGenEncMove(MenuData* self, int id, int speed);
void menuGenKeyDown(MenuData* self, int id);

void menuAdsrDraw(MenuData* self);
void menuAdsrEncMove(MenuData* self, int id, int speed);
void menuAdsrKeyDown(MenuData* self, int id);

void menuHzEnvDraw(MenuData* self);
void menuHzEnvEncMove(MenuData* self, int id, int speed);
void menuHzEnvKeyDown(MenuData* self, int id);

void menuSheetDraw(MenuData* self);
void menuSheetRefresh(MenuData* self, byte what);
void menuSheetEncMove(MenuData* self, int id, int speed);
void menuSheetKeyDown(MenuData* self, int id);

void menuSongDraw(MenuData* self);
void menuSongRefresh(MenuData* self, byte what);
void menuSongEncMove(MenuData* self, int id, int speed);
void menuSongKeyDown(MenuData* self, int id);

void menuWsDraw(MenuData* self);
void menuWsEncMove(MenuData* self, int id, int speed);
void menuWsKeyDown(MenuData* self, int id);

void menuLfoDraw(MenuData* self);
void menuLfoEncMove(MenuData* self, int id, int speed);
void menuLfoKeyDown(MenuData* self, int id);

void menuErrorsDraw(MenuData* self);
void menuErrorsEncMove(MenuData* self, int id, int speed);
void menuErrorsKeyDown(MenuData* self, int id);

void menuSetupDraw(MenuData* self);
void menuSetupEncMove(MenuData* self, int id, int speed);
void menuSetupKeyDown(MenuData* self, int id);

void menuFileDraw(MenuData* self);
void menuFileEncMove(MenuData* self, int id, int speed);
void menuFileKeyDown(MenuData* self, int id);

#endif
