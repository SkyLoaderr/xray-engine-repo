#include "stdafx.h"
#pragma hdrstop
#include "splash.h"
#include "LogForm.h"
#include "main.h"
#include "UI_LevelMain.h"
#include "UI_LevelTools.h"
//---------------------------------------------------------------------------
USEFORM("BottomBar.cpp", fraBottomBar); /* TFrame: File Type */
USEFORM("main.cpp", frmMain);
USEFORM("TopBar.cpp", fraTopBar); /* TFrame: File Type */
USEFORM("DOOneColor.cpp", frmOneColor);
USEFORM("DOShuffle.cpp", frmDOShuffle);
USEFORM("EditLibrary.cpp", frmEditLibrary);
USEFORM("EditLightAnim.cpp", frmEditLightAnim);
USEFORM("FrameAIMap.cpp", fraAIMap);
USEFORM("FrameDetObj.cpp", fraDetailObject);
USEFORM("FrameEmitter.cpp", fraEmitter); /* TFrame: File Type */
USEFORM("FrameGroup.cpp", fraGroup);
USEFORM("FrameLight.cpp", fraLight);
USEFORM("FrameObject.cpp", fraObject);
USEFORM("FramePortal.cpp", fraPortal);
USEFORM("FramePS.cpp", fraPS);
USEFORM("FrameSector.cpp", fraSector);
USEFORM("FrameShape.cpp", fraShape);
USEFORM("FrameSpawn.cpp", fraSpawn);
USEFORM("FrameWayPoint.cpp", fraWayPoint);
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
USEFORM("ObjectList.cpp", frmObjectList);
USEFORM("previewimage.cpp", frmPreviewImage);
USEFORM("PropertiesEObject.cpp", frmPropertiesEObject);
USEFORM("SceneProperties.cpp", frmSceneProperties);
USEFORM("ShaderFunction.cpp", frmShaderFunction);
USEFORM("Splash.cpp", frmSplash);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
//    try{
        frmSplash 				= xr_new<TfrmSplash>((TComponent*)0);
        frmSplash->Show			();
        frmSplash->Repaint		();
        frmSplash->SetStatus	("Core initializing...");

    	Core._initialize		("level",ELogCallback);

        Application->Initialize	();
        Application->Icon		= frmSplash->Icon;
                                       
        frmSplash->SetStatus	("Loading...");

// startup create
        Tools					= xr_new<CLevelTools>();
        UI						= xr_new<CLevelMain>();
		Application->Title 		= UI->EditorDesc();
        TfrmLog::CreateLog		();

		Application->CreateForm(__classid(TfrmMain), &frmMain);
		frmMain->SetHInst		(hInst);

		xr_delete(frmSplash);

		Application->Run		();


        TfrmLog::DestroyLog		();

        xr_delete				(Tools);
        xr_delete				(UI);

    	Core._destroy			();
//    }
//    catch (Exception &exception)
//    {
//           Application->ShowException(&exception);
//    }
    return 0;
}
//---------------------------------------------------------------------------




