#include "stdafx.h"
#pragma hdrstop

#include "detailmodel.h"
#include "Library.h"
#include "EditObject.h"
#include "EditMesh.h"

//------------------------------------------------------------------------------
#define DETOBJ_CHUNK_VERSION		0x1000
#define DETOBJ_CHUNK_REFERENCE 		0x0101
#define DETOBJ_CHUNK_SCALE_LIMITS	0x0102
#define DETOBJ_CHUNK_DENSITY_FACTOR	0x0103
#define DETOBJ_CHUNK_FLAGS			0x0104

#define DETOBJ_VERSION 				0x0001
//------------------------------------------------------------------------------

CDetail::CDetail(){
	shader				= 0;
	flags				= 0;
	m_pRefs				= 0;
    m_bMarkDel			= false;
    s_min				= 0.5f;
    s_max         		= 2.f;
    m_fDensityFactor	= 1.f;
    m_sRefs				= "";
	vertices			= 0;
	number_vertices		= 0;
	indices				= 0;
	number_indices		= 0;
}

CDetail::~CDetail()
{
	Unload();
}

void CDetail::Unload()
{
	if (vertices)		{ xr_free(vertices);	vertices=0; }
	if (indices)		{ xr_free(indices);		indices=0;	}
    Lib.RemoveEditObject(m_pRefs);
}

LPCSTR CDetail::GetName	(){
    return m_pRefs?m_pRefs->GetName():m_sRefs.c_str();
}

void CDetail::OnDeviceCreate(){
	if (!m_pRefs)		return;
    CSurface* surf		= *m_pRefs->FirstSurface();
    VERIFY				(surf);
    VERIFY				(surf->_Shader());
    shader				= surf->_Shader();
}

void CDetail::OnDeviceDestroy(){
    shader 				= 0;
}

int CDetail::_AddVert(const Fvector& p, float u, float v)
{
	fvfVertexIn V(p,u,v);
    for (DWORD k=0; k<number_vertices; k++)
    	if (vertices[k].similar(V)) return k;
    number_vertices++;
//    if (1==number_vertices) vertices = (fvfVertexIn*)xr_malloc(number_vertices*sizeof(fvfVertexIn));
//    else					
    vertices = (fvfVertexIn*)xr_realloc(vertices,number_vertices*sizeof(fvfVertexIn));
    vertices[number_vertices-1] = V;
    return number_vertices-1;
}

bool CDetail::Update	(LPCSTR name){
	m_sRefs				= name;
    // update link
    CEditableObject* R	= Lib.CreateEditObject(name);
    if (!R){
        ELog.DlgMsg		(mtError, "CDetail: '%s' not found in library", name);
        return false;
    }
    if(R->SurfaceCount()!=1){
    	ELog.DlgMsg		(mtError,"Object must contain 1 material.");
	    Lib.RemoveEditObject(R);
    	return false;
    }
	if(R->MeshCount()==0){
    	ELog.DlgMsg		(mtError,"Object must contain 1 mesh.");
	    Lib.RemoveEditObject(R);
    	return false;
    }

    Lib.RemoveEditObject(m_pRefs);
    m_pRefs				= R;

    // create shader
    CSurface* surf		= *m_pRefs->FirstSurface();
    shader				= surf->_Shader();
    R_ASSERT			(shader);

    // fill geometry
    CEditableMesh* M	= *m_pRefs->FirstMesh();
	number_indices 		= M->GetFaceCount(false)*3;
	indices				= (WORD*)xr_malloc(number_indices*sizeof(WORD));

    // fill vertices
    bv_bb.invalidate();
    DWORD idx			= 0;
    for (FaceIt f_it=M->m_Faces.begin(); f_it!=M->m_Faces.end(); f_it++){
    	for (int k=0; k<3; k++,idx++){
        	WORD& i_it	= indices[idx];
        	st_Face& F 	= *f_it;
            Fvector& P  = M->m_Points[F.pv[k].pindex];
            st_VMapPt& vm=M->m_VMRefs[F.pv[k].vmref][0];
            Fvector2& uv= M->m_VMaps[vm.vmap_index]->getUV(vm.index);
        	i_it 		= _AddVert	(P,uv.x,uv.y);
	        bv_bb.modify(vertices[i_it].P);
        }
    }
	bv_bb.getsphere		(bv_sphere.P,bv_sphere.R);

    return true;
}

bool CDetail::Load(CStream& F){
	// check version
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_VERSION));
    DWORD version		= F.Rdword();
    if (version!=DETOBJ_VERSION){
    	ELog.Msg(mtError,"CDetail: unsupported version.");
        return false;
    }

	// references
	char buf[255];
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_REFERENCE));
    F.RstringZ			(buf);

    // scale
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_SCALE_LIMITS));
    s_min				= F.Rfloat(); if (fis_zero(s_min)) 	s_min = 0.1f;
	s_max		        = F.Rfloat(); if (s_max<s_min)		s_max = s_min;

	// density factor
    if (F.FindChunk(DETOBJ_CHUNK_DENSITY_FACTOR))
	    m_fDensityFactor= F.Rfloat();

    if (F.FindChunk(DETOBJ_CHUNK_FLAGS))
    	flags			= F.Rdword();

    // update object
    return 				Update(buf);
}

void CDetail::Save(CFS_Base& F){
	// version
	F.open_chunk		(DETOBJ_CHUNK_VERSION);
    F.Wdword			(DETOBJ_VERSION);
    F.close_chunk		();

    // reference
	F.open_chunk		(DETOBJ_CHUNK_REFERENCE);
    F.WstringZ			(m_sRefs.c_str());
    F.close_chunk		();

	// scale
	F.open_chunk		(DETOBJ_CHUNK_SCALE_LIMITS);
    F.Wfloat			(s_min);
    F.Wfloat			(s_max);
    F.close_chunk		();

	// density factor
	F.open_chunk		(DETOBJ_CHUNK_DENSITY_FACTOR);
    F.Wfloat			(m_fDensityFactor);
    F.close_chunk		();

    // flags
	F.open_chunk		(DETOBJ_CHUNK_FLAGS);
    F.Wdword			(flags);
    F.close_chunk		();
}

void CDetail::Export(CFS_Base& F){
	R_ASSERT			(m_pRefs);
    CSurface* surf		= *m_pRefs->FirstSurface();
	R_ASSERT			(surf);
    // write data
	F.WstringZ			(surf->_ShaderName());
	F.WstringZ			(surf->_Texture());

    F.Wdword			(flags);
    F.Wfloat			(s_min);
    F.Wfloat			(s_max);

    F.Wdword			(number_vertices);
    F.Wdword			(number_indices);

    F.write				(vertices, 	number_vertices*sizeof(fvfVertexIn));
    F.write				(indices, 	number_indices*sizeof(WORD));
}

