#include "StdAfx.h"
#include "UIProgressShape.h"

#include "UIStatic.h"

CUIProgressShape::CUIProgressShape(){
	m_pTexture = NULL;
	m_pBackground = NULL;
	m_bText = false;
};

CUIProgressShape::~CUIProgressShape(){
	xr_delete(m_pTexture);
	xr_delete(m_pBackground);
}	

CUIStatic* CUIProgressShape::InitTexture(LPCSTR texture){
	m_pTexture = xr_new<CUIStatic>();
	AttachChild(m_pTexture);	
	m_pTexture->SetStretchTexture(true);

	if (NULL == texture)
		return m_pTexture;
	
	m_pTexture->Init(0,0,GetWidth(),GetHeight());	
	m_pTexture->InitTextureEx(texture, "hud\\seta");
	m_pTexture->SetColor(0xff7f7f7f);

	return m_pTexture;
}

CUIStatic* CUIProgressShape::InitBackground(LPCSTR texture){
	m_pBackground = xr_new<CUIStatic>();
	AttachChild(m_pBackground);
	m_pBackground->SetStretchTexture(true);

	if (NULL == texture)
		return m_pBackground;

	m_pBackground->Init(0,0,GetWidth(),GetHeight());
	m_pBackground->InitTexture(texture);//,"hud\\seta");

	return m_pBackground;
}

void CUIProgressShape::SetPos(float pos){
    m_pTexture->GetStaticItem()->SetAlphaRef(iFloor(253.0f*pos) + 2);	
}

void CUIProgressShape::SetPos(int pos, int max){
    m_pTexture->GetStaticItem()->SetAlphaRef(iFloor(253.0f*float(pos)/float(max)) + 2);	
	if (m_bText)
	{
		string256 _buff;
		m_pTexture->SetText(itoa(pos,_buff,10));
	}
}

void CUIProgressShape::SetTextVisible(bool b){
	m_bText = b;
}