#include "Pch.h"
#pragma hdrstop
USERES("ed.res");
USEUNIT("ui_tools.cpp");
USEFORM("BuildProgress.cpp", frmBuildProgress);
USEFORM("main.cpp", frmMain);
USEUNIT("edit\UI_D3D.cpp");
USEUNIT("edit\UI_Main.cpp");
USEUNIT("edit\Texture.cpp");
USEUNIT("edit\StaticMesh.cpp");
USEUNIT("edit\Library.cpp");
USEUNIT("edit\SObject2.cpp");
USEUNIT("edit\SceneObject.cpp");
USEUNIT("edit\Scene.cpp");
USEUNIT("edit\SceneUndo.cpp");
USEUNIT("edit\FileSystem.cpp");
USEUNIT("edit\SceneUtil.cpp");
USEUNIT("edit\Light.cpp");
USEUNIT("edit\D3DUtils.cpp");
USELIB("lib\ddraw.lib");
USEUNIT("edit\Log.cpp");
USEUNIT("UI_LightTools.cpp");
USEFORM("SelectLight.cpp", fraSelectLight); /* TFrame: File Type */
USEFORM("AddLight.cpp", fraAddLight); /* TFrame: File Type */
USEFORM("MoveLight.cpp", fraMoveLight); /* TFrame: File Type */
USEFORM("PropertiesLight.cpp", frmPropertiesLight);
USE("edit\PCH.H", File);
USEUNIT("UI_ObjectTools.cpp");
USEFORM("SelectObject.cpp", fraSelectObject); /* TFrame: File Type */
USEFORM("MoveObject.cpp", fraMoveObject); /* TFrame: File Type */
USEFORM("PropertiesObject.cpp", frmPropertiesObject);
USEFORM("RotateObject.cpp", fraRotateObject); /* TFrame: File Type */
USEFORM("AddObject.cpp", fraAddObject); /* TFrame: File Type */
USEFORM("ChoseObject.cpp", frmChoseObject);
USEUNIT("ui_control.cpp");
USELIB("D:\USERS\Alexmx\bcb\CBuilder5\Lib\cg32.lib");
USEUNIT("EDIT\Ffileops.cpp");
USEUNIT("edit\Builder.cpp");
USEUNIT("edit\SceneCopyPaste.cpp");
USEUNIT("EDIT\SceneClassList.cpp");
USEUNIT("edit\BuilderCore.cpp");
USEFORM("SceneProperties.cpp", frmSceneProperties);
USE("!.txt", File);
USE("communicate.h", File);
USEUNIT("edit\BuilderRemote.cpp");
USEFORM("PropertiesSubObject.cpp", frmPropertiesSubObject);
USEFORM("AddSound.cpp", fraAddSound); /* TFrame: File Type */
USEFORM("SelectSound.cpp", fraSelectSound); /* TFrame: File Type */
USEFORM("MoveSound.cpp", fraMoveSound); /* TFrame: File Type */
USEFORM("PropertiesSound.cpp", frmPropertiesSound);
USEUNIT("UI_SoundTools.cpp");
USEUNIT("edit\Sound.cpp");
USEUNIT("edit\BuilderTexFiles.cpp");
USEUNIT("edit\BuilderLTX.cpp");
USEFORM("ObjectList.cpp", frmObjectList);
USEFORM("Splash.cpp", frmSplash);
USEUNIT("visibilitytester.cpp");
USEUNIT("vt_sphere.cpp");
USEUNIT("Statistic.cpp");
USE("ed.todo", ToDo);
USEUNIT("edit\LzHuf.cpp");
USEUNIT("Rapid\cl_raypick.cpp");
USEUNIT("Rapid\cl_collide.cpp");
USEUNIT("Rapid\cl_intersect.cpp");
USEUNIT("Rapid\cl_overlap.cpp");
USEUNIT("Rapid\cl_RAPID.cpp");
USEUNIT("Rapid\cl_build.cpp");
USEUNIT("edit\Primitives.cpp");
USEFORM("SelectPClipper.cpp", fraSelectPClipper); /* TFrame: File Type */
USEFORM("MovePClipper.cpp", fraMovePClipper); /* TFrame: File Type */
USEFORM("RotatePClipper.cpp", fraRotatePClipper); /* TFrame: File Type */
USEFORM("AddPClipper.cpp", fraAddPClipper); /* TFrame: File Type */
USEUNIT("UI_PClipperTools.cpp");
USEFORM("PropertiesPClipper.cpp", frmPropertiesPClipper);
USEUNIT("edit\PClipper.cpp");
USEFORM("ScalePClipper.cpp", fraScalePClipper); /* TFrame: File Type */
USEUNIT("ui_scenetools.cpp");
USEFORM("MoveScene.cpp", fraMoveScene); /* TFrame: File Type */
USEFORM("EditLibrary.cpp", frmEditLibrary);
USE("edit\StaticMeshChunks.h", File);
USEUNIT("edit\gizmo.cpp");
USEFORM("SelectTexturing.cpp", fraSelectTexturing); /* TFrame: File Type */
USEUNIT("edit\texturizer.cpp");
USEUNIT("UI_TexturingTools.cpp");
USELIB("lib\CreateDX.lib");
USEFORM("TopBar.cpp", fraTopBar); /* TFrame: File Type */
//---------------------------------------------------------------------------
#include "main.h"
#include "splash.h"
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    try{
        frmSplash = new TfrmSplash(0);
        frmSplash->Show();
        frmSplash->Repaint();

        frmSplash->SetStatus("Initializing");

        Application->Initialize();

        frmSplash->SetStatus("Loading...");
        Application->CreateForm(__classid(TfrmMain), &frmMain);
         Application->CreateForm(__classid(TfrmBuildProgress), &frmBuildProgress);
         Application->CreateForm(__classid(TfraSelectLight), &fraSelectLight);
         Application->CreateForm(__classid(TfraAddLight), &fraAddLight);
         Application->CreateForm(__classid(TfraMoveLight), &fraMoveLight);
         Application->CreateForm(__classid(TfrmPropertiesLight), &frmPropertiesLight);
         Application->CreateForm(__classid(TfrmPropertiesObject), &frmPropertiesObject);
         Application->CreateForm(__classid(TfraSelectObject), &fraSelectObject);
         Application->CreateForm(__classid(TfraMoveObject), &fraMoveObject);
         Application->CreateForm(__classid(TfraRotateObject), &fraRotateObject);
         Application->CreateForm(__classid(TfraAddObject), &fraAddObject);
         Application->CreateForm(__classid(TfrmChoseObject), &frmChoseObject);
         Application->CreateForm(__classid(TfrmSceneProperties), &frmSceneProperties);
         Application->CreateForm(__classid(TfrmPropertiesSubObject), &frmPropertiesSubObject);
         Application->CreateForm(__classid(TfrmPropertiesSound), &frmPropertiesSound);
         Application->CreateForm(__classid(TfraAddSound), &fraAddSound);
         Application->CreateForm(__classid(TfraSelectSound), &fraSelectSound);
         Application->CreateForm(__classid(TfraMoveSound), &fraMoveSound);
         Application->CreateForm(__classid(TfrmObjectList), &frmObjectList);
         Application->CreateForm(__classid(TfrmPropertiesPClipper), &frmPropertiesPClipper);
         Application->CreateForm(__classid(TfraSelectPClipper), &fraSelectPClipper);
         Application->CreateForm(__classid(TfraMovePClipper), &fraMovePClipper);
         Application->CreateForm(__classid(TfraRotatePClipper), &fraRotatePClipper);
         Application->CreateForm(__classid(TfraAddPClipper), &fraAddPClipper);
         Application->CreateForm(__classid(TfraScalePClipper), &fraScalePClipper);
         Application->CreateForm(__classid(TfraMoveScene), &fraMoveScene);
         Application->CreateForm(__classid(TfrmEditLibrary), &frmEditLibrary);
         Application->CreateForm(__classid(TfraSelectTexturing), &fraSelectTexturing);
         frmMain->SetHInst(hInst);

        _DELETE(frmSplash);
        Application->Run();
    }
    catch (Exception &exception)
    {
           Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
