#include "stdafx.h"
#pragma hdrstop
USERES("LevelEditor.res");
USERES("resource.res");
USE("!.txt", File);
USEUNIT("edit\Library.cpp");
USEUNIT("edit\Scene.cpp");
USEUNIT("edit\SceneUndo.cpp");
USEUNIT("edit\FileSystem.cpp");
USEUNIT("edit\SceneUtil.cpp");
USEUNIT("edit\D3DUtils.cpp");
USEUNIT("edit\Builder.cpp");
USEUNIT("EDIT\SceneClassList.cpp");
USEUNIT("edit\BuilderCore.cpp");
USEUNIT("edit\BuilderRemote.cpp");
USEUNIT("edit\Sound.cpp");
USEUNIT("edit\BuilderTexFiles.cpp");
USEUNIT("edit\BuilderLTX.cpp");
USEUNIT("edit\Glow.cpp");
USEUNIT("edit\TLSPRITE.CPP");
USEUNIT("edit\RPoint.cpp");
USEUNIT("edit\Log.cpp");
USEUNIT("edit\BuilderCF.cpp");
USEUNIT("edit\BuilderOGF.cpp");
USE("edit\std_classes.h", File);
USEUNIT("edit\frustum.cpp");
USEFORM("EditLibrary.cpp", frmEditLibrary);
USEUNIT("edit\sector.cpp");
USEUNIT("edit\portal.cpp");
USEUNIT("edit\portalutils.cpp");
USEUNIT("stdafx.cpp");
USEFORM("NumericVector.cpp", frmNumericVector);
USELIB("lib\LWO.lib");
USEUNIT("edit\EditMesh.cpp");
USEFORM("ShaderFunction.cpp", frmShaderFunction);
USEUNIT("edit\EditObjectImport.cpp");
USEUNIT("edit\EditMeshIO.cpp");
USEUNIT("edit\EditMeshPick.cpp");
USEUNIT("edit\EditMeshModify.cpp");
USEUNIT("edit\Device.cpp");
USEUNIT("edit\SceneIO.cpp");
USEUNIT("edit\EditMeshRender.cpp");
USEUNIT("edit\Event.cpp");
USEFORM("TopBar.cpp", fraTopBar);
USEFORM("LeftBar.cpp", fraLeftBar);
USEFORM("BottomBar.cpp", fraBottomBar);
USEFORM("FrameObject.cpp", fraObject);
USEFORM("FrameSector.cpp", fraSector);
USEFORM("FramePortal.cpp", fraPortal);
USEFORM("PropertiesLight.cpp", frmPropertiesLight);
USEFORM("PropertiesObject.cpp", frmPropertiesObject);
USEFORM("PropertiesSound.cpp", frmPropertiesSound);
USEFORM("PropertiesGlow.cpp", frmPropertiesGlow);
USEFORM("PropertiesSector.cpp", frmPropertiesSector);
USEFORM("PropertiesPortal.cpp", frmPropertiesPortal);
USEFORM("SceneProperties.cpp", frmSceneProperties);
USEFORM("ObjectList.cpp", frmObjectList);
USEFORM("EditorPref.cpp", frmEditorPreferences);
USEFORM("Splash.cpp", frmSplash);
USEFORM("main.cpp", frmMain);
USELIB("lib\cg32.lib");
USEFORM("PropertiesEvent.cpp", frmPropertiesEvent);
USEFORM("LogForm.cpp", frmLog);
USEUNIT("edit\EditObjectIO.cpp");
USEUNIT("PropertiesObjectSMotions.cpp");
USEUNIT("edit\EditObjectMotions.cpp");
USEUNIT("PropertiesObjectOMotions.cpp");
USEUNIT("PropertiesObjectMeshes.cpp");
USEUNIT("PropertiesObjectInfo.cpp");
USEUNIT("edit\BuilderSkeletonOGF.cpp");
USELIB("lib\xrProgressive.lib");
USEFORM("FrameLight.cpp", fraLight); /* TFrame: File Type */
USEFORM("OneEnvironment.cpp", frmOneEnvironment);
USELIB("lib\MagicFMd.lib");
USEUNIT("edit\PSLibrary.cpp");
USEFORM("PropertiesPS.cpp", frmPropertiesPS);
USEUNIT("edit\PSObject.cpp");
USEUNIT("engine\xrParticlesLib.cpp");
USE("engine\Particle.h", File);
USE("engine\ParticleSystem.h", File);
USEFORM("ChoseForm.cpp", frmChoseItem);
USEUNIT("engine\_quaternion.cpp");
USEUNIT("engine\_compressed_normal.cpp");
USEFORM("ImageEditor.cpp", frmTextureLib);
USEFORM("FramePS.cpp", fraPS); /* TFrame: File Type */
USEFORM("previewimage.cpp", frmPreviewImage);
USELIB("lib\DXT.lib");
USEFORM("FrameEmitter.cpp", fraEmitter); /* TFrame: File Type */
USEUNIT("edit\xr_trims.cpp");
USEFORM("PropertiesRPoint.cpp", frmPropertiesRPoint);
USEFORM("FrameRPoint.cpp", fraRPoint); /* TFrame: File Type */
USEUNIT("engine\interp.cpp");
USEUNIT("engine\Envelope.cpp");
USEUNIT("engine\motion.cpp");
USEUNIT("engine\bone.cpp");
USELIB("lib\gauss.lib");
USEUNIT("engine\clsid.cpp");
USEUNIT("engine\cl_raypick.cpp");
USEUNIT("engine\cl_build.cpp");
USEUNIT("engine\cl_collector.cpp");
USEUNIT("engine\cl_collide.cpp");
USEUNIT("engine\cl_intersect.cpp");
USEUNIT("engine\cl_overlap.cpp");
USEUNIT("engine\cl_RAPID.cpp");
USEUNIT("engine\cl_bboxcollide.cpp");
USEUNIT("edit\AITraffic.cpp");
USEFORM("FrameAITPoint.cpp", fraAITPoint); /* TFrame: File Type */
USEUNIT("edit\DetailObjects.cpp");
USEFORM("FrameDetObj.cpp", fraDetailObject); /* TFrame: File Type */
USEUNIT("engine\xr_ini.cpp");
USE("engine\Communicate.h", File);
USEUNIT("edit\ColorPicker.cpp");
USEUNIT("edit\Occluder.cpp");
USEFORM("FrameOccluder.cpp", fraOccluder); /* TFrame: File Type */
USEFORM("PropertiesOccluder.cpp", frmPropertiesOccluder);
USEUNIT("engine\LzHuf.cpp");
USEUNIT("edit\UI_AITPointTools.cpp");
USEUNIT("edit\UI_Camera.cpp");
USEUNIT("edit\ui_control.cpp");
USEUNIT("edit\UI_CustomTools.cpp");
USEUNIT("edit\UI_DOTools.cpp");
USEUNIT("edit\UI_EventTools.cpp");
USEUNIT("edit\UI_GlowTools.cpp");
USEUNIT("edit\UI_LightTools.cpp");
USEUNIT("edit\UI_Main.cpp");
USEUNIT("edit\UI_MainCommand.cpp");
USEUNIT("edit\UI_ObjectTools.cpp");
USEUNIT("edit\UI_OccluderTools.cpp");
USEUNIT("edit\UI_PortalTools.cpp");
USEUNIT("edit\UI_PSTools.cpp");
USEUNIT("edit\UI_RPointTools.cpp");
USEUNIT("edit\UI_SectorTools.cpp");
USEUNIT("edit\UI_SoundTools.cpp");
USEUNIT("edit\ui_tools.cpp");
USEUNIT("edit\ELight.cpp");
USEUNIT("engine\FS.cpp");
USEFORM("DOShuffle.cpp", frmDOShuffle);
USEFORM("DOOneColor.cpp", frmOneColor);
USEUNIT("edit\DetailObjectsRender.cpp");
USEFORM("PropertiesDetailObject.cpp", frmPropertiesDO);
USEUNIT("edit\DetailObjectsPick.cpp");
USE("!TODO.txt", File);
USEUNIT("edit\HW.cpp");
USEUNIT("engine\HWCaps.cpp");
USEUNIT("engine\PrimitivesR.cpp");
USEUNIT("engine\Primitive.cpp");
USEUNIT("engine\SharedPrimitive.cpp");
USELIB("lib\DX\d3d8.lib");
USELIB("lib\DX\d3dx8d.lib");
USEUNIT("edit\EditObject.cpp");
USEUNIT("engine\Xr_input.cpp");
USEUNIT("engine\fcontroller.cpp");
USEUNIT("engine\XR_CustomFont.cpp");
USEUNIT("engine\XR_HUDFont.cpp");
USELIB("lib\DX\dinput.lib");
USEUNIT("lib\DX\c_dinput.cpp");
USEUNIT("engine\pure.cpp");
USEUNIT("edit\CustomObject.cpp");
USEUNIT("edit\SceneObject.cpp");
USEUNIT("edit\SceneObjectIO.cpp");
USEUNIT("engine\xrImage_Resampler.cpp");
USEUNIT("Engine\TextureManager_Loader.cpp");
USEUNIT("Engine\Texture.cpp");
USEUNIT("edit\Cursor3D.cpp");
USEUNIT("Engine\SH_Matrix.cpp");
USEUNIT("Engine\SH_Texture.cpp");
USEUNIT("Engine\Shader.cpp");
USEUNIT("Engine\SH_Constant.cpp");
USEUNIT("Engine\xr_avi.cpp");
USEUNIT("Engine\Blenders\Blender_Recorder.cpp");
USEUNIT("Engine\Blenders\Blender.cpp");
USEUNIT("Engine\Blenders\Blender_Vertex.cpp");
USEUNIT("Engine\Blenders\Blender_Editor_Selection.cpp");
USEUNIT("Engine\Blenders\Blender_Editor_Wire.cpp");
USEUNIT("Engine\Blenders\Blender_Screen_GRAY.cpp");
USEUNIT("Engine\Blenders\Blender_Screen_SET.cpp");
USEUNIT("Engine\Blenders\Blender_default_aref.cpp");
USEUNIT("Engine\Blenders\Blender_Vertex_aref.cpp");
USEUNIT("Engine\Blenders\BlenderDefault.cpp");
USEUNIT("Engine\Image.cpp");
USEUNIT("Engine\tss_def.cpp");
USEOBJ("Intermediate\cpuid.obj");
USEOBJ("Intermediate\_math.obj");
USEUNIT("Engine\xrLoadSurface.cpp");
USEUNIT("edit\ImageManager.cpp");
USEUNIT("edit\ImageThumbnail.cpp");
USEUNIT("Engine\TextureManager.cpp");
USEFORM("PropertiesList.cpp", frmProperties);
USEUNIT("edit\ETextureParams.cpp");
USEUNIT("Engine\Blenders\blender_light.cpp");
USEUNIT("edit\FolderLib.cpp");
USEUNIT("edit\EStats.cpp");
USEUNIT("edit\UI_MainExtern.cpp");
USEUNIT("Engine\xr_efflensflare.cpp");
USELIB("lib\ETools.lib");
USEFORM("PropertiesSceneObject.cpp", frmPropertiesSceneObject);
USEUNIT("edit\EditObjectEditor.cpp");
USEUNIT("Engine\Blenders\Blender_LaEmB.cpp");
USEUNIT("Engine\Blenders\Blender_Lm(EbB).cpp");
USEUNIT("Engine\Blenders\Blender_LmBmmD.cpp");
USEUNIT("Engine\fmesh.cpp");
USEUNIT("Engine\Blenders\Blender_BmmD.cpp");
USELIB("lib\FreeImage.lib");
USEUNIT("edit\BuilderGame.cpp");
USEFORM("EventOneAction.cpp", frmOneEventAction);
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


