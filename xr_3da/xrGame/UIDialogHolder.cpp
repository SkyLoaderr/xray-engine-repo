#include "stdafx.h"
#include "UIDialogHolder.h"
#include "ui\UIDialogWnd.h"
#include "MainUI.h"
#include "UICursor.h"

//----------CDialogHolder--------------
CDialogHolder::CDialogHolder()
{
	shedule.t_min			= 5;
	shedule.t_max			= 20;
	shedule_register();
}

CDialogHolder::~CDialogHolder()
{
	shedule_unregister();
}

void CDialogHolder::StartMenu (CUIDialogWnd* pDialog)
{
	R_ASSERT( !pDialog->IsShown() );

	AddDialogToRender(pDialog);
	SetMainInputReceiver(pDialog);
	pDialog->SetHolder(this);
	pDialog->Show();

	GetUICursor()->Show();
}


void CDialogHolder::StopMenu (CUIDialogWnd* pDialog)
{
	R_ASSERT( pDialog->IsShown() );
	R_ASSERT( MainInputReceiver()==pDialog );

	RemoveDialogToRender(pDialog);
	SetMainInputReceiver(NULL);
	pDialog->SetHolder(NULL);
	pDialog->Hide();

	if(!MainInputReceiver() )
		GetUICursor()->Hide();
}

void CDialogHolder::AddDialogToRender(CUIWindow* pDialog)
{
	if(std::find(m_dialogsToRender.begin(), m_dialogsToRender.end(), pDialog) == m_dialogsToRender.end() )
	{
		m_dialogsToRender.push_back(pDialog);
		pDialog->Show(true);
	}
}

void CDialogHolder::RemoveDialogToRender(CUIWindow* pDialog)
{
	xr_vector<CUIWindow*>::iterator it = std::find(m_dialogsToRender.begin(),m_dialogsToRender.end(),pDialog);
	if(it != m_dialogsToRender.end())
	{
		(*it)->Show(false);
		(*it)->Enable(false);
		m_dialogsToRender.erase(it);
		m_dialogsToErase.push_back(*it);
	}
}

void CDialogHolder::DoRenderDialogs()
{
	xr_vector<CUIWindow*>::iterator it = m_dialogsToRender.begin();
	for(; it!=m_dialogsToRender.end();++it)
		if((*it)->IsShown())
			(*it)->Draw();

}
CUIDialogWnd* CDialogHolder::MainInputReceiver()
{ 
	if ( !m_input_receivers.empty() ) 
		return m_input_receivers.top(); 
	return NULL; 
};

void CDialogHolder::SetMainInputReceiver	(CUIDialogWnd* ir)	
{ 
	if( MainInputReceiver() == ir ) return;
	if(!ir){//remove
		if(!m_input_receivers.empty())
			m_input_receivers.pop();
	}else{
		m_input_receivers.push(ir);
	}
};

void CDialogHolder::StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	if( pDialog->IsShown() )
		StopMenu(pDialog);
	else
		StartMenu(pDialog);

	xr_vector<CUIWindow*>::iterator it = std::find(m_dialogsToErase.begin(), m_dialogsToErase.end(), pDialog);
	if (m_dialogsToErase.end() != it)
		m_dialogsToErase.erase(it);

}

void CDialogHolder::shedule_Update(u32 dt)
{
	ISheduled::shedule_Update(dt);
	xr_vector<CUIWindow*>::iterator it = m_dialogsToRender.begin();
	for(; it!=m_dialogsToRender.end();++it)
		if((*it)->IsEnabled())
			(*it)->Update();


	for(it = m_dialogsToErase.begin(); it!=m_dialogsToErase.end(); ++it)
	{
		xr_vector<CUIWindow*>::iterator it_find = std::find(m_dialogsToRender.begin(),
			m_dialogsToRender.end(), *it);
		if (it_find != m_dialogsToRender.end())
		{
			m_dialogsToRender.erase(it_find);
		}
	}
	m_dialogsToErase.clear();

}
float CDialogHolder::shedule_Scale()
{
	return 0.5f;
}

void CDialogHolder::CleanInternals()
{
	while( !m_input_receivers.empty() )
		m_input_receivers.pop();

	m_dialogsToRender.clear();
	m_dialogsToErase.clear();
	GetUICursor()->Hide();
}
