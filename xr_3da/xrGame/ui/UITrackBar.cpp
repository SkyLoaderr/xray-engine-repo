#include "StdAfx.h"

#include "UITrackBar.h"
//.#include "UITrackButton.h"
#include "UIFrameLineWnd.h"
#include "UI3tButton.h"
#include "UITextureMaster.h"
#include "../../xr_input.h"

#define DEF_CONTROL_HEIGHT		21
#define FRAME_LINE_TEXTURE		"ui_slider_e"
#define FRAME_LINE_TEXTURE_D	"ui_slider_d"
#define SLIDER_TEXTURE			"ui_slider_button"

CUITrackBar::CUITrackBar()
	: m_min(0),
	  m_max(1),
	  m_val(0),
	  m_back_up(0),
	 m_b_invert(false),
	 m_step(0.01f)
{	
	m_pFrameLine					= xr_new<CUIFrameLineWnd>();	
	AttachChild						(m_pFrameLine);	
	m_pFrameLine->SetAutoDelete		(true);
	m_pFrameLine_d					= xr_new<CUIFrameLineWnd>(); 
	m_pFrameLine_d->SetVisible		(false);
	AttachChild						(m_pFrameLine_d); 
	m_pFrameLine_d->SetAutoDelete	(true);
	m_pSlider						= xr_new<CUI3tButton>();			
	AttachChild						(m_pSlider);		
	m_pSlider->SetAutoDelete		(true);
//.	m_pSlider->SetOwner				(this);
}

bool CUITrackBar::OnMouse(float x, float y, EUIMessages mouse_action)
{
	CUIWindow::OnMouse(x, y, mouse_action);

	if (m_bCursorOverWindow)
	{
		if (pInput->iGetAsyncBtnState(0))
			UpdatePosRelativeToMouse();
	}
	return true;
}

void CUITrackBar::Init(float x, float y, float width, float height){
	string128			buf;
	float				item_height;
	float				item_width;
	CUIWindow::Init		(x, y, width, DEF_CONTROL_HEIGHT);


	item_height			= CUITextureMaster::GetTextureHeight(strconcat(buf,FRAME_LINE_TEXTURE,"_b"));
	m_pFrameLine->Init	(0, (height - item_height)/2, width, item_height);
	m_pFrameLine->InitTexture(FRAME_LINE_TEXTURE);
	m_pFrameLine_d->Init(0,(height - item_height)/2, width, item_height);
	m_pFrameLine_d->InitTexture(FRAME_LINE_TEXTURE_D);

	strconcat			(buf,SLIDER_TEXTURE,"_e");
	item_width			= CUITextureMaster::GetTextureWidth(buf);
    item_height			= CUITextureMaster::GetTextureHeight(buf);
	m_pSlider->Init		(0, (height - item_height)/2, item_width, item_height);
	m_pSlider->InitTexture(SLIDER_TEXTURE);
}	

void CUITrackBar::SetCurrentValue()
{
	GetOptFloatValue	(m_val, m_min, m_max);
	UpdatePos			();
}

//. #include "../HUDmanager.h"
void CUITrackBar::Draw()
{
	CUIWindow::Draw();
/*
	Fvector2 p;
	GetAbsolutePos(p);
	string128 buff;
	sprintf(buff,"[%.2f %.2f], %.2f",m_min,m_max,m_val);
	UI()->Font()->pFontSmall->Out(p.x, p.y, buff);
*/
}

void CUITrackBar::SaveValue()
{
	CUIOptionsItem::SaveValue	();
	SaveOptFloatValue			(m_val);
}

bool CUITrackBar::IsChanged()
{
	float val,min,max;
	GetOptFloatValue	(val, min, max);
	return val != m_val;    
}

void CUITrackBar::SeveBackUpValue()
{
	m_back_up		= m_val;
}

void CUITrackBar::Undo()
{
	m_val			= m_back_up;
	SaveValue		();
	SetCurrentValue	();
}

void CUITrackBar::Enable(bool status)
{
	m_bIsEnabled				= status;
	m_pFrameLine->SetVisible	(status);
	m_pSlider->Enable			(status);
	m_pFrameLine_d->SetVisible	(!status);
}

void CUITrackBar::UpdatePosRelativeToMouse()
{
	float btn_width				= m_pSlider->GetWidth();
	float window_width			= GetWidth();		
	float fpos					= cursor_pos.x;

	if( GetInvert() )
		fpos					= window_width - fpos;

	if (fpos < btn_width/2)
		fpos = btn_width/2;
	else 
	if (fpos > window_width - btn_width/2)
		fpos = window_width - btn_width/2;

	m_val						= (m_max - m_min)*(fpos - btn_width/2)/(window_width - btn_width)+ m_min;
	
	float _d	= (m_val-m_min);
	
	float _v	= _d/m_step;
	int _vi		= iFloor(_v);
	float _vf	= m_step*_vi;
	
	if(_d-_vf>m_step/2.0f)	
		_vf		+= m_step;

	m_val		= m_min+_vf;

	UpdatePos	();
}

void CUITrackBar::UpdatePos()
{
#ifdef DEBUG
	R_ASSERT2(m_val >= m_min && m_val <= m_max, "CUITrackBar::UpdatePos() - m_val >= m_min && m_val <= m_max" );
#endif

	float btn_width				= m_pSlider->GetWidth();
	float window_width			= GetWidth();		
	float free_space			= window_width - btn_width;
	Fvector2 pos				= m_pSlider->GetWndPos();
    
	pos.x						= (m_val - m_min)*free_space/(m_max - m_min);
	if( GetInvert() )
		pos.x					= free_space-pos.x;

	m_pSlider->SetWndPos		(pos);
	SaveValue					();
}

void CUITrackBar::OnMessage(const char* message)
{
	if (0 == xr_strcmp(message,"set_default_value"))
	{
//.		if ("r__detail_density" == m_entry)
//.			m_val = 0.3f;
//.		else
        m_val = m_min + (m_max - m_min)/2.0f;

		UpdatePos();
	}
}
