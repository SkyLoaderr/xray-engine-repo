#include "stdafx.h"
#pragma hdrstop
USEFORM("EditLibrary.cpp", frmEditLibrary);
USEFORM("NumericVector.cpp", frmNumericVector);
USEFORM("ShaderFunction.cpp", frmShaderFunction);
USEFORM("TopBar.cpp", fraTopBar);
USEFORM("LeftBar.cpp", fraLeftBar);
USEFORM("BottomBar.cpp", fraBottomBar);
USEFORM("FrameObject.cpp", fraObject);
USEFORM("FrameSector.cpp", fraSector);
USEFORM("FramePortal.cpp", fraPortal);
USEFORM("SceneProperties.cpp", frmSceneProperties);
USEFORM("ObjectList.cpp", frmObjectList);
USEFORM("EditorPref.cpp", frmEditorPreferences);
USEFORM("Splash.cpp", frmSplash);
USEFORM("main.cpp", frmMain);
USEFORM("LogForm.cpp", frmLog);
USEFORM("FrameLight.cpp", fraLight); /* TFrame: File Type */
USEFORM("OneEnvironment.cpp", frmOneEnvironment);
USEFORM("ChoseForm.cpp", frmChoseItem);
USEFORM("ImageEditor.cpp", frmTextureLib);
USEFORM("FramePS.cpp", fraPS); /* TFrame: File Type */
USEFORM("previewimage.cpp", frmPreviewImage);
USEFORM("FrameEmitter.cpp", fraEmitter); /* TFrame: File Type */
USEFORM("FrameRPoint.cpp", fraSpawnPoint); /* TFrame: File Type */
USEFORM("FrameWayPoint.cpp", fraWayPoint); /* TFrame: File Type */
USEFORM("FrameDetObj.cpp", fraDetailObject); /* TFrame: File Type */
USEFORM("DOShuffle.cpp", frmDOShuffle);
USEFORM("DOOneColor.cpp", frmOneColor);
USEFORM("PropertiesDetailObject.cpp", frmPropertiesDO);
USEFORM("PropertiesList.cpp", Properties);
USEFORM("EditLightAnim.cpp", frmEditLightAnim);
USEFORM("PropertiesEObject.cpp", frmPropertiesEObject);
USEFORM("FrameGroup.cpp", fraGroup); /* TFrame: File Type */
USEFORM("TextForm.cpp", frmText);
USEFORM("PropertiesWayPoint.cpp", frmPropertiesWayPoint);
USEFORM("FrameShape.cpp", fraShape); /* TFrame: File Type */
USEFORM("FrameAIMap.cpp", fraAIMap); /* TFrame: File Type */
USEFORM("edit\ItemList.cpp", ItemList);
//---------------------------------------------------------------------------
#include "main.h"
#include "splash.h"
#include "LogForm.h"
#include "EditorPref.h"
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
		Application->CreateForm(__classid(TfrmEditorPreferences), &frmEditorPreferences);
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


