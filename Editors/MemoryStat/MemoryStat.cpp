#include "stdafx.h"
#pragma hdrstop

//---------------------------------------------------------------------------
USEFORM("main.cpp", frmMain);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        if (!Application->Handle){ 
            Application->CreateHandle	(); 
            Application->Icon->Handle 	= LoadIcon(MainInstance, "MAINICON"); 
			Application->Title 			= "Loading...";
        } 
    	Core._initialize		("memory_stat",0);

        Application->Initialize	();
                                       
// startup create
//.		TfrmLog::CreateLog		();

		Application->CreateForm(__classid(TfrmMain), &frmMain);

		Application->Run		();

//.		TfrmLog::DestroyLog		(); 

    	Core._destroy			();
/*
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->Run();
	}
	catch (Exception &exception)
	{
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
			Application->ShowException(&exception);
		}
	}
*/    
	return 0;
}
//---------------------------------------------------------------------------
