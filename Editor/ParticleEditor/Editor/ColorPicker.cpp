//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ColorPicker.h"

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
bool SelectColor(LPDWORD currentcolor, bool bDefaultPicker){
	VERIFY(currentcolor);
	if (bDefaultPicker){
        TCD.cdColor->Color = TColor(rgb2bgr(*currentcolor));
        if (TCD.cdColor->Execute()){
			*currentcolor = bgr2rgb(TCD.cdColor->Color);
        	return true;
        }
        return false;
    }else{
    	DWORD clr=*currentcolor;
  	    if (FSColorPickerExecute(&clr, 0, 0)){
        	*currentcolor = clr;
         	return true;
        }
        return false;
    }
}

bool SelectColorWin(LPDWORD currentcolor, bool bDefaultPicker){
	VERIFY(currentcolor);
	if (bDefaultPicker){
        TCD.cdColor->Color = TColor(*currentcolor);
        if (TCD.cdColor->Execute()){
			*currentcolor = TCD.cdColor->Color;
        	return true;
        }
        return false;
    }else{
        DWORD cur = bgr2rgb(*currentcolor);
        if (FSColorPickerExecute(&cur, 0, 0)){
			*currentcolor = rgb2bgr(cur);
        	return true;
        }
	    return false;
    }
}

