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
    OnDeviceDestroy		();
}

LPCSTR CDetail::GetName	(){
    return m_pRefs?m_pRefs->GetName():m_sRefs.c_str();
}

void CDetail::DefferedLoad()
{
}

void CDetail::OnDeviceCreate()
{
	if (!m_pRefs)		return;
    CSurface* surf		= *m_pRefs->FirstSurface();
    VERIFY				(surf);
    AnsiString	s_name	= surf->_ShaderName();
    AnsiString	t_name	= surf->_Texture();
	shader				= Device.Shader.Create(s_name.c_str(),t_name.c_str());
}

void CDetail::OnDeviceDestroy()
{
	Device.Shader.Delete(shader);
}

int CDetail::_AddVert(const Fvector& p, float u, float v)
{
	fvfVertexIn V(p,u,v);
    for (u32 k=0; k<number_vertices; k++)
    	if (vertices[k].similar(V)) return k;
    number_vertices++;
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

    // fill geometry
    CEditableMesh* M	= *m_pRefs->FirstMesh();
	number_indices 		= M->GetFaceCount(false)*3;
	indices				= (WORD*)xr_malloc(number_indices*sizeof(WORD));

    // fill vertices
    bv_bb.invalidate();
    u32 idx			= 0;
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

    OnDeviceCreate		();

    return true;
}

bool CDetail::Load(IReader& F){
	// check version
    R_ASSERT			(F.find_chunk(DETOBJ_CHUNK_VERSION));
    u32 version		= F.r_u32();
    if (version!=DETOBJ_VERSION){
    	ELog.Msg(mtError,"CDetail: unsupported version.");
        return false;
    }

	// references
	char buf[255];
    R_ASSERT			(F.find_chunk(DETOBJ_CHUNK_REFERENCE));
    F.r_stringZ			(buf);

    // scale
    R_ASSERT			(F.find_chunk(DETOBJ_CHUNK_SCALE_LIMITS));
    s_min				= F.r_float(); if (fis_zero(s_min)) 	s_min = 0.1f;
	s_max		        = F.r_float(); if (s_max<s_min)		s_max = s_min;

	// density factor
    if (F.find_chunk(DETOBJ_CHUNK_DENSITY_FACTOR))
	    m_fDensityFactor= F.r_float();

    if (F.find_chunk(DETOBJ_CHUNK_FLAGS))
    	flags			= F.r_u32();

    // update object
    return 				Update(buf);
}

void CDetail::Save(IWriter& F){
	// version
	F.open_chunk		(DETOBJ_CHUNK_VERSION);
    F.w_u32				(DETOBJ_VERSION);
    F.close_chunk		();

    // reference
	F.open_chunk		(DETOBJ_CHUNK_REFERENCE);
    F.w_stringZ			(m_sRefs.c_str());
    F.close_chunk		();

	// scale
	F.open_chunk		(DETOBJ_CHUNK_SCALE_LIMITS);
    F.w_float			(s_min);
    F.w_float			(s_max);
    F.close_chunk		();

	// density factor
	F.open_chunk		(DETOBJ_CHUNK_DENSITY_FACTOR);
    F.w_float			(m_fDensityFactor);
    F.close_chunk		();

    // flags
	F.open_chunk		(DETOBJ_CHUNK_FLAGS);
    F.w_u32				(flags);
    F.close_chunk		();
}

void CDetail::Export(IWriter& F){
	R_ASSERT			(m_pRefs);
    CSurface* surf		= *m_pRefs->FirstSurface();
	R_ASSERT			(surf);
    // write data
	F.w_stringZ			(surf->_ShaderName());
	F.w_stringZ			(surf->_Texture());

    F.w_u32				(flags);
    F.w_float			(s_min);
    F.w_float			(s_max);

    F.w_u32				(number_vertices);
    F.w_u32				(number_indices);

    F.w					(vertices, 	number_vertices*sizeof(fvfVertexIn));
    F.w					(indices, 	number_indices*sizeof(WORD));
}

