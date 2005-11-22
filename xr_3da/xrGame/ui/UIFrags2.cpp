#include "StdAfx.h"

#include "UIFrags2.h"
#include "UIStats.h"
#include "xrXmlParser.h"
#include "UIXmlInit.h"
#include "UIStatic.h"

CUIFrags2::CUIFrags2(){	
	m_pStats2 = xr_new<CUIStats>();  AttachChild(m_pStats2);
}

CUIFrags2::~CUIFrags2(){
	xr_delete(m_pStats2);
}


void CUIFrags2::Init(CUIXml& xml_doc, LPCSTR path){
	InitBackground(xml_doc, path);

	m_pStats->Init(xml_doc, "stats_wnd", 1);
	m_pStats2->Init(xml_doc, "stats_wnd", 2);

    //
	float x = xml_doc.ReadAttribFlt("stats_wnd", 0, "x2");
	R_ASSERT(x);
	Fvector2 pos = m_pStats2->GetWndPos();
	pos.x = x;
	pos.y += 3;
	m_pStats2->SetWndPos(pos);

	pos = m_pStats->GetWndPos();
	pos.y += 3;
	m_pStats->SetWndPos(pos);

}