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

class CDialogHolder :public ISheduled
{
	//dialogs
	xr_stack<CUIDialogWnd*>									m_input_receivers;
/*	xr_vector<CUIWindow*>									m_dialogsToRender;
	xr_vector<CUIWindow*>									m_dialogsToErase;
*/
	xr_vector<dlgItem>										m_dialogsToRender;


	void					StartMenu						(CUIDialogWnd* pDialog);
	void					StopMenu						(CUIDialogWnd* pDialog);
	void					SetMainInputReceiver			(CUIDialogWnd* ir);
protected:
	void					DoRenderDialogs					();
	void					CleanInternals					();
public:
	CDialogHolder					();
	virtual					~CDialogHolder					();
	virtual	void			shedule_Update					(u32 dt);
	virtual	float			shedule_Scale					();

	//dialogs
	CUIDialogWnd*			MainInputReceiver				();
	virtual void			StartStopMenu					(CUIDialogWnd* pDialog, bool bDoHideIndicators);
	void					AddDialogToRender				(CUIWindow* pDialog);
	void					RemoveDialogToRender			(CUIWindow* pDialog);

};
