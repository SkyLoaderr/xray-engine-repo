#include "stdafx.h"
#pragma hdrstop
#include "main.h"
#include "splash.h"
#include "UI_Main.h"
#include "LogForm.h"
//---------------------------------------------------------------------------
USEFORM("BonePart.cpp", frmBonePart);
USEFORM("BottomBar.cpp", fraBottomBar); /* TFrame: File Type */
USEFORM("EditorPref.cpp", frmEditPrefs);
USEFORM("KeyBar.cpp", frmKeyBar);
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
USEFORM("LogForm.cpp", frmLog);
USEFORM("main.cpp", frmMain);
USEFORM("Splash.cpp", frmSplash);
USEFORM("Editor\ChoseForm.cpp", frmChoseItem);
USEFORM("Editor\FrameEmitter.cpp", fraEmitter); /* TFrame: File Type */
USEFORM("Editor\ImageEditor.cpp", frmImageLib);
USEFORM("Editor\ItemList.cpp", ItemList);
USEFORM("Editor\NumericVector.cpp", frmNumericVector);
USEFORM("Editor\PropertiesList.cpp", Properties);
USEFORM("Editor\ShaderFunction.cpp", frmShaderFunction);
USEFORM("Editor\TextForm.cpp", frmText);
USEFORM("Editor\TopBar.cpp", fraTopBar); /* TFrame: File Type */
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
//    try{
        frmSplash 				= xr_new<TfrmSplash>((TComponent*)0);
        frmSplash->Show			();
        frmSplash->Repaint		();
        frmSplash->SetStatus	("Core initialize...");

    	Core._initialize		(_EDITOR_FILE_NAME_,ELogCallback);
        TfrmLog::CreateLog		();

        Application->Initialize	();

        frmSplash->SetStatus	("Loading...");

// startup create
		Application->Title 		= _EDITOR_NAME_;
		Application->CreateForm	(__classid(TfrmMain), &frmMain);
		Application->CreateForm	(__classid(TfrmBonePart), &frmBonePart);
		frmMain->SetHInst		(hInst);

        xr_delete				(frmSplash);

        Application->Run		();

        TfrmLog::DestroyLog		();
    	Core._destroy			();
//    }
//    catch (Exception &exception)
//    {
//           Application->ShowException(&exception);
//    }
    return 0;
}
//---------------------------------------------------------------------------




