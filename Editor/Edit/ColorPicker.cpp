//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ColorPicker.h"
#include "D3DUtils.h"

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
        if (originalcolor) TCD.cdColor->Color = DU::rgb2bgr(*originalcolor);
        if (TCD.cdColor->Execute()){
			*currentcolor = DU::bgr2rgb(TCD.cdColor->Color);
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
}

