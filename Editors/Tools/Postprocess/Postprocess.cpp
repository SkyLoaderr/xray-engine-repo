//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include "_types.h"
#include <xrCore.h>
#include <y:\postprocess\core_hdr\vector.h>
//#include "CameraManager.h"
#include <stdio.h>

#define cosf cos
#define fabsf fabs
#define sinf sin
#define sqrtf sqrt
#define Device NULL

#pragma hdrstop

#pragma comment (lib, "x:\xrCoreB.lib")
//---------------------------------------------------------------------------
USEFORM("main.cpp", MainForm);
USEFORM("float_param.cpp", Form9);
USEFORM("color_constructor.cpp", Form6);
USEFORM("color_param.cpp", Form7);
USEFORM("float_constructor.cpp", Form8);
USEFORM("Color.cpp", ColorForm);
USEFORM("single_param.cpp", SingleParam);
//---------------------------------------------------------------------------
FILE                        *g_LogFileHandle            =       NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void    log_callback    (LPCSTR lpString)
{
    if (!g_LogFileHandle)
       return;
    fprintf (g_LogFileHandle, "%s\n", lpString);
}


WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        g_LogFileHandle = fopen ("postprocess.log", "wt");
         Core._initialize ("Postprocess editor", log_callback, FALSE);
         FS._initialize (CLocatorAPI::flScanAppRoot, 0, 0);
         Application->Initialize();
         Application->CreateForm(__classid(TMainForm), &MainForm);
         Application->Run();
         Core._destroy();
         fclose (g_LogFileHandle);
    }
    catch (Exception &exception)
    {
        fclose (g_LogFileHandle);
         Application->ShowException(&exception);
    }
    catch (...)
    {
         try
         {
             throw Exception("");
         }
         catch (Exception &exception)
         {
            fclose (g_LogFileHandle);
             Application->ShowException(&exception);
         }
    }
    return 0;
}
//---------------------------------------------------------------------------
