#include "stdafx.h"
#pragma hdrstop
USERES("ShaderEditor.res");
USERES("resource.res");
USE("!.txt", File);
USEUNIT("stdafx.cpp");
USEUNIT("editor\FileSystem.cpp");
USEUNIT("editor\D3DUtils.cpp");
USEUNIT("editor\Log.cpp");
USE("editor\Fmesh.h", File);
USE("editor\std_classes.h", File);
USEUNIT("editor\frustum.cpp");
USELIB("lib\LWO.lib");
USEUNIT("editor\EditMesh.cpp");
USEUNIT("editor\EditObjectImport.cpp");
USEUNIT("editor\EditMeshIO.cpp");
USEUNIT("editor\EditMeshModify.cpp");
USEUNIT("editor\Device.cpp");
USEUNIT("editor\EditMeshRender.cpp");
USEFORM("BottomBar.cpp", fraBottomBar);
USEFORM("EditorPref.cpp", frmEditorPreferences);
USEFORM("Splash.cpp", frmSplash);
USEFORM("main.cpp", frmMain);
USELIB("lib\cg32.lib");
USEUNIT("editor\EditObjectIO.cpp");
USEUNIT("editor\EditObjectMotions.cpp");
USELIB("lib\xrProgressive.lib");
USELIB("lib\MagicFMd.lib");
USEUNIT("engine\_quaternion.cpp");
USEUNIT("engine\_compressed_normal.cpp");
USELIB("lib\DXT.lib");
USE("engine\nvdxt_options.h", File);
USEUNIT("editor\xr_trims.cpp");
USEUNIT("engine\interp.cpp");
USEUNIT("engine\Envelope.cpp");
USEUNIT("engine\motion.cpp");
USEUNIT("engine\bone.cpp");
USEUNIT("engine\clsid.cpp");
USEUNIT("engine\xr_ini.cpp");
USEUNIT("editor\ColorPicker.cpp");
USEUNIT("engine\LzHuf.cpp");
USEUNIT("editor\UI_Camera.cpp");
USEUNIT("UI_MainCommand.cpp");
USEUNIT("engine\FS.cpp");
USEUNIT("editor\HW.cpp");
USEUNIT("engine\HWCaps.cpp");
USEUNIT("engine\PrimitivesR.cpp");
USEUNIT("engine\Primitive.cpp");
USEUNIT("engine\SharedPrimitive.cpp");
USELIB("lib\d3d8.lib");
USELIB("lib\d3dx8d.lib");
USEUNIT("editor\EditObject.cpp");
USEUNIT("engine\Xr_input.cpp");
USEUNIT("engine\fcontroller.cpp");
USEUNIT("engine\XR_CustomFont.cpp");
USEUNIT("engine\XR_HUDFont.cpp");
USELIB("lib\dinput.lib");
USEUNIT("lib\c_dinput.cpp");
USEUNIT("engine\pure.cpp");
USEUNIT("engine\xrImage_Resampler.cpp");
USEOBJ("Intermediate\cpuid.obj");
USEOBJ("Intermediate\_math.obj");
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
USEFORM("PropertiesShader.cpp", frmShaderProperties);
USEFORM("LogForm.cpp", frmLog);
USEUNIT("Editor\Library.cpp");
USEFORM("Editor\ChoseForm.cpp", frmChoseItem);
USEUNIT("Engine\xr_avi.cpp");
USEUNIT("Engine\SH_Constant.cpp");
USEUNIT("Engine\SH_Matrix.cpp");
USEUNIT("Engine\SH_Texture.cpp");
USEUNIT("Engine\Shader.cpp");
USEUNIT("Engine\Texture.cpp");
USEUNIT("Engine\TextureManager.cpp");
USEUNIT("Engine\TextureManager_Loader.cpp");
USEUNIT("Engine\tss_def.cpp");
USEUNIT("Engine\Image.cpp");
USEUNIT("Engine\tga.cpp");
USEFORM("Editor\ShaderFunction.cpp", frmShaderFunction);
USEUNIT("Engine\BLENDERS\BlenderDefault.cpp");
USEUNIT("Engine\BLENDERS\Blender_default_aref.cpp");
USEUNIT("Engine\BLENDERS\Blender_Editor_Selection.cpp");
USEUNIT("Engine\BLENDERS\Blender_Editor_Wire.cpp");
USEUNIT("Engine\BLENDERS\Blender_Recorder.cpp");
USEUNIT("Engine\BLENDERS\Blender_Screen_GRAY.cpp");
USEUNIT("Engine\BLENDERS\Blender_Screen_SET.cpp");
USEUNIT("Engine\BLENDERS\Blender_Vertex.cpp");
USEUNIT("Engine\BLENDERS\Blender_Vertex_aref.cpp");
USEUNIT("Engine\BLENDERS\Blender.cpp");
USEUNIT("SHEngineTools.cpp");
USEUNIT("SHCompilerTools.cpp");
USEUNIT("Engine\BLENDERS\blender_light.cpp");
USEUNIT("Editor\FolderLib.cpp");
USEUNIT("Editor\EStats.cpp");
USEUNIT("Engine\xrLoadSurface.cpp");
USELIB("Lib\FreeImage.lib");
USEUNIT("Editor\UI_Main.cpp");
USEUNIT("UI_Tools.cpp");
USEUNIT("UI_MainExtern.cpp");
USEUNIT("Editor\ImageThumbnail.cpp");
USEUNIT("Editor\ImageManager.cpp");
USELIB("Lib\ETools.lib");
USEUNIT("UI_MainShader.cpp");
USEUNIT("Editor\EditMeshPick.cpp");
USEFORM("Editor\TopBar.cpp", fraTopBar); /* TFrame: File Type */
USEUNIT("Editor\EditObjectEditor.cpp");
USELIB("Lib\memmgr.lib");
USEUNIT("Engine\BLENDERS\Blender_LaEmB.cpp");
USEUNIT("Engine\BLENDERS\Blender_LmBmmD.cpp");
USEUNIT("Engine\BLENDERS\Blender_Lm(EbB).cpp");
USEUNIT("Engine\BLENDERS\Blender_BmmD.cpp");
USEUNIT("Editor\ModelPool.cpp");
USEUNIT("Engine\Animations\xrSkin1W.cpp");
USEUNIT("Engine\Animations\FBasicVisual.cpp");
USEUNIT("Engine\Animations\FHierrarhyVisual.cpp");
USEUNIT("Engine\Animations\FProgressive.cpp");
USEUNIT("Engine\Animations\FProgressiveFixedVisual.cpp");
USEUNIT("Engine\Animations\FVisual.cpp");
USEUNIT("Engine\Animations\SkeletonCalculate.cpp");
USEUNIT("Engine\Animations\SkeletonX.cpp");
USEUNIT("Engine\Animations\xrBoneLerp.cpp");
USEUNIT("Engine\Animations\BodyInstance.cpp");
USELIB("Lib\xrCDB.lib");
USEUNIT("Engine\xrXRC.cpp");
USEUNIT("Editor\Engine.cpp");
USEFORM("Editor\PropertiesList.cpp", frmProperties);
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
		Application->Title = "Shader Editor";
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



