#include "stdafx.h"
#pragma hdrstop
#include "splash.h"
#include "LogForm.h"
#include "main.h"
#include "ui_particlemain.h"
#include "UI_ParticleTools.h"
//---------------------------------------------------------------------------
USEFORM("Splash.cpp", frmSplash);
USEFORM("main.cpp", frmMain);
USEFORM("BottomBar.cpp", fraBottomBar); /* TFrame: File Type */
USEFORM("TopBar.cpp", fraTopBar); /* TFrame: File Type */
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
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
        Tools					= xr_new<CParticleTools>();
        UI						= xr_new<CParticleMain>();
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




