#include "stdafx.h"
#pragma hdrstop
USEFORM("BottomBar.cpp", fraBottomBar);
USEFORM("EditorPref.cpp", frmEditorPreferences);
USEFORM("Splash.cpp", frmSplash);
USEFORM("main.cpp", frmMain);
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
USEFORM("LogForm.cpp", frmLog);
USEFORM("Editor\ChoseForm.cpp", frmChoseItem);
USEFORM("Editor\TopBar.cpp", fraTopBar); /* TFrame: File Type */
USEFORM("Editor\PropertiesList.cpp", frmProperties);
USEFORM("BonePart.cpp", frmBonePart);
USEFORM("KeyBar.cpp", frmKeyBar);
USEFORM("Editor\NumericVector.cpp", frmNumericVector);
USEFORM("Editor\TextForm.cpp", frmText);
USEFORM("Editor\ImageEditor.cpp", frmImageLib);
USEFORM("Editor\ShaderFunction.cpp", frmShaderFunction);
USEFORM("Editor\ItemList.cpp", ItemList);
//---------------------------------------------------------------------------
#include "main.h"
#include "splash.h"
#include "UI_Main.h"
#include "LogForm.h"
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
//    try{
        frmSplash = xr_new<TfrmSplash>((TComponent*)0);
        frmSplash->Show();
        frmSplash->Repaint();
        frmSplash->SetStatus("Core initialize...");

    	Core._initialize	(_EDITOR_FILE_NAME_,ELogCallback);
        TfrmLog::CreateLog();

        Application->Initialize();

        frmSplash->SetStatus("Loading...");

// startup create
		Application->Title = "Actor Editor";
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmEditorPreferences), &frmEditorPreferences);
		Application->CreateForm(__classid(TfrmBonePart), &frmBonePart);
		frmMain->SetHInst(hInst);

        xr_delete(frmSplash);

        Application->Run();

        TfrmLog::DestroyLog();
    	Core._destroy();
//    }
//    catch (Exception &exception)
//    {
//           Application->ShowException(&exception);
//    }
    return 0;
}
//---------------------------------------------------------------------------




