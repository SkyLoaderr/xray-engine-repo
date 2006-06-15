#pragma once

class CUIDialogWnd;
class CUIWindow;

class dlgItem{
public:
	dlgItem			(CUIWindow* pWnd);
	CUIWindow*		wnd;
	bool			enabled;
	bool operator < (const dlgItem& itm);
};

class CDialogHolder :public ISheduled,public pureFrame
{
	//dialogs
	xr_vector<CUIDialogWnd*>								m_input_receivers;
	xr_vector<dlgItem>										m_dialogsToRender;


	void					StartMenu						(CUIDialogWnd* pDialog);
	void					StopMenu						(CUIDialogWnd* pDialog);
	void					SetMainInputReceiver			(CUIDialogWnd* ir, bool _find_remove);
protected:
	void					DoRenderDialogs					();
	void					CleanInternals					();
public:
	CDialogHolder					();
	virtual					~CDialogHolder					();
	virtual	shared_str		shedule_Name					() const		{ return shared_str("CDialogHolder"); };
	virtual	void			shedule_Update					(u32 dt);
	virtual	float			shedule_Scale					();
	virtual bool			shedule_Needed					()				{return true;};

	//dialogs
	CUIDialogWnd*			MainInputReceiver				();
	virtual void			StartStopMenu					(CUIDialogWnd* pDialog, bool bDoHideIndicators);
	void					AddDialogToRender				(CUIWindow* pDialog);
	void					RemoveDialogToRender			(CUIWindow* pDialog);
	virtual void			OnFrame							();
};
