//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ColorPicker.h"

extern "C" DLL_API bool FSColorPickerExecute(LPDWORD currentColor, LPDWORD originalColor, const int initialExpansionState);
bool SelectColor(LPDWORD currentcolor, LPDWORD originalcolor){
    return FSColorPickerExecute(currentcolor, originalcolor, 0);
}

DWORD bgr2rgb(DWORD val){
    BYTE r, g, b;
    r = (BYTE) (val >>  0);
    g = (BYTE) (val >>  8);
    b = (BYTE) (val >> 16);
    return ((DWORD)(r<<16) | (g<<8) | (b));
}

DWORD rgb2bgr(DWORD val){
    BYTE r, g, b;
    r = (BYTE) (val >> 16);
    g = (BYTE) (val >>  8);
    b = (BYTE) (val >> 0);
    return ((DWORD)(b<<16) | (g<<8) | (r));
}

bool SelectColorWin(LPDWORD currentcolor, LPDWORD originalcolor){
	VERIFY(currentcolor);
	DWORD cur = bgr2rgb(*currentcolor);
    bool bRes;
    if (originalcolor){
		DWORD orig = bgr2rgb(*originalcolor);
		bRes = FSColorPickerExecute(&cur, &orig, 0);
    }else
		bRes = FSColorPickerExecute(&cur, 0, 0);

    if (bRes) *currentcolor = rgb2bgr(cur);

    return bRes;
}

