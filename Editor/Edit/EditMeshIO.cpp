//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMesh.h"
#include "EditObject.h"
#include "FileSystem.h"

#define EMESH_CURRENT_VERSION	      	0x0011
//----------------------------------------------------
#define EMESH_CHUNK_VERSION	        	0x1000
#define EMESH_CHUNK_MESHNAME        	0x1001
#define EMESH_CHUNK_VISIBLE	        	0x1002
#define EMESH_CHUNK_LOCKED	        	0x1003
#define EMESH_CHUNK_BBOX	        	0x1004
#define EMESH_CHUNK_VERTS	        	0x1005
#define EMESH_CHUNK_FACES	        	0x1006
#define EMESH_CHUNK_VMAPS_0	        	0x1007
#define EMESH_CHUNK_VMREFS	        	0x1008
#define EMESH_CHUNK_SFACE				0x1009
#define EMESH_CHUNK_BOP					0x1010
#define EMESH_CHUNK_VMAPS_1		       	0x1011

void CEditMesh::SaveMesh(CFS_Base& F){
	F.open_chunk	(EMESH_CHUNK_VERSION);
	F.Wword       	(EMESH_CURRENT_VERSION); 								
	F.close_chunk  	();
    
	F.open_chunk	(EMESH_CHUNK_MESHNAME);
    F.WstringZ		(m_Name);				    							
	F.close_chunk   ();

	F.write_chunk	(EMESH_CHUNK_BBOX,&m_Box, sizeof(m_Box));
	F.write_chunk	(EMESH_CHUNK_VISIBLE,&m_Visible,1);						
	F.write_chunk	(EMESH_CHUNK_LOCKED,&m_Locked,1);						
	F.write_chunk	(EMESH_CHUNK_BOP,&m_Ops, sizeof(m_Ops));

	F.open_chunk	(EMESH_CHUNK_VERTS);
	F.Wdword		(m_Points.size());										
    F.write			(m_Points.begin(), m_Points.size()*sizeof(Fvector));	
    for (AdjIt a_it=m_Adjs.begin(); a_it!=m_Adjs.end(); a_it++){
    	int sz 		= a_it->size(); VERIFY(sz<=255);						
		F.Wbyte		(sz);                                                   
        F.write		(a_it->begin(), sizeof(int)*sz);						
    }
	F.close_chunk     ();

	F.open_chunk	(EMESH_CHUNK_FACES);
	F.Wdword		(m_Faces.size()); 		/* polygon count */				
    F.write			(m_Faces.begin(), m_Faces.size()*sizeof(st_Face));		
	F.close_chunk  	();

	F.open_chunk	(EMESH_CHUNK_VMREFS);
	F.Wdword		(m_VMRefs.size());                                      
    for (VMRefsIt r_it=m_VMRefs.begin(); r_it!=m_VMRefs.end(); r_it++){
    	int sz 		= r_it->size(); VERIFY(sz<=255);
		F.Wbyte		(sz);													
        F.write		(r_it->begin(), sizeof(st_VMapPt)*sz);					
    }
	F.close_chunk	();
    
	F.open_chunk	(EMESH_CHUNK_SFACE);
	F.Wword			(m_SurfFaces.size()); 	/* surface polygon count*/		
	for (SurfFacesPairIt plp_it=m_SurfFaces.begin(); plp_it!=m_SurfFaces.end(); plp_it++){
    	F.WstringZ	(plp_it->first->name); 	/* surface name*/				
    	INTVec& 	pol_lst = plp_it->second;
        F.Wdword	(pol_lst.size());		/* surface-polygon indices*/	
        F.write		(pol_lst.begin(), sizeof(int)*pol_lst.size());	
    }
	F.close_chunk     ();

	F.open_chunk	(EMESH_CHUNK_VMAPS_1);
	F.Wdword		(m_VMaps.size());
    for (VMapIt vm_it=m_VMaps.begin(); vm_it!=m_VMaps.end(); vm_it++){
        F.WstringZ	(vm_it->name);							
        F.Wbyte		(vm_it->dim);
        F.Wbyte		(vm_it->type);
        F.Wdword	(vm_it->size());
        F.write		(vm_it->getdata(), vm_it->datasize() );
    }
	F.close_chunk  	();
}

