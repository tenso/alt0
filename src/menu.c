#include "menu.h"

void menuInit(Menu* menu, void (*draw)(MenuData* self), void (*encMove)(MenuData* self, int id,  int speed), 
              void (*keyDown)(MenuData* self, int id), void (*keyUp)(MenuData* self, int id), 
              void (*refresh)(MenuData* self, byte what))
{
    menu->self.currentSub = 0;
    menu->self.needRefresh = false;

    menu->draw = draw;
    menu->encMove = encMove;
    menu->keyDown = keyDown;
    menu->keyUp = keyUp;
    menu->refresh = refresh;

    if (menu->refresh == 0)
    {
        menu->refresh = &menuRefresh;
    }
    menu->self.currentQuick = 0;
    menu->self.show = false;

    menu->self.needRefresh = true;
}

void menuRefresh(MenuData* self, byte what)
{
    self->needRefresh = true;
}
