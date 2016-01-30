#ifndef MENU_H
#define MENU_H

#include "defines.h"

struct MenuData
{
    sbyte currentSub;
    bool needRefresh;

    //used for "main menu"
    bool show;
    sbyte currentQuick;
};
typedef struct MenuData MenuData;

struct Menu
{
    MenuData self;

    void (*draw)(MenuData* self);
    void (*encMove)(MenuData* self, int id, int speed);
    void (*keyDown)(MenuData* self, int id);
    void (*keyUp)(MenuData* self, int id);
    void (*refresh)(MenuData* self, byte what);
};
typedef struct Menu Menu;

void menuInit(Menu* self, void (*draw)(MenuData* self), 
              void (*encMove)(MenuData* self, int id,  int speed),  
              void (*keyDown)(MenuData* self, int id),
              void (*keyUp)(MenuData* self, int id),
              void (*refresh)(MenuData* self, byte what));

//default function
#define REFRESH_ALL 0
#define REFRESH_SHEET_POS 1
void menuRefresh(MenuData* self, byte what);

#endif
