//----------------------------------------------------
// file: TempObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "TempObject.h"
#include "FStaticRender.h"

CTempObject::CTempObject(){
	m_pVisual		= 0;
	m_iLifeTime		= int_max;
	m_bAutoRemove	= TRUE;
}
//----------------------------------------------------

CTempObject::~CTempObject(){
	Render.Models.Delete(m_pVisual);
}
//----------------------------------------------------

void CTempObject::Update(DWORD dt){
	m_iLifeTime		-= dt;
}
