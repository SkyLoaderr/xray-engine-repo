#include "stdafx.h"
#pragma hdrstop
#include "main.h"
#include "splash.h"
#include "LogForm.h"
#include "EditorPref.h"
//---------------------------------------------------------------------------
USEFORM("edit\ItemList.cpp", ItemList);
USEFORM("TopBar.cpp", fraTopBar); /* TFrame: File Type */
USEFORM("BottomBar.cpp", fraBottomBar); /* TFrame: File Type */
USEFORM("ChoseForm.cpp", frmChoseItem);
USEFORM("DOOneColor.cpp", frmOneColor);
USEFORM("DOShuffle.cpp", frmDOShuffle);
USEFORM("EditLibrary.cpp", frmEditLibrary);
USEFORM("EditLightAnim.cpp", frmEditLightAnim);
USEFORM("EditorPref.cpp", frmEditPrefs);
USEFORM("FrameAIMap.cpp", fraAIMap); /* TFrame: File Type */
USEFORM("FrameDetObj.cpp", fraDetailObject); /* TFrame: File Type */
USEFORM("FrameEmitter.cpp", fraEmitter); /* TFrame: File Type */
USEFORM("FrameGroup.cpp", fraGroup); /* TFrame: File Type */
USEFORM("FrameLight.cpp", fraLight); /* TFrame: File Type */
USEFORM("FramePortal.cpp", fraPortal); /* TFrame: File Type */
USEFORM("FramePS.cpp", fraPS); /* TFrame: File Type */
USEFORM("FrameRPoint.cpp", fraSpawnPoint); /* TFrame: File Type */
USEFORM("FrameSector.cpp", fraSector); /* TFrame: File Type */
USEFORM("FrameShape.cpp", fraShape); /* TFrame: File Type */
USEFORM("FrameWayPoint.cpp", fraWayPoint); /* TFrame: File Type */
USEFORM("ImageEditor.cpp", frmImageLib);
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
USEFORM("LogForm.cpp", frmLog);
USEFORM("main.cpp", frmMain);
USEFORM("NumericVector.cpp", frmNumericVector);
USEFORM("ObjectList.cpp", frmObjectList);
USEFORM("OneEnvironment.cpp", frmOneEnvironment);
USEFORM("previewimage.cpp", frmPreviewImage);
USEFORM("PropertiesEObject.cpp", frmPropertiesEObject);
USEFORM("PropertiesList.cpp", Properties);
USEFORM("SceneProperties.cpp", frmSceneProperties);
USEFORM("ShaderFunction.cpp", frmShaderFunction);
USEFORM("Splash.cpp", frmSplash);
USEFORM("TextForm.cpp", frmText);
USEFORM("FrameObject.cpp", fraObject); /* TFrame: File Type */
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
//    try{
        frmSplash 					= xr_new<TfrmSplash>((TComponent*)0);
        frmSplash->Show				();
        frmSplash->Repaint			();

        frmSplash->SetStatus		("Core initializing...");

    	Core._initialize			(_EDITOR_FILE_NAME_,ELogCallback);
        TfrmLog::CreateLog			();

        Application->Initialize		();

        frmSplash->SetStatus		("Loading...");

// startup create
		Application->Title 			= _EDITOR_NAME_;
        Application->CreateForm(__classid(TfrmMain), &frmMain);
		frmMain->SetHInst			(hInst);

        xr_delete					(frmSplash);

		Application->Run			();

        TfrmLog::DestroyLog			();
//    }catch (Exception &exception){
//           Application->ShowException(&exception);
//    }
    return 0;
}
//---------------------------------------------------------------------------


