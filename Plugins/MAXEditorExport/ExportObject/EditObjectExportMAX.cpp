//----------------------------------------------------
// file: CEditableObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"

CSurface*	CEditableObject::CreateSurface(Mtl* M, DWORD m_id){
	for (SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
		if (((*s_it)->mat_id==m_id)&&((*s_it)->pMtlMain==M)) return *s_it;
	CSurface* S		= new CSurface();
	S->mat_id		= m_id;
	S->pMtlMain		= M;
	m_Surfaces.push_back(S);
	return S;
}

LPCSTR CEditableObject::GenerateSurfaceName(const char* base_name){
	static char nm[128];
	strcpy(nm, base_name);
	if (FindSurfaceByName(nm)){
		DWORD idx=0;
		do{
			sprintf(nm,"%s_%d",base_name,idx);
			idx++;
		}while(FindSurfaceByName(nm));
	}
	return nm;
}
/*
void CEditableObject::SaveObject(const char* fname){
    CFS_Memory F;
    F.open_chunk(CHUNK_OBJECT_BODY);
    Save(F);
    F.close_chunk();
    F.SaveTo(fname,0);//"OBJECT");
}

void CEditableObject::Save(CFS_Base& F){
	F.open_chunk	(SCENEOBJECT_CHUNK_PARAMS);
	F.Wword			(m_Selected);
	F.Wword			(m_Visible);
	F.WstringZ		(m_Name );
	F.close_chunk	();

	F.open_chunk	(EOBJ_CHUNK_VERSION);
	F.Wword			(EOBJ_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(EOBJ_CHUNK_PLACEMENT);
    F.Wvector		(vPosition);
    F.Wvector		(vRotate);
    F.Wvector		(vScale);
	F.close_chunk	();

    F.write_chunk	(EOBJ_CHUNK_FLAG,&m_DynamicObject,1);

    if( IsReference() ){
    	// reference object version
        F.open_chunk	(EOBJ_CHUNK_REFERENCE);
        F.write			(&m_LibRef->m_LibRefVer,m_LibRef->m_LibRefVer.size());
        F.WstringZ		(m_LibRef->m_Name);
        F.close_chunk	();
    } else {
    	// object version
        F.write_chunk	(EOBJ_CHUNK_LIB_VERSION,&m_LibRefVer,m_LibRefVer.size());
        // surfaces
        F.open_chunk	(EOBJ_CHUNK_SURFACES);
        F.Wdword		(m_Surfaces.size());
        for (SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
            F.WstringZ	((*s_it)->name);
            F.WstringZ	(!(*s_it)->shader.empty()?(*s_it)->shader.c_str():"");
            F.Wbyte		((*s_it)->sideflag);
            F.Wdword	((*s_it)->dwFVF);
            F.Wdword	((*s_it)->textures.size());
            for (AStringIt n_it=(*s_it)->textures.begin(); n_it!=(*s_it)->textures.end(); n_it++)
                F.WstringZ(n_it->c_str());
            for (AStringIt v_it=(*s_it)->vmaps.begin(); v_it!=(*s_it)->vmaps.end(); v_it++)
                F.WstringZ(v_it->c_str());
        }
        F.close_chunk	();

        // meshes
        F.open_chunk	(EOBJ_CHUNK_EDITMESHES);
        int count = 0;
        for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++){
			F.open_chunk(count); count++;
            (*m)->SaveMesh(F);
			F.close_chunk();
        }
		F.close_chunk	();
    }
} 
*/