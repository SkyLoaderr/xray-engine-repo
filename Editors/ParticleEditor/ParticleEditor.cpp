#include "stdafx.h"
#pragma hdrstop
USEFORM("BottomBar.cpp", fraBottomBar);
USEFORM("EditorPref.cpp", frmEditorPreferences);
USEFORM("Splash.cpp", frmSplash);
USEFORM("main.cpp", frmMain);
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
USEFORM("LogForm.cpp", frmLog);
USEFORM("Editor\ChoseForm.cpp", frmChoseItem);
USEFORM("Editor\ShaderFunction.cpp", frmShaderFunction);
USEFORM("Editor\FrameEmitter.cpp", fraEmitter); /* TFrame: File Type */
USEFORM("Editor\TopBar.cpp", fraTopBar); /* TFrame: File Type */
USEFORM("Editor\TextForm.cpp", frmText);
USEFORM("Editor\ImageEditor.cpp", frmImageLib);
USEFORM("Editor\PropertiesList.cpp", frmProperties);
USEFORM("Editor\NumericVector.cpp", frmNumericVector);
USEFORM("Editor\ItemList.cpp", ItemList);
//---------------------------------------------------------------------------
#include "main.h"
#include "splash.h"
#include "UI_Main.h"
#include "LogForm.h"
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
//    try
    {
        frmSplash = xr_new<TfrmSplash>((TComponent*)0);
        frmSplash->Show();
        frmSplash->Repaint();

        frmSplash->SetStatus("Core Initialize...");

		Core._initialize		(_EDITOR_FILE_NAME_,ELogCallback);
        TfrmLog::CreateLog();

        Application->Initialize();

        frmSplash->SetStatus("Loading...");

// startup create
		Application->Title = "Particle Editor";
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		frmMain->SetHInst(hInst);

        xr_delete(frmSplash);

        Application->Run();

        TfrmLog::DestroyLog();
    }
//    catch (Exception &exception)
//    {
//           Application->ShowException(&exception);
//    }
    return 0;
}
//---------------------------------------------------------------------------




