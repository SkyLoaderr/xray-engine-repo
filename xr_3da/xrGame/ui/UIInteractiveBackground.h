// File:        UIInteractiveBackground.h
// Description: template class designed for UI controls to represent their state;
//              there are 4 states: Enabled, Disabled, Hightlighted and Touched.
//              As a rule you can use one of 3 background types:
//              Normal Texture, String Texture, Frame Texture (CUIStatic, CUIFrameLineWnd, CUIFrameWindow)
// Created:     29.12.2004
// Author:      Serhiy 0. Vynnychenko
// Mial:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//

#pragma once

#include "StdAfx.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"

enum UIState{
	S_Enabled,
	S_Disabled,
	S_Highlighted,
	S_Touched
};


template <class T>
class CUIInteractiveBackground : public CUIWindow
{
public:
	CUIInteractiveBackground();
	virtual ~CUIInteractiveBackground();

	virtual void Init(int x, int y, int width, int height);
	virtual void Init(LPCSTR texture_e, int x, int y, int width, int height);
	virtual void InitEnabledState(LPCSTR texture_e);
	virtual void InitDisabledState(LPCSTR texture_d);
	virtual void InitHighlightedState(LPCSTR texture_h);
	virtual void InitTouchedState(LPCSTR texture_t);
	virtual void SetState(UIState state);
	virtual void Draw();

protected:
	T* m_stateCurrent;
	T* m_stateEnabled;
	T* m_stateDisabled;
	T* m_stateHighlighted;
	T* m_stateTouched;
};

template <class T>
CUIInteractiveBackground<T>::CUIInteractiveBackground(){
	this->m_stateCurrent     = NULL;
	this->m_stateEnabled     = NULL;
	this->m_stateDisabled    = NULL;
	this->m_stateHighlighted = NULL;
	this->m_stateTouched     = NULL;	
}

template <class T>
CUIInteractiveBackground<T>::~CUIInteractiveBackground(){

}

template <class T>
void CUIInteractiveBackground<T>::Init(int x, int y, int width, int height){
	CUIWindow::Init(x, y, width, height);
}

template <class T>
void CUIInteractiveBackground<T>::Init(LPCSTR texture_e, int x, int y, int width, int height){
	CUIWindow::Init(x, y, width, height);

	InitEnabledState(texture_e);

	this->m_stateEnabled->Init(texture_e, 0, 0, width, height);
	this->m_stateCurrent = this->m_stateEnabled;
}

template <class T>
void CUIInteractiveBackground<T>::InitEnabledState(LPCSTR texture_e){
	RECT r = GetWndRect();

	if (!m_stateEnabled)
	{
		m_stateEnabled = xr_new<T>();
		m_stateEnabled->SetAutoDelete(true);
		AttachChild(m_stateEnabled);
	}

	this->m_stateEnabled->Init(texture_e, 0, 0, r.right - r.left, r.bottom - r.top);

	SetState(S_Enabled);
}

template <class T>
void CUIInteractiveBackground<T>::InitDisabledState(LPCSTR texture_d){
	RECT r = GetWndRect();

	if (!m_stateDisabled)
	{
		m_stateDisabled = xr_new<T>();
		m_stateDisabled->SetAutoDelete(true);
		AttachChild(m_stateDisabled);
	}

	this->m_stateDisabled->Init(texture_d, 0, 0, r.right - r.left, r.bottom - r.top); 
}

template <class T>
void CUIInteractiveBackground<T>::InitHighlightedState(LPCSTR texture_h){
	RECT r = GetWndRect();
    
	if (!m_stateHighlighted)
	{
		m_stateHighlighted = xr_new<T>();
		m_stateHighlighted->SetAutoDelete(true);
		AttachChild(m_stateHighlighted);		
	}

	this->m_stateHighlighted->Init(texture_h, 0, 0, r.right - r.left, r.bottom - r.top); 
}

template <class T>
void CUIInteractiveBackground<T>::InitTouchedState(LPCSTR texture_d){
	RECT r = GetWndRect();

    if (!m_stateTouched)
	{
		m_stateTouched = xr_new<T>();
		m_stateTouched->SetAutoDelete(true);
		AttachChild(m_stateTouched);		
	}

	this->m_stateTouched->Init(texture_d, 0, 0, r.right - r.left, r.bottom - r.top); 
}

template <class T>
void CUIInteractiveBackground<T>::SetState(UIState state){
	switch (state)
	{
	case S_Enabled:
		this->m_stateCurrent = this->m_stateEnabled;
		break;
	case S_Disabled:
		this->m_stateCurrent = this->m_stateDisabled ? this->m_stateDisabled : this->m_stateEnabled;
		break;
	case S_Highlighted:
		this->m_stateCurrent = this->m_stateHighlighted ? this->m_stateHighlighted : this->m_stateEnabled;
		break;
	case S_Touched:
		this->m_stateCurrent = this->m_stateTouched ? this->m_stateTouched : this->m_stateEnabled;
	}
}

template <class T>
void CUIInteractiveBackground<T>::Draw(){
	if (m_stateCurrent)
        m_stateCurrent->Draw();
}

typedef CUIInteractiveBackground<CUIStatic> CUI_IB_Static;
typedef CUIInteractiveBackground<CUIFrameWindow> CUI_IB_FrameWindow;
typedef CUIInteractiveBackground<CUIFrameLineWnd> CUI_IB_FrameLineWnd;
