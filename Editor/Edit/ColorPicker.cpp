//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ColorPicker.h"
#include "D3DUtils.h"
#include "xr_func.h"

class CTCD{
public:
	TColorDialog* cdColor;
public:
	CTCD(){
		cdColor = new TColorDialog(0);
        cdColor->Options = TColorDialogOptions()<<cdFullOpen;
    }
    ~CTCD(){
    	_DELETE(cdColor);
    }
};
static CTCD TCD;

extern "C" DLL_API bool FSColorPickerExecute(LPDWORD currentColor, LPDWORD originalColor, const int initialExpansionState);
bool SelectColor(LPDWORD currentcolor, LPDWORD originalcolor, bool bDefaultPicker){
	VERIFY(currentcolor);
	if (bDefaultPicker){
        if (originalcolor) TCD.cdColor->Color = rgb2bgr(*originalcolor);
        if (TCD.cdColor->Execute()){
			*currentcolor = bgr2rgb(TCD.cdColor->Color);
        	return true;
        }
        return false;
    }else{
	    return FSColorPickerExecute(currentcolor, originalcolor, 0);
    }
}

bool SelectColorWin(LPDWORD currentcolor, LPDWORD originalcolor, bool bDefaultPicker){
	VERIFY(currentcolor);
	if (bDefaultPicker){
        if (originalcolor) TCD.cdColor->Color = *originalcolor;
        if (TCD.cdColor->Execute()){
			*currentcolor = TCD.cdColor->Color;
        	return true;
        }
        return false;
    }else{
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
}

