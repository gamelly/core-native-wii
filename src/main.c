#include <grrlib.h>
#include <wiiuse/wpad.h>

bool dvmInitDefault() {
	return true;
}

int __crtmain(int argc, char **argv) {
    GRRLIB_Init();
    
    WPAD_Init();
    
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    
    while (1) {
        WPAD_ScanPads();
        
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) {
            break;
        }
        
        GRRLIB_Rectangle(100, 100, 200, 150, 0xFF0000FF, 1);
        
        GRRLIB_Render();
    }
    
    GRRLIB_Exit();
    return 0;
}