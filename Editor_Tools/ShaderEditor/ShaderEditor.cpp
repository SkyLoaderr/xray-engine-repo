#include "stdafx.h"
#pragma hdrstop
USERES("ShaderEditor.res");
USE("!.txt", File);
USEUNIT("stdafx.cpp");
USEUNIT("editor\Texture.cpp");
USEUNIT("editor\FileSystem.cpp");
USEUNIT("editor\D3DUtils.cpp");
USEUNIT("editor\Log.cpp");
USE("editor\Fmesh.h", File);
USE("editor\std_classes.h", File);
USEUNIT("editor\frustum.cpp");
USELIB("lib\LWO.lib");
USEUNIT("editor\EditMesh.cpp");
USEUNIT("editor\Shader.cpp");
USEUNIT("editor\EditObjectImport.cpp");
USEUNIT("editor\EditMeshIO.cpp");
USEUNIT("editor\EditMeshModify.cpp");
USEUNIT("editor\Device.cpp");
USEUNIT("editor\TextureIO.cpp");
USEUNIT("editor\TextureThumbnail.cpp");
USEUNIT("editor\EditMeshRender.cpp");
USEFORM("TopBar.cpp", fraTopBar);
USEFORM("BottomBar.cpp", fraBottomBar);
USEFORM("EditorPref.cpp", frmEditorPreferences);
USEFORM("Splash.cpp", frmSplash);
USEFORM("main.cpp", frmMain);
USELIB("lib\cg32.lib");
USEFORM("LogForm.cpp", frmLog);
USEUNIT("editor\EditObjectIO.cpp");
USEUNIT("editor\EditObjectMotions.cpp");
USELIB("lib\xrProgressive.lib");
USEUNIT("engine\xrShaderLib.cpp");
USEUNIT("engine\XRShader.cpp");
USEUNIT("engine\ShaderManager.cpp");
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
USELIB("lib\CreateDX.lib");
USEUNIT("engine\cl_raypick.cpp");
USEUNIT("engine\cl_build.cpp");
USEUNIT("engine\cl_collector.cpp");
USEUNIT("engine\cl_collide.cpp");
USEUNIT("engine\cl_intersect.cpp");
USEUNIT("engine\cl_overlap.cpp");
USEUNIT("engine\cl_RAPID.cpp");
USEUNIT("engine\cl_bboxcollide.cpp");
USEUNIT("engine\xr_ini.cpp");
USEUNIT("editor\ColorPicker.cpp");
USEUNIT("engine\LzHuf.cpp");
USEUNIT("editor\UI_Camera.cpp");
USEUNIT("UI_Main.cpp");
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
USEUNIT("engine\EStats.cpp");
USEUNIT("engine\XR_CustomFont.cpp");
USEUNIT("engine\XR_HUDFont.cpp");
USELIB("lib\dinput.lib");
USEUNIT("lib\c_dinput.cpp");
USEUNIT("engine\pure.cpp");
USEUNIT("engine\xrImage_Resampler.cpp");
USEOBJ("Intermediate\cpuid.obj");
USEOBJ("Intermediate\_math.obj");
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
USEUNIT("ShaderTools.cpp");
USEFORM("FrameProperties.cpp", fraProperties); /* TFrame: File Type */
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



