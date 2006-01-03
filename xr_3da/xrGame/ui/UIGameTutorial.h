#pragma once

class CUIWindow;
class TutorialItem;
class CUIXml;

class CUIGameTutorial :public pureFrame, public pureRender,	public IInputReceiver
{
protected:
	IInputReceiver*				m_pStoredInputReceiver;
	CUIWindow*					m_UIWindow;
	xr_deque<TutorialItem*>		m_items;
	bool						m_bActive;
	bool						m_bPlayEachItem;
	bool						GrabInput				();
public:
							CUIGameTutorial();
	void					Start		(LPCSTR tutor_name);
	void					Stop		();
	void					Next		();

	virtual void			OnFrame		();
	virtual void			OnRender	();
	CUIWindow*				MainWnd		()				{return m_UIWindow;}
	bool					IsActive	()				{return m_bActive;}


	//IInputReceiver
	virtual void	IR_OnMousePress					(int btn);
	virtual void	IR_OnMouseRelease				(int btn);
	virtual void	IR_OnMouseHold					(int btn);
	virtual void	IR_OnMouseMove					(int x, int y);
	virtual void	IR_OnMouseStop					(int x, int y);

	virtual void	IR_OnKeyboardPress				(int dik);
	virtual void	IR_OnKeyboardRelease			(int dik);
	virtual void	IR_OnKeyboardHold				(int dik);

	virtual void	IR_OnMouseWheel					(int direction)	;
};

class TutorialItem
{
	struct SUIHighlightedAreas{
		shared_str			m_wnd_name;
		float				m_start;
		float				m_length;
		bool				m_bshown;
	};
	xr_vector<SUIHighlightedAreas>	m_ui_highlighting;
	enum {	etiNeedPauseOn			= (1<<0),
			etiNeedPauseOff			= (1<<1),
			etiStoredPauseState		= (1<<5),
			etiCanBeStopped			= (1<<6),
			etiGrabInput			= (1<<7),
			etiSoundStereo			= (1<<8)
		};
public:
								~TutorialItem();
	CUIWindow*					m_UIWindow;
	ref_sound					m_sound_m;
	ref_sound					m_sound_s_l;
	ref_sound					m_sound_s_r;
	float						m_time_start;
	float						m_time_length;
	shared_str					m_snd_name;
	string64					m_pda_section;
	Fvector2					m_desired_cursor_pos;
	Flags32						m_flags;
	int							m_continue_dik_guard;

public:
	void						Load				(CUIXml* xml,int idx);
	void						Update				();
	void						Start				(CUIGameTutorial*);
	bool						Stop				(CUIGameTutorial*, bool bForce=false);
	void						OnKeyboardPress		(int dik);
	bool						GrabInput			()					{return !!m_flags.test(etiGrabInput);}
};