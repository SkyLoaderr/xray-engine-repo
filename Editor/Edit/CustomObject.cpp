//----------------------------------------------------
// file: CustomObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "CustomObject.h"
#include "ui_main.h"
#include "scene.h"

#define SCENEOBJECT_CHUNK_PARAMS 		0xF900
#define SCENEOBJECT_CHUNK_GROUPINDEX	0xF901
#define SCENEOBJECT_CHUNK_LOCK	 		0xF902
//----------------------------------------------------

CCustomObject::~CCustomObject(){
	OnDestroy();
}

void CCustomObject::Select( BOOL flag ){
    if (m_Visible&&(m_Selected!=flag)){
        m_Selected = flag;
        UI.RedrawScene();
//		Scene->UndoSave();
    }
};

void CCustomObject::Show( BOOL flag ){
    m_Visible = flag;
    if (!m_Visible) m_Selected = false;
    UI.RedrawScene();
};

void CCustomObject::Lock( BOOL flag ){
	m_Locked = flag;
};

void CCustomObject::OnShowHint(AStringVec& dest){
    dest.push_back(AnsiString("Class: ")+AnsiString(GetNameByClassID(m_ClassID)));
    dest.push_back(AnsiString("Name:  ")+AnsiString(m_Name));
    dest.push_back(AnsiString("-------"));
}

bool CCustomObject::Load(CStream& F){
    R_ASSERT(F.FindChunk(SCENEOBJECT_CHUNK_PARAMS));
	m_Selected 		= F.Rword();
	m_Visible  		= F.Rword();
	F.RstringZ		(m_Name);

    if(F.FindChunk(SCENEOBJECT_CHUNK_GROUPINDEX))
		m_GroupIndex= F.Rdword();

	if(F.FindChunk(SCENEOBJECT_CHUNK_LOCK))
		m_Locked	= F.Rword();

	return true;
}
void CCustomObject::Save(CFS_Base& F){
	F.open_chunk	(SCENEOBJECT_CHUNK_PARAMS);
	F.Wword			(m_Selected);
	F.Wword			(m_Visible);
	F.WstringZ		(m_Name );
	F.close_chunk	();

	F.open_chunk	(SCENEOBJECT_CHUNK_GROUPINDEX);
	F.Wdword		(m_GroupIndex);
	F.close_chunk	();

	F.open_chunk	(SCENEOBJECT_CHUNK_LOCK);
	F.Wword			(m_Locked);
	F.close_chunk	();
}
//----------------------------------------------------

