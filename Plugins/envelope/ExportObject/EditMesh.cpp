//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMesh.h"
#include "EditObject.h"
#include "..\FS.h"

CEditMesh::~CEditMesh(){
	Clear();
}

void CEditMesh::Clear(){
	m_Points.clear 		();
    m_Adjs.clear		();
	m_Faces.clear		();
    m_VMaps.clear		();
    m_SurfFaces.clear	();
    m_VMRefs.clear		();
}

void CEditMesh::RecomputeBBox(){
	if( m_Points.empty() ){
		m_Box.set(0,0,0, 0,0,0);
		return;
    }
	m_Box.set( m_Points[0], m_Points[0] );
	for(FvectorIt pt=m_Points.begin()+1; pt!=m_Points.end(); pt++)
		m_Box.modify(*pt);
}
//----------------------------------------------------

void CEditMesh::FlipFaces(){
	for(FaceIt f = m_Faces.begin(); f!=m_Faces.end(); f++){
		st_FaceVert v = f->pv[0];
		f->pv[0] = f->pv[2];
		f->pv[2] = v;
	}
}
//----------------------------------------------------------------------------

st_VMap* CEditMesh::FindVMapByName(const char* name, EVMType t){
	for (VMapIt vm_it=m_VMaps.begin(); vm_it!=m_VMaps.end(); vm_it++){
		if ((vm_it->type==t)&&(stricmp(vm_it->name,name)==0)) return vm_it;
	}
	return 0;
}
