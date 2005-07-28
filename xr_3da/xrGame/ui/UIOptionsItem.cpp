////////////////////////////////////////////////////
//
///////////////////////////////////////////////////

#include "StdAfx.h"
#include "UIOptionsItem.h"
#include "UIOptionsManager.h"
#include "../MainUI.h"
#include "../../xr_ioconsole.h"

CUIOptionsItem::~CUIOptionsItem(){
	UI()->OptionsManager()->UnRegisterItem(this);
}

void CUIOptionsItem::Register(const char* entry, const char* group){
	UI()->OptionsManager()->RegisterItem(this, group);
	m_entry = entry;	
}

void CUIOptionsItem::SendMessage2Group(const char* group, const char* message){
	UI()->OptionsManager()->SendMessage2Group(group,message);
}

void CUIOptionsItem::OnMessage(const char* group, const char* message){
	// do nothing
}

LPCSTR CUIOptionsItem::GetOptStringValue(){
	return Console->GetString(m_entry.c_str());
}

void CUIOptionsItem::SaveOptStringValue(const char* val){
	xr_string command = m_entry;
	command += " ";
	command += val;
	Console->Execute(command.c_str());
}

void CUIOptionsItem::BindAction2Key(const char* val){
	if (!val || 0 == xr_strlen(val))
		return;
	xr_string comm_bind = "bind ";
	xr_string comm_unbind = "unbind ";
	comm_bind += m_entry;
	comm_unbind += m_entry;
	comm_bind += " ";
	comm_bind += val;
	Console->Execute(comm_unbind.c_str());
	Console->Execute(comm_bind.c_str());
}

void CUIOptionsItem::GetOptIntegerValue(int& val, int& min, int& max){
	Console->GetInteger(m_entry.c_str(), val, min, max);
}

void CUIOptionsItem::SaveOptIntegerValue(int val){
	char buf[16];
	xr_string command = m_entry;
	command += " ";
	command += itoa(val, buf, 10);
	Console->Execute(command.c_str());
}

void CUIOptionsItem::GetOptFloatValue(float& val, float& min, float& max){
	Console->GetFloat(m_entry.c_str(), val, min, max);
}

void CUIOptionsItem::SaveOptFloatValue(float val){
	char buf[64];
	xr_string command = m_entry;
	command += " ";
	sprintf(buf,"%f",val);
	command += buf;
	Console->Execute(command.c_str());
}

bool CUIOptionsItem::GetOptBoolValue(){
	BOOL val;
	Console->GetBool(m_entry.c_str(), val);
	return val ? true : false;
}

void CUIOptionsItem::SaveOptBoolValue(bool val){
	char buf[16];
	xr_string command = m_entry;
	command += " ";
	command += itoa(val, buf, 10);
	Console->Execute(command.c_str());
}

char* CUIOptionsItem::GetOptTokenValue(){
	return Console->GetToken(m_entry.c_str());
}

xr_token* CUIOptionsItem::GetOptToken(){
	return Console->GetXRToken(m_entry.c_str());
}


void CUIOptionsItem::SaveOptTokenValue(const char* val){
	SaveOptStringValue(val);
}

