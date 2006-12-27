#include "StdAfx.h"
#include "UIOptionsItem.h"
#include "UIOptionsManager.h"
#include "../../xr_ioconsole.h"

CUIOptionsManager CUIOptionsItem::m_optionsManager;

CUIOptionsItem::~CUIOptionsItem()
{
	m_optionsManager.UnRegisterItem(this);
}

void CUIOptionsItem::Register(const char* entry, const char* group)
{
	m_optionsManager.RegisterItem	(this, group);
	m_entry							= entry;	
}

void CUIOptionsItem::SendMessage2Group(const char* group, const char* message)
{
	m_optionsManager.SendMessage2Group(group,message);
}

void CUIOptionsItem::OnMessage(const char* message)
{
	// do nothing
}

LPCSTR CUIOptionsItem::GetOptStringValue()
{
	return Console->GetString(m_entry.c_str());
}

void CUIOptionsItem::SaveOptStringValue(const char* val)
{
	xr_string command	= m_entry;
	command				+= " ";
	command				+= val;
	Console->Execute	(command.c_str());
}

void CUIOptionsItem::BindAction2Key(const char* val)
{
	xr_string comm_unbind = "unbind ";
	comm_unbind += m_entry;
	Console->Execute(comm_unbind.c_str());

	if (val && xr_strlen(val))
	{
		xr_string comm_bind = "bind ";
		comm_bind += m_entry;
		comm_bind += " ";
		comm_bind += val;
		Console->Execute(comm_bind.c_str());
	}	
}

void CUIOptionsItem::GetOptIntegerValue(int& val, int& min, int& max)
{
	Console->GetInteger(m_entry.c_str(), val, min, max);
}

void CUIOptionsItem::SaveOptIntegerValue(int val)
{
	string512			command;
	sprintf				(command, "%s %d", m_entry.c_str(), val);
	Console->Execute	(command);
}

void CUIOptionsItem::GetOptFloatValue(float& val, float& min, float& max)
{
	Console->GetFloat(m_entry.c_str(), val, min, max);
}

void CUIOptionsItem::SaveOptFloatValue(float val)
{
	string512			command;
	sprintf				(command, "%s %f", m_entry.c_str(), val);
	Console->Execute	(command);
}

bool CUIOptionsItem::GetOptBoolValue()
{
	BOOL val;
	Console->GetBool(m_entry.c_str(), val);
	return val ? true : false;
}

void CUIOptionsItem::SaveOptBoolValue(bool val)
{
	string512			command;
	sprintf				(command, "%s %s", m_entry.c_str(), (val)?"on":"off");
	Console->Execute	(command);
}

char* CUIOptionsItem::GetOptTokenValue()
{
	return Console->GetToken(m_entry.c_str());
}

xr_token* CUIOptionsItem::GetOptToken()
{
	return Console->GetXRToken(m_entry.c_str());
}

void CUIOptionsItem::SaveOptTokenValue(const char* val){
	SaveOptStringValue(val);
}

void CUIOptionsItem::SaveValue(){
	if (	m_entry == "vid_mode"		|| 
			m_entry == "_preset"		|| 
			m_entry == "rs_fullscreen" 	||	
			m_entry == "rs_fullscreen"	||
			m_entry == "r__supersample"	|| 
			m_entry == "rs_refresh_60hz"||
			m_entry == "rs_no_v_sync"	||
			m_entry == "texture_lod")
	m_optionsManager.DoVidRestart();

	if (m_entry == "snd_freq" || m_entry == "snd_efx")
		m_optionsManager.DoSndRestart();
}
