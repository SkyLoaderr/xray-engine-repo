//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------
                
#include "stdafx.h"
#pragma hdrstop

#include "EditMesh.h"
#include "EditObject.h"

#define EMESH_CURRENT_VERSION	      	0x0011
//----------------------------------------------------
#define EMESH_CHUNK_VERSION	        	0x1000
#define EMESH_CHUNK_MESHNAME        	0x1001
#define EMESH_CHUNK_FLAGS	        	0x1002
#define EMESH_CHUNK_NOT_USED_0        	0x1003
#define EMESH_CHUNK_BBOX	        	0x1004
#define EMESH_CHUNK_VERTS	        	0x1005
#define EMESH_CHUNK_FACES	        	0x1006
#define EMESH_CHUNK_VMAPS_0	        	0x1007
#define EMESH_CHUNK_VMREFS	        	0x1008
#define EMESH_CHUNK_SFACE				0x1009
#define EMESH_CHUNK_BOP					0x1010
#define EMESH_CHUNK_VMAPS_1		       	0x1011
#define EMESH_CHUNK_VMAPS_2		       	0x1012
#define EMESH_CHUNK_SG			       	0x1013

void CEditableMesh::SaveMesh(IWriter& F){
	F.open_chunk	(EMESH_CHUNK_VERSION);
	F.w_u16       	(EMESH_CURRENT_VERSION);
	F.close_chunk  	();

	F.open_chunk	(EMESH_CHUNK_MESHNAME);
    F.w_stringZ		(m_Name);
	F.close_chunk   ();

	F.w_chunk		(EMESH_CHUNK_BBOX,&m_Box, sizeof(m_Box));
	F.w_chunk		(EMESH_CHUNK_FLAGS,&m_Flags,1);
	F.w_chunk		(EMESH_CHUNK_BOP,&m_Ops, sizeof(m_Ops));

	F.open_chunk	(EMESH_CHUNK_VERTS);
	F.w_u32			(m_Points.size());
    F.w				(&*m_Points.begin(), m_Points.size()*sizeof(Fvector));
    for (AdjIt a_it=m_Adjs.begin(); a_it!=m_Adjs.end(); a_it++){
    	int sz 		= a_it->size(); VERIFY(sz<=255);
		F.w_u8		(sz);
        F.w			(&*a_it->begin(), sizeof(int)*sz);
    }
	F.close_chunk     ();

	F.open_chunk	(EMESH_CHUNK_FACES);
	F.w_u32			(m_Faces.size()); 		/* polygon count */
    F.w				(&*m_Faces.begin(), m_Faces.size()*sizeof(st_Face));
	F.close_chunk  	();

    if (!m_SGs.empty()){
    	R_ASSERT		(m_SGs.size()==m_Faces.size());
        F.open_chunk	(EMESH_CHUNK_SG);
        F.w				(&*m_SGs.begin(), m_SGs.size()*sizeof(u32));
        F.close_chunk  	();
    }

	F.open_chunk	(EMESH_CHUNK_VMREFS);
	F.w_u32			(m_VMRefs.size());
    for (VMRefsIt r_it=m_VMRefs.begin(); r_it!=m_VMRefs.end(); r_it++){
    	int sz 		= r_it->size(); VERIFY(sz<=255);
		F.w_u8		(sz);
        F.w			(r_it->begin(), sizeof(st_VMapPt)*sz);
    }
	F.close_chunk	();

	F.open_chunk	(EMESH_CHUNK_SFACE);
	F.w_u16			(m_SurfFaces.size()); 	/* surface polygon count*/
	for (SurfFacesPairIt plp_it=m_SurfFaces.begin(); plp_it!=m_SurfFaces.end(); plp_it++){
    	F.w_stringZ	(plp_it->first->_Name()); 	/* surface name*/
    	IntVec& 	pol_lst = plp_it->second;
        F.w_u32		(pol_lst.size());		/* surface-polygon indices*/
        F.w			(&*pol_lst.begin(), sizeof(int)*pol_lst.size());
    }
	F.close_chunk     ();

	F.open_chunk	(EMESH_CHUNK_VMAPS_2);
	F.w_u32		(m_VMaps.size());
    for (VMapIt vm_it=m_VMaps.begin(); vm_it!=m_VMaps.end(); vm_it++){
        F.w_stringZ	((*vm_it)->name);
        F.w_u8		((*vm_it)->dim);
		F.w_u8		((*vm_it)->polymap);
        F.w_u8		((*vm_it)->type);
        F.w_u32		((*vm_it)->size());
        F.w			((*vm_it)->getVMdata(), (*vm_it)->VMdatasize());
        F.w			((*vm_it)->getVIdata(), (*vm_it)->VIdatasize());
		if ((*vm_it)->polymap)
	        F.w		((*vm_it)->getPIdata(), (*vm_it)->PIdatasize());
    }
	F.close_chunk  	();
}

