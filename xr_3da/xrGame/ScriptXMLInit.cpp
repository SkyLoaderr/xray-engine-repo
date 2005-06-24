
#include "StdAfx.h"
#include "ScriptXmlInit.h"
#include "ui\UIXmlInit.h"
#include "ui\UITextureMaster.h"
#include "ui\UICheckButton.h" //#include "ui\UI3tButton.h"
#include "ui\UISpinNum.h"
#include "ui\UISpinText.h"
#include "ui\UIListWnd.h"
#include "ui\UITabControl.h"
#include "ui\UIFrameWindow.h"
#include "ui\UILabel.h"
#include "ui\ServerList.h"
#include "ui\UIMapList.h"
#include "ui\UIEditBox.h"

#include "script_space.h"
using namespace luabind;

CScriptXmlInit::CScriptXmlInit(){

}

CScriptXmlInit::CScriptXmlInit(const CScriptXmlInit&){
	// do nothing
}

CScriptXmlInit& CScriptXmlInit::operator =(const CScriptXmlInit& other){
	// do nothing
	return (*this);
}

void CScriptXmlInit::ParseFile(LPCSTR xml_file){
	m_xml.Init(CONFIG_PATH, UI_PATH, xml_file);
}

void CScriptXmlInit::InitWindow(LPCSTR path, int index, CUIWindow* pWnd){
	CUIXmlInit::InitWindow(m_xml, path, index, pWnd);
}

void CScriptXmlInit::InitFrame(LPCSTR path, int index, CUIFrameWindow* pWnd){
	CUIXmlInit::InitFrameWindow(m_xml, path, index, pWnd);
}

void CScriptXmlInit::InitFrameLine(LPCSTR path, int index, CUIFrameLineWnd* pWnd){
	CUIXmlInit::InitFrameLine(m_xml, path, index, pWnd);
}

void CScriptXmlInit::InitLabel(LPCSTR path, int index, CUILabel* pWnd){
	CUIXmlInit::InitLabel(m_xml, path, index, pWnd);
}

void CScriptXmlInit::InitEditBox(LPCSTR path, int index, CUIEditBox* pWnd){
	CUIXmlInit::InitEditBox(m_xml, path, index, pWnd);
}

void CScriptXmlInit::InitStatic(LPCSTR path, int index, CUIStatic* pWnd){
	CUIXmlInit::InitStatic(m_xml, path, index, pWnd);
}

void CScriptXmlInit::InitCheck(LPCSTR path, int index, CUICheckButton* pWnd){
	CUIXmlInit::InitCheck(m_xml, path, index, pWnd);
}

void CScriptXmlInit::InitSpinNum(LPCSTR path, int index, CUISpinNum* pWnd){
	CUIXmlInit::InitSpin(m_xml, path, index, pWnd);
}

void CScriptXmlInit::InitSpinText(LPCSTR path, int index, CUISpinText *pWnd){
	CUIXmlInit::InitSpin(m_xml, path, index, pWnd);
}


void CScriptXmlInit::InitButton(LPCSTR path, int index, CUIButton* pWnd){
	CUIXmlInit::InitButton(m_xml, path, index, pWnd);
}


void CScriptXmlInit::Init3tButton(LPCSTR path, int index, CUI3tButton* pWnd){
	CUIXmlInit::Init3tButton(m_xml, path, index, pWnd);
}

void CScriptXmlInit::InitList(LPCSTR path, int index, CUIListWnd* pWnd){
	CUIXmlInit::InitListWnd(m_xml, path, index, pWnd);
}


void CScriptXmlInit::InitTab(LPCSTR path, int index, CUITabControl* pWnd){
	CUIXmlInit::InitTabControl(m_xml, path, index, pWnd);
}

void CScriptXmlInit::ParseShTexInfo(LPCSTR xml_file){
	CUITextureMaster::ParseShTexInfo(xml_file);
}

void CScriptXmlInit::FreeShTexInfo(){
	CUITextureMaster::FreeShTexInfo();
}

void CScriptXmlInit::InitServerList(LPCSTR path, CServerList* pWnd){
	pWnd->InitFromXml(m_xml, path);
}

void CScriptXmlInit::InitMapList(LPCSTR path, CUIMapList* pWnd){
	pWnd->InitFromXml(m_xml, path);
}


void CScriptXmlInit::script_register(lua_State *L){
	module(L)
	[
		class_<CScriptXmlInit>("CScriptXmlInit")
		.def(					constructor<>())
		.def("ParseFile",		&CScriptXmlInit::ParseFile)
		.def("ParseShTexInfo",	&CScriptXmlInit::ParseShTexInfo)
		.def("FreeShTexInfo",	&CScriptXmlInit::FreeShTexInfo)
		.def("InitWindow",		&CScriptXmlInit::InitWindow)
		.def("InitFrame",		&CScriptXmlInit::InitFrame)
		.def("InitFrameLine",	&CScriptXmlInit::InitFrameLine)
		.def("InitLabel",		&CScriptXmlInit::InitLabel)
		.def("InitEditBox",		&CScriptXmlInit::InitEditBox)		
		.def("InitStatic",		&CScriptXmlInit::InitStatic)
		.def("InitCheck",		&CScriptXmlInit::InitCheck)
		.def("InitSpinNum",		&CScriptXmlInit::InitSpinNum)
		.def("InitSpinText",	&CScriptXmlInit::InitSpinText)
		.def("InitButton",		&CScriptXmlInit::InitButton)
		.def("Init3tButton",	&CScriptXmlInit::Init3tButton)
		.def("InitList",		&CScriptXmlInit::InitList)
		.def("InitTab",			&CScriptXmlInit::InitTab)
		.def("InitServerList",	&CScriptXmlInit::InitServerList)
		.def("InitMapList",	&CScriptXmlInit::InitMapList)

	];

}
