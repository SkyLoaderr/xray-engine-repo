#include "stdafx.h"
#pragma hdrstop
USERES("ed.res");
USE("!.txt", File);
USEUNIT("edit\Texture.cpp");
USEUNIT("edit\Library.cpp");
USEUNIT("edit\SceneObject.cpp");
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
USE("edit\Fmesh.h", File);
USE("edit\std_classes.h", File);
USEUNIT("edit\frustum.cpp");
USEFORM("EditLibrary.cpp", frmEditLibrary);
USEUNIT("edit\sector.cpp");
USEUNIT("edit\portal.cpp");
USEUNIT("edit\portalutils.cpp");
USEUNIT("stdafx.cpp");
USEFORM("NumericVector.cpp", frmNumericVector);
USELIB("lib\LWO.lib");
USEUNIT("edit\EditObject.cpp");
USEUNIT("edit\EditMesh.cpp");
USEUNIT("edit\Shader.cpp");
USEFORM("EditShaders.cpp", frmEditShaders);
USEFORM("OneStage.cpp", frmOneStage);
USEFORM("ShaderFunction.cpp", frmShaderFunction);
USEFORM("ShaderModificator.cpp", frmShaderModificator);
USEUNIT("edit\EditObjectImport.cpp");
USEUNIT("edit\EditMeshIO.cpp");
USEUNIT("edit\EditMeshPick.cpp");
USEUNIT("edit\EditMeshModify.cpp");
USEUNIT("edit\Device.cpp");
USEUNIT("edit\DPatch.cpp");
USEUNIT("edit\TextureIO.cpp");
USEUNIT("edit\TextureThumbnail.cpp");
USEUNIT("edit\SceneIO.cpp");
USEUNIT("edit\EditMeshRender.cpp");
USEUNIT("edit\Event.cpp");
USEFORM("TopBar.cpp", fraTopBar);
USEFORM("LeftBar.cpp", fraLeftBar);
USEFORM("BottomBar.cpp", fraBottomBar);
USEFORM("FrameObject.cpp", fraObject);
USEFORM("FrameSector.cpp", fraSector);
USEFORM("FramePortal.cpp", fraPortal);
USEFORM("FrameDPatch.cpp", fraParticles);
USEFORM("PropertiesLight.cpp", frmPropertiesLight);
USEFORM("PropertiesObject.cpp", frmPropertiesObject);
USEFORM("PropertiesSound.cpp", frmPropertiesSound);
USEFORM("PropertiesGlow.cpp", frmPropertiesGlow);
USEFORM("PropertiesSector.cpp", frmPropertiesSector);
USEFORM("PropertiesPortal.cpp", frmPropertiesPortal);
USEFORM("PropertiesShader.cpp", frmPropertiesShader);
USEFORM("SceneProperties.cpp", frmSceneProperties);
USEFORM("ObjectList.cpp", frmObjectList);
USEFORM("EditorPref.cpp", frmEditorPreferences);
USEFORM("Splash.cpp", frmSplash);
USEUNIT("Cursor3D.cpp");
USEFORM("main.cpp", frmMain);
USELIB("lib\cg32.lib");
USEFORM("PropertiesEvent.cpp", frmPropertiesEvent);
USEFORM("LogForm.cpp", frmLog);
USELIB("lib\d3dx8d.lib");
USEUNIT("edit\EditObjectIO.cpp");
USEUNIT("PropertiesObjectSMotions.cpp");
USEUNIT("edit\EditObjectMotions.cpp");
USEUNIT("PropertiesObjectOMotions.cpp");
USEUNIT("PropertiesObjectSurfaces.cpp");
USEUNIT("PropertiesObjectMeshes.cpp");
USEUNIT("PropertiesObjectInfo.cpp");
USEUNIT("edit\BuilderSkeletonOGF.cpp");
USELIB("lib\xrProgressive.lib");
USEFORM("FrameLight.cpp", fraLight); /* TFrame: File Type */
USEFORM("OneEnvironment.cpp", frmOneEnvironment);
USEUNIT("shared\xrShaderLib.cpp");
USEUNIT("shared\XRShader.cpp");
USEUNIT("shared\ShaderManager.cpp");
USEUNIT("shared\tss.cpp");
USEFORM("StatisticForm.cpp", frmStatistic);
USELIB("lib\MagicFMd.lib");
USEFORM("EditParticles.cpp", frmEditParticles);
USEUNIT("edit\PSLibrary.cpp");
USEFORM("PropertiesPS.cpp", frmPropertiesPS);
USEUNIT("edit\PSObject.cpp");
USEUNIT("shared\xrParticlesLib.cpp");
USE("shared\Particle.h", File);
USE("shared\ParticleSystem.h", File);
USEFORM("PropertiesPSDef.cpp", frmPropertiesPSDef);
USEFORM("ChoseForm.cpp", frmChoseItem);
USEUNIT("shared\_quaternion.cpp");
USEUNIT("shared\_compressed_normal.cpp");
USEFORM("ImageEditor.cpp", frmTextureLib);
USEFORM("FramePS.cpp", fraPS); /* TFrame: File Type */
USEFORM("previewimage.cpp", frmPreviewImage);
USELIB("lib\DXT.lib");
USE("shared\nvdxt_options.h", File);
USEFORM("FrameEmitter.cpp", fraEmitter); /* TFrame: File Type */
USEUNIT("edit\xr_trims.cpp");
USEFORM("PropertiesRPoint.cpp", frmPropertiesRPoint);
USEFORM("FrameRPoint.cpp", fraRPoint); /* TFrame: File Type */
USEUNIT("shared\interp.cpp");
USEUNIT("shared\Envelope.cpp");
USEUNIT("shared\motion.cpp");
USEUNIT("shared\bone.cpp");
USELIB("lib\gauss.lib");
USEUNIT("shared\clsid.cpp");
USELIB("lib\CreateDX.lib");
USEUNIT("shared\cl_raypick.cpp");
USEUNIT("shared\cl_build.cpp");
USEUNIT("shared\cl_collector.cpp");
USEUNIT("shared\cl_collide.cpp");
USEUNIT("shared\cl_intersect.cpp");
USEUNIT("shared\cl_overlap.cpp");
USEUNIT("shared\cl_RAPID.cpp");
USEUNIT("shared\cl_bboxcollide.cpp");
USEUNIT("edit\AITraffic.cpp");
USEFORM("FrameAITPoint.cpp", fraAITPoint); /* TFrame: File Type */
USEUNIT("edit\DetailObjects.cpp");
USEFORM("FrameDetObj.cpp", fraDetailObject); /* TFrame: File Type */
USEUNIT("shared\xr_ini.cpp");
USE("shared\Communicate.h", File);
USEUNIT("edit\ColorPicker.cpp");
USEUNIT("edit\Occluder.cpp");
USEFORM("FrameOccluder.cpp", fraOccluder); /* TFrame: File Type */
USEFORM("PropertiesOccluder.cpp", frmPropertiesOccluder);
USEUNIT("edit\BuilderDO.cpp");
USEUNIT("shared\LzHuf.cpp");
USEUNIT("edit\UI_AITPointTools.cpp");
USEUNIT("edit\UI_Camera.cpp");
USEUNIT("edit\ui_control.cpp");
USEUNIT("edit\UI_CustomTools.cpp");
USEUNIT("edit\UI_D3D.cpp");
USEUNIT("edit\UI_DOTools.cpp");
USEUNIT("edit\UI_DPatchTools.cpp");
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
USEUNIT("shared\FS.cpp");
USEUNIT("edit\Statistic.cpp");
USEOBJ("ed_debug\_math.obj");
USEOBJ("ed_debug\cpuid.obj");
USEFORM("DOShuffle.cpp", frmDOShuffle);
USEFORM("DOOneColor.cpp", frmOneColor);
USEUNIT("edit\DetailObjectsRender.cpp");
USEFORM("PropertiesDetailObject.cpp", frmPropertiesDO);
USEUNIT("edit\DetailObjectsPick.cpp");
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


