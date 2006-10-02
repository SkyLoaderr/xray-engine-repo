#include "stdafx.h"
#pragma hdrstop

#include "ui_main.h"
#include "LogForm.h"

class CMSMain: public TUI
{
public:
    virtual void 	OutUICursorPos		(){}
	virtual void 	OutGridSize			(){}
	virtual void 	OutInfo				(){}
    virtual char* 	GetCaption			(){return "";}
    virtual void 	OutCameraPos		(){}
    virtual void 	SetStatus			(LPSTR s, bool bOutLog=true){}
    virtual void 	ResetStatus			(){}
    virtual bool 	ApplyShortCut		(WORD Key, TShiftState Shift){return false;}
    virtual bool 	ApplyGlobalShortCut	(WORD Key, TShiftState Shift){return false;}
    virtual LPCSTR 	EditorName			(){return "memstat";}
    virtual LPCSTR	EditorDesc			(){return "Memory Statistic";}
	virtual	void	RegisterCommands	(){}
    virtual void	ProgressDraw		(){}
    virtual void	RealQuit			(){}
    virtual void 	RealUpdateScene		(){}
};

//---------------------------------------------------------------------------
USEFORM("main.cpp", frmMain);
USEFORM("StatisticGrid.cpp", frmStatistic);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    if (!Application->Handle){ 
        Application->CreateHandle	(); 
        Application->Icon->Handle 	= LoadIcon(MainInstance, "MAINICON"); 
        Application->Title 			= "Loading...";
    } 
    Core._initialize		("memory_stat",ELogCallback);

    Application->Initialize	();

// startup create
    UI						= xr_new<CMSMain>();
    UI->RegisterCommands	();

    Application->Title 		= UI->EditorDesc();
    TfrmLog::CreateLog		();

    Application->CreateForm	(__classid(TfrmMain), &frmMain);
	Application->CreateForm	(__classid(TfrmStatistic), &frmStatistic);
	Application->Run		();

    TfrmLog::DestroyLog		(); 

    UI->ClearCommands		();
    xr_delete				(UI);

    Core._destroy			();
	return 0;
}
//---------------------------------------------------------------------------
