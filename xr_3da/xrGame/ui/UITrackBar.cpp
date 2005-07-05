#include "StdAfx.h"

#include "UITrackBar.h"
#include "UIFrameLineWnd.h"
#include "UI3tButton.h"
#include "UITextureMaster.h"

#define DEF_CONTROL_HEIGHT		21
#define FRAME_LINE_TEXTURE		"ui_slider_e"
#define FRAME_LINE_TEXTURE_D	"ui_slider_d"
#define SLIDER_TEXTURE			"ui_slider_button"

CUITrackBar::CUITrackBar()
	: m_min(0),
	  m_max(1),
	  m_val(m_min)
{	
	m_pFrameLine = xr_new<CUIFrameLineWnd>();	
	AttachChild(m_pFrameLine);	
	m_pFrameLine->SetAutoDelete(true);
	m_pFrameLine_d = xr_new<CUIFrameLineWnd>(); 
	m_pFrameLine_d->SetVisible(false);
	AttachChild(m_pFrameLine_d); 
	m_pFrameLine_d->SetAutoDelete(true);
	m_pSlider = xr_new<CUI3tButton>();			AttachChild(m_pSlider);		m_pSlider->SetAutoDelete(true);
}

CUITrackBar::~CUITrackBar(){

}

void CUITrackBar::Init(float x, float y, float width, float height){
	string128 buf;
	float item_height;
	float item_width;
	CUIWindow::Init(x, y, width, DEF_CONTROL_HEIGHT);


	item_height = CUITextureMaster::GetTextureHeight(strconcat(buf,FRAME_LINE_TEXTURE,"_b"));
	m_pFrameLine->Init(0, (height - item_height)/2, width, item_height);
	m_pFrameLine->InitTexture(FRAME_LINE_TEXTURE);
	m_pFrameLine_d->Init(0,(height - item_height)/2, width, item_height);
	m_pFrameLine_d->InitTexture(FRAME_LINE_TEXTURE_D);

	strconcat(buf,SLIDER_TEXTURE,"_e");
	item_width  = CUITextureMaster::GetTextureWidth(buf);
    item_height = CUITextureMaster::GetTextureHeight(buf);
	m_pSlider->Init(0, (height - item_height)/2, item_width, item_height);
	m_pSlider->InitTexture(SLIDER_TEXTURE);
}	

void CUITrackBar::OnMouse(float x, float y, EUIMessages mouse_action){
	CUIWindow::OnMouse(x,y, mouse_action);
}

void CUITrackBar::OnMouseDown(bool left_button /* = true */){
	if (left_button){
		float window_width = GetWidth();
		m_val = (m_max - m_min)*cursor_pos.x/window_width + m_min;
		UpdatePos();
	}
}

void CUITrackBar::SetCurrentValue(){
	GetOptFloatValue(m_val, m_min, m_max);
	UpdatePos();
}

void CUITrackBar::SaveValue(){
	SaveOptFloatValue(m_val);
}

void CUITrackBar::Enable(bool status){
	m_bIsEnabled = status;
	
	m_pFrameLine->SetVisible(status);
	m_pSlider->Enable(status);
	m_pFrameLine_d->SetVisible(!status);
}

void CUITrackBar::UpdatePos(){
#ifdef DEBUG
	R_ASSERT2(m_val >= m_min && m_val <= m_max, "CUITrackBar::UpdatePos() - m_val >= m_min && m_val <= m_max" );
#endif
	float window_width = GetWidth();
	float slider_width = m_pSlider->GetWidth();
	float free_space = window_width - slider_width;
	Fvector2 pos = m_pSlider->GetWndPos();
    pos.x = (m_val - m_min)*free_space/(m_max - m_min);
	m_pSlider->SetWndPos(pos);
	SaveValue();
}