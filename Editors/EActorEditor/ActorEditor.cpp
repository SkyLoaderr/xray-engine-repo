#include "stdafx.h"
#pragma hdrstop

#include "main.h"
#include "splash.h"
#include "UI_ActorMain.h"
#include "UI_Tools.h"
#include "LogForm.h"
//---------------------------------------------------------------------------
USEFORM("BonePart.cpp", frmBonePart);
USEFORM("ClipEditor.cpp", ClipMaker);
USEFORM("KeyBar.cpp", frmKeyBar);
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
USEFORM("Splash.cpp", frmSplash);
USEFORM("main.cpp", frmMain);
USEFORM("TopBar.cpp", fraTopBar); /* TFrame: File Type */
USEFORM("BottomBar.cpp", fraBottomBar); /* TFrame: File Type */
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
//    try{
        frmSplash 				= xr_new<TfrmSplash>((TComponent*)0);
        frmSplash->Show			();
        frmSplash->Repaint		();
        frmSplash->SetStatus	("Core initializing...");

    	Core._initialize		("editor",ELogCallback);

        Application->Initialize	();
                                       
        frmSplash->SetStatus	("Loading...");

// startup create
        Tools					= xr_new<CActorTools>();
        UI						= xr_new<CActorMain>();

		Application->Title 		= UI->EditorDesc();
        TfrmLog::CreateLog		();

		xr_delete(frmSplash);
        
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		frmMain->SetHInst		(hInst);

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

