#include "stdafx.h"
#pragma hdrstop
#include "main.h"
#include "splash.h"
#include "UI_Main.h"
#include "LogForm.h"
//---------------------------------------------------------------------------
USEFORM("Editor\ChoseForm.cpp", frmChoseItem);
USEFORM("Editor\FrameEmitter.cpp", fraEmitter); /* TFrame: File Type */
USEFORM("Editor\ImageEditor.cpp", frmImageLib);
USEFORM("Editor\ItemList.cpp", ItemList);
USEFORM("Editor\NumericVector.cpp", frmNumericVector);
USEFORM("Editor\PropertiesList.cpp", Properties);
USEFORM("Editor\ShaderFunction.cpp", frmShaderFunction);
USEFORM("Editor\TextForm.cpp", frmText);
USEFORM("Editor\TopBar.cpp", fraTopBar); /* TFrame: File Type */
USEFORM("BottomBar.cpp", fraBottomBar); /* TFrame: File Type */
USEFORM("EditorPref.cpp", frmEditPrefs);
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
USEFORM("LogForm.cpp", frmLog);
USEFORM("main.cpp", frmMain);
USEFORM("PropertiesPSDef.cpp", frmPropertiesPSDef);
USEFORM("Splash.cpp", frmSplash);
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




