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
USEFORM("PropertiesLight.cpp", frmPropertiesLight);
USEFORM("PropertiesSound.cpp", frmPropertiesSound);
USEFORM("PropertiesGlow.cpp", frmPropertiesGlow);
USEFORM("PropertiesSector.cpp", frmPropertiesSector);
USEFORM("PropertiesPortal.cpp", frmPropertiesPortal);
USEFORM("SceneProperties.cpp", frmSceneProperties);
USEFORM("ObjectList.cpp", frmObjectList);
USEFORM("EditorPref.cpp", frmEditorPreferences);
USEFORM("Splash.cpp", frmSplash);
USEFORM("main.cpp", frmMain);
USEFORM("PropertiesEvent.cpp", frmPropertiesEvent);
USEFORM("LogForm.cpp", frmLog);
USEFORM("FrameLight.cpp", fraLight); /* TFrame: File Type */
USEFORM("OneEnvironment.cpp", frmOneEnvironment);
USEFORM("PropertiesPS.cpp", frmPropertiesPS);
USEFORM("ChoseForm.cpp", frmChoseItem);
USEFORM("ImageEditor.cpp", frmTextureLib);
USEFORM("FramePS.cpp", fraPS); /* TFrame: File Type */
USEFORM("previewimage.cpp", frmPreviewImage);
USEFORM("FrameEmitter.cpp", fraEmitter); /* TFrame: File Type */
USEFORM("PropertiesRPoint.cpp", frmPropertiesSpawnPoint);
USEFORM("FrameRPoint.cpp", fraSpawnPoint); /* TFrame: File Type */
USEFORM("FrameWayPoint.cpp", fraWayPoint); /* TFrame: File Type */
USEFORM("FrameDetObj.cpp", fraDetailObject); /* TFrame: File Type */
USEFORM("DOShuffle.cpp", frmDOShuffle);
USEFORM("DOOneColor.cpp", frmOneColor);
USEFORM("PropertiesDetailObject.cpp", frmPropertiesDO);
USEFORM("PropertiesList.cpp", frmProperties);
USEFORM("PropertiesSceneObject.cpp", frmPropertiesSceneObject);
USEFORM("EventOneAction.cpp", frmOneEventAction);
USEFORM("EditLightAnim.cpp", frmEditLightAnim);
USEFORM("PropertiesEObject.cpp", frmPropertiesEObject);
USEFORM("FrameGroup.cpp", fraGroup); /* TFrame: File Type */
USEFORM("PropertiesGroup.cpp", frmPropertiesGroup);
USEFORM("FrameEvent.cpp", fraEvent); /* TFrame: File Type */
USEFORM("TextForm.cpp", frmText);
USEFORM("PropertiesWayPoint.cpp", frmPropertiesWayPoint);
//---------------------------------------------------------------------------
#include "main.h"
#include "splash.h"
#include "UI_Main.h"
#include "LogForm.h"
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    try{
        TfrmLog::CreateLog();

        frmSplash = new TfrmSplash(0);
        frmSplash->Show();
        frmSplash->Repaint();

        frmSplash->SetStatus("Initializing");

        Application->Initialize();

        frmSplash->SetStatus("Loading...");

// startup create
		Application->Title = "Level Editor";
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmEditorPreferences), &frmEditorPreferences);
		frmMain->SetHInst(hInst);

        _DELETE(frmSplash);

        Application->Run();

        TfrmLog::DestroyLog();
    }
    catch (Exception &exception)
    {
           Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------