bool CEditMesh::LoadMesh(CStream& F){
    DWORD version=0;

    R_ASSERT(F.ReadChunk(EMESH_CHUNK_VERSION,&version));
    if (version!=EMESH_CURRENT_VERSION){
        Log->DlgMsg( mtError, "CEditMesh: unsuported file version. Mesh can't load.");
        return false;
    }

    R_ASSERT(F.FindChunk(EMESH_CHUNK_MESHNAME));
	F.RstringZ		(m_Name);

    R_ASSERT(F.ReadChunk(EMESH_CHUNK_BBOX,&m_Box));
    R_ASSERT(F.ReadChunk(EMESH_CHUNK_VISIBLE,&m_Visible));
    R_ASSERT(F.ReadChunk(EMESH_CHUNK_LOCKED,&m_Locked));
    F.ReadChunk(EMESH_CHUNK_BOP,&m_Ops);

    R_ASSERT(F.FindChunk(EMESH_CHUNK_VERTS));
	DWORD cnt			= F.Rdword();
    m_Points.resize		(cnt);
    m_Adjs.resize		(cnt);
	F.Read				(m_Points.begin(), cnt*sizeof(Fvector));
    for (AdjIt a_it=m_Adjs.begin(); a_it!=m_Adjs.end(); a_it++){
        cnt				= F.Rbyte();
        a_it->resize	(cnt);
        F.Read			(a_it->begin(), sizeof(int)*cnt);
    }

    R_ASSERT(F.FindChunk(EMESH_CHUNK_FACES));
	m_Faces.resize		(F.Rdword());
	F.Read				(m_Faces.begin(), m_Faces.size()*sizeof(st_Face));

    R_ASSERT(F.FindChunk(EMESH_CHUNK_VMREFS));
    m_VMRefs.resize		(F.Rdword());
    int sz_vmpt			= sizeof(st_VMapPt);
    for (VMRefsIt r_it=m_VMRefs.begin(); r_it!=m_VMRefs.end(); r_it++){
        r_it->resize	(F.Rbyte());
        F.Read			(r_it->begin(), sz_vmpt*r_it->size());
    }

    R_ASSERT(F.FindChunk(EMESH_CHUNK_SFACE));
    char surf_name[MAX_OBJ_NAME];
    DWORD sface_cnt		= F.Rword(); // surface-face count
    for (DWORD sp_i=0; sp_i<sface_cnt; sp_i++){
        F.RstringZ		(surf_name);
        int surf_id;
        st_Surface* 	surf= m_Parent->FindSurfaceByName(surf_name, &surf_id); VERIFY(surf);
        INTVec&			face_lst = m_SurfFaces[surf];
        face_lst.resize	(F.Rdword());
        F.Read			(face_lst.begin(), face_lst.size()*sizeof(int));
    }

    if(F.FindChunk(EMESH_CHUNK_VMAPS_1)){
        m_VMaps.resize	(F.Rdword());
        for (VMapIt vm_it=m_VMaps.begin(); vm_it!=m_VMaps.end(); vm_it++){
            F.RstringZ	(vm_it->name);							
			vm_it->dim 	= F.Rbyte();
			vm_it->type	= EVMType(F.Rbyte());
			vm_it->resize(F.Rdword());
            F.Read		(vm_it->getdata(), vm_it->datasize() );
        }
    }else{
	    R_ASSERT(F.FindChunk(EMESH_CHUNK_VMAPS_0));
        m_VMaps.resize	(F.Rdword());
        for (VMapIt vm_it=m_VMaps.begin(); vm_it!=m_VMaps.end(); vm_it++){
            F.RstringZ	(vm_it->name);							
			vm_it->dim 	= 2;
			vm_it->type	= vmtUV;
			vm_it->resize(F.Rdword());
            F.Read		(vm_it->getdata(), vm_it->datasize() );
        }
    }

	return true;
}
//----------------------------------------------------
 
