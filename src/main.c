#include <malloc.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void draw_square_2d(f32 x, f32 y, f32 size) {
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

    GX_Position2f32(x - size, y - size); GX_Color4u8(255, 255, 255, 255);
    GX_Position2f32(x + size, y - size); GX_Color4u8(255, 255, 255, 255);
    GX_Position2f32(x + size, y + size); GX_Color4u8(255, 255, 255, 255);
    GX_Position2f32(x - size, y + size); GX_Color4u8(255, 255, 255, 255);

    GX_End();
}

int __crtmain(){
    VIDEO_Init();
    WPAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    void *gp_fifo = MEM_K0_TO_K1(memalign(32, 256 * 1024));
    memset(gp_fifo, 0, 256 * 1024);
    GX_Init(gp_fifo, 256 * 1024);
    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
    GX_InvalidateTexAll();

    Mtx44 perspective;
    guOrtho(perspective, 0, rmode->efbHeight, 0, rmode->fbWidth, 0, 1);
    GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);

    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GX_SetColorUpdate(GX_TRUE);
    GX_SetAlphaUpdate(GX_TRUE);

    while (1) {
        WPAD_ScanPads();

        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;

        GX_SetCopyClear((GXColor){0, 0, 0, 255}, 0x00FFFFFF);

        GX_ClearVtxDesc();
        GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
        GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

        draw_square_2d(rmode->fbWidth / 2, rmode->efbHeight / 2, 50.0f);

        GX_DrawDone();
        GX_CopyDisp(xfb, GX_TRUE);
        VIDEO_WaitVSync();
    }

    return 0;
}