bool CEditableMesh::LoadMesh(IReader& F){
    u32 version=0;

    R_ASSERT(F.r_chunk(EMESH_CHUNK_VERSION,&version));
    if (version!=EMESH_CURRENT_VERSION){
        ELog.DlgMsg( mtError, "CEditableMesh: unsuported file version. Mesh can't load.");
        return false;
    }

    R_ASSERT(F.find_chunk(EMESH_CHUNK_MESHNAME));
	F.r_stringZ		(m_Name);

    R_ASSERT(F.r_chunk(EMESH_CHUNK_BBOX,&m_Box));
    R_ASSERT(F.r_chunk(EMESH_CHUNK_FLAGS,&m_Flags));
    F.r_chunk(EMESH_CHUNK_BOP,&m_Ops);

    R_ASSERT(F.find_chunk(EMESH_CHUNK_VERTS));
	u32 cnt			= F.r_u32();
    if (cnt<3){
        ELog.Msg	(mtError, "CEditableMesh: Vertices<3.");
     	return false;
    }
    m_Points.resize		(cnt);
    m_Adjs.resize		(cnt);
	F.r					(&*m_Points.begin(), cnt*sizeof(Fvector));
    for (AdjIt a_it=m_Adjs.begin(); a_it!=m_Adjs.end(); a_it++){
        cnt				= F.r_u8();
        a_it->resize	(cnt);
        F.r				(&*a_it->begin(), sizeof(int)*cnt);
    }

    R_ASSERT(F.find_chunk(EMESH_CHUNK_FACES));
	m_Faces.resize		(F.r_u32());
    if (m_Faces.size()==0){
        ELog.Msg		(mtError, "CEditableMesh: Faces==0.");
     	return false;
    }
	F.r					(&*m_Faces.begin(), m_Faces.size()*sizeof(st_Face));

	m_SGs.resize		(m_Faces.size(),m_Flags.is(flSGMask)?0:-1);
	int sg_chunk_size	= F.find_chunk(EMESH_CHUNK_SG);
	if (sg_chunk_size){
		VERIFY			(m_SGs.size()*sizeof(u32)==sg_chunk_size);
		F.r				(&*m_SGs.begin(), m_SGs.size()*sizeof(u32));
	}

    R_ASSERT(F.find_chunk(EMESH_CHUNK_VMREFS));
    m_VMRefs.resize		(F.r_u32());
    int sz_vmpt			= sizeof(st_VMapPt);
    for (VMRefsIt r_it=m_VMRefs.begin(); r_it!=m_VMRefs.end(); r_it++){
        r_it->resize	(F.r_u8());
        F.r				(r_it->begin(), sz_vmpt*r_it->size());
    }

    R_ASSERT(F.find_chunk(EMESH_CHUNK_SFACE));
    string128 surf_name;
    u32 sface_cnt		= F.r_u16(); // surface-face count
    for (u32 sp_i=0; sp_i<sface_cnt; sp_i++){
        F.r_stringZ		(surf_name);
        int surf_id;
        CSurface* surf	= m_Parent->FindSurfaceByName(surf_name, &surf_id); VERIFY(surf);
        IntVec&			face_lst = m_SurfFaces[surf];
        face_lst.resize	(F.r_u32());
        if (face_lst.empty()){
	        ELog.Msg	(mtError, "Empty surface found: %s",surf->_Name());
    	 	return false;
        }
        F.r				(&*face_lst.begin(), face_lst.size()*sizeof(int));
    }

    if(F.find_chunk(EMESH_CHUNK_VMAPS_2)){
		m_VMaps.resize	(F.r_u32());
		for (VMapIt vm_it=m_VMaps.begin(); vm_it!=m_VMaps.end(); vm_it++){
			*vm_it		= xr_new<st_VMap>();
			F.r_stringZ	((*vm_it)->name);
			(*vm_it)->dim 	= F.r_u8();
			(*vm_it)->polymap=F.r_u8();
			(*vm_it)->type	= EVMType(F.r_u8());
			(*vm_it)->resize(F.r_u32());
			F.r			((*vm_it)->getVMdata(), (*vm_it)->VMdatasize());
			F.r			((*vm_it)->getVIdata(), (*vm_it)->VIdatasize());
			if ((*vm_it)->polymap)
				F.r		((*vm_it)->getPIdata(), (*vm_it)->PIdatasize());
		}
	}else{
		if(F.find_chunk(EMESH_CHUNK_VMAPS_1)){
			m_VMaps.resize	(F.r_u32());
			for (VMapIt vm_it=m_VMaps.begin(); vm_it!=m_VMaps.end(); vm_it++){
				*vm_it		= xr_new<st_VMap>();
				F.r_stringZ	((*vm_it)->name);
				(*vm_it)->dim 	= F.r_u8();
				(*vm_it)->type	= EVMType(F.r_u8());
				(*vm_it)->resize(F.r_u32());
				F.r			((*vm_it)->getVMdata(), (*vm_it)->VMdatasize() );
			}
		}else{
			R_ASSERT(F.find_chunk(EMESH_CHUNK_VMAPS_0));
			m_VMaps.resize	(F.r_u32());
			for (VMapIt vm_it=m_VMaps.begin(); vm_it!=m_VMaps.end(); vm_it++){
				*vm_it		= xr_new<st_VMap>();
				F.r_stringZ	((*vm_it)->name);
				(*vm_it)->dim 	= 2;
				(*vm_it)->type	= vmtUV;
				(*vm_it)->resize(F.r_u32());
				F.r			((*vm_it)->getVMdata(), (*vm_it)->VMdatasize() );
			}
		}
		// update vmaps
		RebuildVMaps();
	}

//	if (!m_CFModel) GenerateCFModel();
	return true;
}
//----------------------------------------------------

