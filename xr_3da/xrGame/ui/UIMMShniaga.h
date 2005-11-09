#pragma once

#include "UIWindow.h"

class CUIAnimatedStatic;
class CUIStatic;
class CUIXml;
class CUIScrollView;

class CUIMMShniaga : public CUIWindow{
public:
	using CUIWindow::Init;

	CUIMMShniaga();
	~CUIMMShniaga();

			void Init(CUIXml& xml_doc, LPCSTR path);
	virtual void Update();
	virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);
			bool IsButton(CUIWindow* st);

protected:
			void ShowMain();
			void ShowNewGame();
	float	pos(float x1, float x2, u32 t);

    CUIStatic*			m_shniaga;
	CUIStatic*			m_magnifier;
	CUIStatic*			m_anims[2];
	CUIStatic*			m_gratings[2];
	CUIScrollView*		m_view;

    u32					m_start_time;
	u32					m_run_time;
    float				m_origin;
	float				m_destination;

	xr_vector<CUIStatic*>	m_buttons;
	xr_vector<CUIStatic*>	m_buttons_new;
    CUIWindow*			m_selected;
};