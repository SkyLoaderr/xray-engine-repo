//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ColorPicker.h"
#include "D3DUtils.h"

extern "C" DLL_API bool FSColorPickerExecute(LPDWORD currentColor, LPDWORD originalColor, const int initialExpansionState);
bool SelectColor(LPDWORD currentcolor, LPDWORD originalcolor){
    return FSColorPickerExecute(currentcolor, originalcolor, 0);
}

bool SelectColorWin(LPDWORD currentcolor, LPDWORD originalcolor){
	VERIFY(currentcolor);
	DWORD cur = DU::bgr2rgb(*currentcolor);
    bool bRes;
    if (originalcolor){
		DWORD orig = DU::bgr2rgb(*originalcolor);
		bRes = FSColorPickerExecute(&cur, &orig, 0);
    }else
		bRes = FSColorPickerExecute(&cur, 0, 0);

    if (bRes) *currentcolor = DU::rgb2bgr(cur);

    return bRes;
}

