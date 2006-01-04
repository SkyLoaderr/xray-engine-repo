#pragma once

class CUIWindow;
class CUIStatic;
class CTutorialItem;
class CUIXml;

class CUIGameTutorial :public pureFrame, public pureRender,	public IInputReceiver
{
protected:
	IInputReceiver*			m_pStoredInputReceiver;
	CUIWindow*				m_UIWindow;
	xr_deque<CTutorialItem*>m_items;
	bool					m_bActive;
	bool					m_bPlayEachItem;
	bool					GrabInput			();
public:
							CUIGameTutorial		();
	void					Start				(LPCSTR tutor_name);
	void					Stop				();
	void					Next				();

	virtual void			OnFrame				();
	virtual void			OnRender			();
	CUIWindow*				MainWnd				()				{return m_UIWindow;}
	bool					IsActive			()				{return m_bActive;}


	//IInputReceiver
	virtual void			IR_OnMousePress		(int btn);
	virtual void			IR_OnMouseRelease	(int btn);
	virtual void			IR_OnMouseHold		(int btn);
	virtual void			IR_OnMouseMove		(int x, int y);
	virtual void			IR_OnMouseStop		(int x, int y);

	virtual void			IR_OnKeyboardPress	(int dik);
	virtual void			IR_OnKeyboardRelease(int dik);
	virtual void			IR_OnKeyboardHold	(int dik);

	virtual void			IR_OnMouseWheel		(int direction)	;
};

class CTutorialItem
{
	struct SSubItem{
		CUIStatic*			m_wnd;
		float				m_start;
		float				m_length;
		bool				m_visible;
	public:
		virtual				~SSubItem			(){}
		virtual	void		Update				(){}
		virtual	void		OnRender			(){}

		virtual void		Start				();
		virtual void		Stop				();
	};
	struct SVideoSubItem:	public SSubItem{
		ref_sound			m_sound[2];
		CTexture*			m_texture;
		bool				m_played;
	public:
							SVideoSubItem():m_texture(0),m_played(false){}
		virtual	void		Update				();
		virtual	void		OnRender			();

		virtual void		Start				();
		virtual void		Stop				();
	};
	DEFINE_VECTOR			(SSubItem*,SubItemVec,SubItemVecIt);
	SubItemVec				m_subitems;
	enum {	
		etiNeedPauseOn		= (1<<0),
		etiNeedPauseOff		= (1<<1),
		etiStoredPauseState	= (1<<5),
		etiCanBeStopped		= (1<<6),
		etiGrabInput		= (1<<7)
	};
public:
	CUIWindow*				m_UIWindow;
	ref_sound				m_sound;
	float					m_time_start;
	float					m_time_length;
	string64				m_pda_section;
	Fvector2				m_desired_cursor_pos;
	Flags32					m_flags;
	int						m_continue_dik_guard;
public:
							~CTutorialItem		();
	void					Load				(CUIXml* xml,int idx);
	void					Update				();
	void					Start				(CUIGameTutorial*);
	bool					Stop				(CUIGameTutorial*, bool bForce=false);
	void					OnKeyboardPress		(int dik);
	bool					GrabInput			()					{return !!m_flags.test(etiGrabInput);}
	void					OnRender			();
};