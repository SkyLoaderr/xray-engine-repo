//----------------------------------------------------
// file: ExportSkeleton.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ExportSkeleton.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "fmesh.h"
#include "std_classes.h"
#include "bone.h"
#include "motion.h"

#include "MgcCont3DBox.h"         
#include "MgcCont3DMinBox.h"         

#include "ui_main.h"
#include "SkeletonAnimated.h"
#include "nvMeshMender.h"

u32 CSkeletonCollectorPacked::VPack(SSkelVert& V)
{
    u32 P 	= 0xffffffff;

    u32 ix,iy,iz;
    ix = iFloor(float(V.O.x-m_VMmin.x)/m_VMscale.x*clpSMX);
    iy = iFloor(float(V.O.y-m_VMmin.y)/m_VMscale.y*clpSMY);
    iz = iFloor(float(V.O.z-m_VMmin.z)/m_VMscale.z*clpSMZ);
    R_ASSERT(ix<=clpSMX && iy<=clpSMY && iz<=clpSMZ);

	int similar_pos=-1;
    {
        U32Vec& vl=m_VM[ix][iy][iz];
        for(U32It it=vl.begin();it!=vl.end(); it++){
        	SSkelVert& src=m_Verts[*it];
        	if(src.similar_pos(V)){
	            if(src.similar(V)){
                    P = *it;
                    break;
            	}
                similar_pos=*it;
            }
        }
    }
    if (0xffffffff==P)
    {
    	if (similar_pos>=0) V.O.set(m_Verts[similar_pos].O);
        P = m_Verts.size();
        m_Verts.push_back(V);

        m_VM[ix][iy][iz].push_back(P);

        u32 ixE,iyE,izE;
        ixE = iFloor(float(V.O.x+m_VMeps.x-m_VMmin.x)/m_VMscale.x*clpSMX);
        iyE = iFloor(float(V.O.y+m_VMeps.y-m_VMmin.y)/m_VMscale.y*clpSMY);
        izE = iFloor(float(V.O.z+m_VMeps.z-m_VMmin.z)/m_VMscale.z*clpSMZ);

        R_ASSERT(ixE<=clpSMX && iyE<=clpSMY && izE<=clpSMZ);

        if (ixE!=ix)							m_VM[ixE][iy][iz].push_back	(P);
        if (iyE!=iy)							m_VM[ix][iyE][iz].push_back	(P);
        if (izE!=iz)							m_VM[ix][iy][izE].push_back	(P);
        if ((ixE!=ix)&&(iyE!=iy))				m_VM[ixE][iyE][iz].push_back(P);
        if ((ixE!=ix)&&(izE!=iz))				m_VM[ixE][iy][izE].push_back(P);
        if ((iyE!=iy)&&(izE!=iz))				m_VM[ix][iyE][izE].push_back(P);
        if ((ixE!=ix)&&(iyE!=iy)&&(izE!=iz))	m_VM[ixE][iyE][izE].push_back(P);
    }
    return P;
}

CSkeletonCollectorPacked::CSkeletonCollectorPacked(const Fbox &_bb, int apx_vertices, int apx_faces)
{
	Fbox bb;		bb.set(_bb); bb.grow(EPS_L);
    // Params
    m_VMscale.set	(bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);
    m_VMmin.set		(bb.min);
    m_VMeps.set		(m_VMscale.x/clpSMX/2,m_VMscale.y/clpSMY/2,m_VMscale.z/clpSMZ/2);
    m_VMeps.x		= (m_VMeps.x<EPS_L)?m_VMeps.x:EPS_L;
    m_VMeps.y		= (m_VMeps.y<EPS_L)?m_VMeps.y:EPS_L;
    m_VMeps.z		= (m_VMeps.z<EPS_L)?m_VMeps.z:EPS_L;

    // Preallocate memory
    m_Verts.reserve	(apx_vertices);
    m_Faces.reserve	(apx_faces);

    int		_size	= (clpSMX+1)*(clpSMY+1)*(clpSMZ+1);
    int		_average= (apx_vertices/_size)/2;
    for (int ix=0; ix<clpSMX+1; ix++)
        for (int iy=0; iy<clpSMY+1; iy++)
            for (int iz=0; iz<clpSMZ+1; iz++)
                m_VM[ix][iy][iz].reserve	(_average);
}
//----------------------------------------------------

CExportSkeleton::SSplit::SSplit(CSurface* surf, const Fbox& bb):CSkeletonCollectorPacked(bb)
{
	m_b2Link	= FALSE;
	m_Texture 	= surf->_Texture();
	m_Shader	= surf->_ShaderName();
}
//----------------------------------------------------

void CExportSkeleton::SSplit::Save(IWriter& F)
{
    // Header
    F.open_chunk		(OGF_HEADER);
    ogf_header			H;
    H.format_version	= xrOGF_FormatVersion;
    H.type				= (m_SWR.size())?MT_SKELETON_GEOMDEF_PM:MT_SKELETON_GEOMDEF_ST;
    H.shader_id			= 0;
    H.bb.min			= m_Box.min;
    H.bb.max			= m_Box.max;                    
    m_Box.getsphere		(H.bs.c,H.bs.r);
    F.w					(&H,sizeof(H));
    F.close_chunk		();
    
    // Texture
    F.open_chunk		(OGF_TEXTURE);
    F.w_stringZ			(m_Texture);
    F.w_stringZ			(m_Shader);
    F.close_chunk		();

    // Vertices
    F.open_chunk		(OGF_VERTICES);
    F.w_u32				(m_b2Link?OGF_VERTEXFORMAT_FVF_2L:OGF_VERTEXFORMAT_FVF_1L);
    F.w_u32				(m_Verts.size());
    if (m_b2Link){
        for (SkelVertIt v_it=m_Verts.begin(); v_it!=m_Verts.end(); v_it++){
            SSkelVert& pV 	= *v_it;
			// write vertex
            F.w_u16		(pV.B0);
            F.w_u16		(pV.B1);
            F.w			(&pV.O,sizeof(Fvector));		// position (offset)
            F.w			(&pV.N,sizeof(Fvector));		// normal
            F.w			(&pV.T,sizeof(Fvector));		// T        
            F.w			(&pV.B,sizeof(Fvector));		// B        
            F.w_float	(pV.w);
            F.w			(&pV.UV,sizeof(Fvector2));		// tu,tv
        }
    }else{
        for (SkelVertIt v_it=m_Verts.begin(); v_it!=m_Verts.end(); v_it++){
            SSkelVert& pV 	= *v_it;
            F.w			(&pV.O,sizeof(Fvector));		// position (offset)
            F.w			(&pV.N,sizeof(Fvector));		// normal
            F.w			(&pV.T,sizeof(Fvector));		// T        
            F.w			(&pV.B,sizeof(Fvector));		// B        
            F.w			(&pV.UV,sizeof(Fvector2));		// tu,tv
            F.w_u32		(pV.B0);
        }
    }
    F.close_chunk		();

    // Faces
    F.open_chunk		(OGF_INDICES);
    F.w_u32				(m_Faces.size()*3);
    F.w					(m_Faces.begin(),m_Faces.size()*3*sizeof(WORD));
    F.close_chunk		();

    // PMap
    if (m_SWR.size()) {
        F.open_chunk(OGF_P_LODS);
        F.w_u32			(0);			// reserved space 16 bytes
        F.w_u32			(0);
        F.w_u32			(0);
        F.w_u32			(0);
        F.w_u32			(m_SWR.size()); // num collapses
        for (u32 swr_idx=0; swr_idx<m_SWR.size(); swr_idx++)
        	F.w			(&m_SWR[swr_idx],sizeof(VIPM_SWR));
        F.close_chunk();
    }
// SMF
//*
	if (0){
    	static u32 chunk_id = 0; chunk_id++;
		AnsiString r=AnsiString("x:\\import\\test")+chunk_id+".smf";
        IWriter* W 	= FS.w_open(r.c_str());
        for (SkelVertIt v_it=m_Verts.begin(); v_it!=m_Verts.end(); v_it++){
            SSkelVert& pV 	= *v_it;
			// vertices
            AnsiString 		tmp;
            tmp.sprintf		("v %f %f %f",pV.O.x,pV.O.y,-pV.O.z);
            W->w_string		(tmp.c_str());
        }
        // face
        for (SkelFaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
            SSkelFace& pF 	= *f_it;
            AnsiString 		tmp;
            tmp.sprintf		("f %d %d %d",pF.v[0]+1,pF.v[2]+1,pF.v[1]+1);
            W->w_string		(tmp.c_str());
        }
        // normals
        W->w_string			("bind n vertex");
        for (v_it=m_Verts.begin(); v_it!=m_Verts.end(); v_it++){
            SSkelVert& pV 	= *v_it;
            AnsiString 		tmp;
            tmp.sprintf		("n %f %f %f",pV.N.x,pV.N.y,-pV.N.z);
            W->w_string		(tmp.c_str());
        }
        FS.w_close	(W);
	}
    
}

void CExportSkeleton::SSplit::CalculateTB()
{
	u32 v_count_reserve			= 3*iFloor(float(m_Verts.size())*1.33f);
	u32 i_count_reserve			= 3*m_Faces.size();

	// Declare inputs
	xr_vector<NVMeshMender::VertexAttribute> 			input;
	input.push_back	(NVMeshMender::VertexAttribute());	// pos
	input.push_back	(NVMeshMender::VertexAttribute());	// norm
	input.push_back	(NVMeshMender::VertexAttribute());	// tex0
	input.push_back	(NVMeshMender::VertexAttribute());	// w_b0_b1                
	input.push_back	(NVMeshMender::VertexAttribute());	// *** faces

	input[0].Name_= "position";	xr_vector<float>&	i_position	= input[0].floatVector_;	i_position.reserve	(v_count_reserve);
	input[1].Name_= "normal";	xr_vector<float>&	i_normal	= input[1].floatVector_;	i_normal.reserve	(v_count_reserve);
	input[2].Name_= "tex0";		xr_vector<float>&	i_tc		= input[2].floatVector_;	i_tc.reserve		(v_count_reserve);
	input[3].Name_= "w_b0_b1";	xr_vector<float>&	i_w_b0_b1	= input[3].floatVector_;	i_w_b0_b1.reserve	(v_count_reserve);
	input[4].Name_= "indices";	xr_vector<int>&		i_indices	= input[4].intVector_;		i_indices.reserve	(i_count_reserve);

	// Declare outputs
	xr_vector<NVMeshMender::VertexAttribute> 			output;
	output.push_back(NVMeshMender::VertexAttribute());	// position, needed for mender
	output.push_back(NVMeshMender::VertexAttribute());	// normal
	output.push_back(NVMeshMender::VertexAttribute());	// tangent
	output.push_back(NVMeshMender::VertexAttribute());	// binormal
	output.push_back(NVMeshMender::VertexAttribute());	// tex0
	output.push_back(NVMeshMender::VertexAttribute());	// w_b0_b1
	output.push_back(NVMeshMender::VertexAttribute());	// *** faces

	output[0].Name_= "position";
	output[1].Name_= "normal";
	output[2].Name_= "tangent";	
	output[3].Name_= "binormal";
	output[4].Name_= "tex0";	
	output[5].Name_= "w_b0_b1";	
	output[6].Name_= "indices";	

    // fill inputs (verts&indices)
    for (SkelVertIt vert_it=m_Verts.begin(); vert_it!=m_Verts.end(); vert_it++){
        SSkelVert	&iV = *vert_it;
        i_position.push_back(iV.O.x);	i_position.push_back(iV.O.y);	i_position.push_back(iV.O.z);
        i_normal.push_back	(iV.N.x);  	i_normal.push_back	(iV.N.y);	i_normal.push_back	(iV.N.z);
        i_tc.push_back		(iV.UV.x);	i_tc.push_back		(iV.UV.y);	i_tc.push_back		(0);
        i_w_b0_b1.push_back	(iV.w);		i_w_b0_b1.push_back	(iV.B0);	i_w_b0_b1.push_back	(iV.B1);
    }
    for (SkelFaceIt face_it=m_Faces.begin(); face_it!=m_Faces.end(); face_it++){
        SSkelFace	&iF = *face_it;
		i_indices.push_back	(iF.v[0]);
		i_indices.push_back	(iF.v[1]);
		i_indices.push_back	(iF.v[2]);
    }
    
	// Perform munge
	NVMeshMender mender;
	if (!mender.Munge(
		input,										// input attributes
		output,										// outputs attributes
		deg2rad(75.f),								// tangent space smooth angle
		0,											// no texture matrix applied to my texture coordinates
		NVMeshMender::FixTangents,					// fix degenerate bases & texture mirroring
		NVMeshMender::DontFixCylindricalTexGen,		// handle cylindrically mapped textures via vertex duplication
		NVMeshMender::DontWeightNormalsByFaceSize	// weigh vertex normals by the triangle's size
		))
	{
		Debug.fatal	("NVMeshMender failed (%s)",mender.GetLastError().c_str());
	}

	// Bind declarators
	// bind
	output[0].Name_= "position";
	output[1].Name_= "normal";
	output[2].Name_= "tangent";	
	output[3].Name_= "binormal";
	output[4].Name_= "tex0";	
	output[5].Name_= "w_b0_b1";	
	output[6].Name_= "indices";	

	xr_vector<float>&	o_position	= output[0].floatVector_;	R_ASSERT(output[0].Name_=="position");
	xr_vector<float>&	o_normal	= output[1].floatVector_;	R_ASSERT(output[1].Name_=="normal");
	xr_vector<float>&	o_tangent	= output[2].floatVector_;	R_ASSERT(output[2].Name_=="tangent");
	xr_vector<float>&	o_binormal	= output[3].floatVector_;	R_ASSERT(output[3].Name_=="binormal");
	xr_vector<float>&	o_tc		= output[4].floatVector_;	R_ASSERT(output[4].Name_=="tex0");
	xr_vector<float>&	o_w_b0_b1	= output[5].floatVector_;	R_ASSERT(output[5].Name_=="w_b0_b1");
	xr_vector<int>&		o_indices	= output[6].intVector_;		R_ASSERT(output[6].Name_=="indices");

	// verify
	R_ASSERT		(3*m_Faces.size()	== o_indices.size());
    u32 v_cnt		= o_position.size();
    R_ASSERT		(0==v_cnt%3);
    R_ASSERT		(v_cnt == o_normal.size());
    R_ASSERT		(v_cnt == o_tangent.size());
    R_ASSERT		(v_cnt == o_binormal.size());
    R_ASSERT		(v_cnt == o_tc.size());
    R_ASSERT		(v_cnt == o_w_b0_b1.size());
    v_cnt			/= 3;

    // retriving data
    u32 o_idx		= 0;
    for (face_it=m_Faces.begin(); face_it!=m_Faces.end(); face_it++){
        SSkelFace	&iF = *face_it;
        iF.v[0]		= o_indices[o_idx++];
        iF.v[1]		= o_indices[o_idx++];
        iF.v[2]		= o_indices[o_idx++];
    }
    m_Verts.clear	(); m_Verts.resize(v_cnt);
    for (u32 v_idx=0; v_idx!=v_cnt; v_idx++){
        SSkelVert	&iV = m_Verts[v_idx];
        iV.O.set	(o_position[v_idx*3+0],	o_position[v_idx*3+1],	o_position[v_idx*3+2]);
        iV.N.set	(o_normal[v_idx*3+0],	o_normal[v_idx*3+1],	o_normal[v_idx*3+2]);
        iV.T.set	(o_tangent[v_idx*3+0],	o_tangent[v_idx*3+1],	o_tangent[v_idx*3+2]);
        iV.B.set	(o_binormal[v_idx*3+0],	o_binormal[v_idx*3+1],	o_binormal[v_idx*3+2]);
        iV.UV.set	(o_tc[v_idx*3+0],		o_tc[v_idx*3+1]);
        iV.w		= o_w_b0_b1[v_idx*3+0];
        iV.B0		= iFloor(o_w_b0_b1[v_idx*3+1]+EPS_L);
        iV.B1		= iFloor(o_w_b0_b1[v_idx*3+2]+EPS_L);
    }

    // Optimize texture coordinates
    // 1. Calc bounds
    Fvector2 	Tdelta;
    Fvector2 	Tmin,Tmax;
    Tmin.set	(flt_max,flt_max);
    Tmax.set	(flt_min,flt_min);
    for (v_idx=0; v_idx!=v_cnt; v_idx++){
        SSkelVert	&iV = m_Verts[v_idx];
        Tmin.min	(iV.UV);
        Tmax.max	(iV.UV);
    }
    Tdelta.x 	= floorf((Tmax.x-Tmin.x)/2+Tmin.x);
    Tdelta.y 	= floorf((Tmax.y-Tmin.y)/2+Tmin.y);

    Fvector2	Tsize;
    Tsize.sub	(Tmax,Tmin);
    if ((Tsize.x>32)||(Tsize.y>32))
    	Msg		("#!Surface [T:'%s', S:'%s'] has UV tiled more than 32 times.",m_Texture.c_str(),m_Shader.c_str());
    
    // 2. Recalc UV mapping
    for (v_idx=0; v_idx!=v_cnt; v_idx++){
        SSkelVert	&iV = m_Verts[v_idx];
        iV.UV.sub	(Tdelta);
    }
}

void CExportSkeleton::SSplit::MakeProgressive()
{
	VIPM_Init	();
    for (SkelVertIt vert_it=m_Verts.begin(); vert_it!=m_Verts.end(); vert_it++)
    	VIPM_AppendVertex(vert_it->O,vert_it->UV);
    for (SkelFaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++)
    	VIPM_AppendFace(f_it->v[0],f_it->v[1],f_it->v[2]);

    VIPM_Result* R = VIPM_Convert(u32(-1),1.f,1);

    // Permute vertices
    SkelVertVec temp_list = m_Verts;
    for(u32 i=0; i<temp_list.size(); i++)
        m_Verts[R->permute_verts[i]]=temp_list[i];
    
    // Fill indices
    m_Faces.resize	(R->indices.size()/3);
    for (u32 f_idx=0; f_idx<m_Faces.size(); f_idx++){
	    SSkelFace& F= m_Faces[f_idx];
    	F.v[0]			= R->indices[f_idx*3+0];
    	F.v[1]			= R->indices[f_idx*3+1];
    	F.v[2]			= R->indices[f_idx*3+2];
    }

    // Fill SWR
    m_SWR.resize		(R->swr_records.size());
    for (u32 swr_idx=0; swr_idx!=m_SWR.size(); swr_idx++)
    	m_SWR[swr_idx]	= R->swr_records[swr_idx];

    // cleanup
    VIPM_Destroy		();
}

/*
    // write SMF
    AnsiString r		= "x:\\import\\test.smf";
    IWriter* W 			= FS.w_open(r.c_str());
    // vertices
    for (SkelVertIt v_it=m_Verts.begin(); v_it!=m_Verts.end(); v_it++){
        SSkelVert& pV 	= *v_it;
        AnsiString 		tmp;
        tmp.sprintf		("v %f %f %f",pV.O.x,pV.O.y,-pV.O.z);
        W->w_string		(tmp.c_str());
    }
    u32 idx 			= R->swr_records.size()-1;
    VIPM_SWR& swr		= R->swr_records[idx];
    for (f_idx=0; f_idx<swr.num_tris; f_idx++){
	    SSkelFace& F	= m_Faces[f_idx+swr.offset/3];

		u16* idx		= &R->indices[swr.offset+f_idx*3];
	    SSkelFace F0;
        F0.v[0]			= *(idx+0);
        F0.v[1]			= *(idx+1);
        F0.v[2]			= *(idx+2);

        AnsiString 		tmp;
        tmp.sprintf		("f %d %d %d",F.v[2]+1,F.v[1]+1,F.v[0]+1);
		W->w_string		(tmp.c_str());
    }
    FS.w_close	(W);
*/

CExportSkeleton::CExportSkeleton(CEditableObject* object)
{
	m_Source=object;
}
//----------------------------------------------------

void ComputeOBB	(Fobb &B, FvectorVec& V)
{
    if (V.size()<3) { B.invalidate(); return; }
    Mgc::Box3	BOX		= Mgc::ContOrientedBox(V.size(), (const Mgc::Vector3*) V.begin());
    B.m_rotate.i.set	(BOX.Axis(0));
    B.m_rotate.j.set	(BOX.Axis(1));
    B.m_rotate.k.set	(BOX.Axis(2));

    // Normalize rotation matrix (были проблемы ContOrientedBox - выдает левую матрицу)
    B.m_rotate.i.crossproduct(B.m_rotate.j,B.m_rotate.k);
    B.m_rotate.j.crossproduct(B.m_rotate.k,B.m_rotate.i);
    
    B.m_translate.set	(BOX.Center());
    B.m_halfsize.set	(BOX.Extents()[0],BOX.Extents()[1],BOX.Extents()[2]);

    if (!_valid(B.m_rotate)||!_valid(B.m_translate)||!_valid(B.m_halfsize)){
        BOX					= Mgc::MinBox(V.size(), (const Mgc::Vector3*) V.begin());
        B.m_rotate.i.set	(BOX.Axis(0));
        B.m_rotate.j.set	(BOX.Axis(1));
        B.m_rotate.k.set	(BOX.Axis(2));

        // Normalize rotation matrix (были проблемы ContOrientedBox - выдает левую матрицу)
        B.m_rotate.i.crossproduct(B.m_rotate.j,B.m_rotate.k);
        B.m_rotate.j.crossproduct(B.m_rotate.k,B.m_rotate.i);
    
        B.m_translate.set	(BOX.Center());
        B.m_halfsize.set	(BOX.Extents()[0],BOX.Extents()[1],BOX.Extents()[2]);
    }
    VERIFY (_valid(B.m_rotate)&&_valid(B.m_translate)&&_valid(B.m_halfsize));
}
//----------------------------------------------------

int CExportSkeletonCustom::FindSplit(LPCSTR shader, LPCSTR texture)
{
	for (SplitIt it=m_Splits.begin(); it!=m_Splits.end(); it++){
		if ((0==stricmp(*it->m_Texture,texture))&&(0==stricmp(*it->m_Shader,shader))) return it-m_Splits.begin();
    }
    return -1;
}
//----------------------------------------------------

bool CExportSkeleton::ExportGeometry(IWriter& F)
{
    if( m_Source->MeshCount() == 0 ) return false;

    if (m_Source->BoneCount()<1){
    	ELog.Msg(mtError,"There are no bones in the object.");
     	return false;
    }

    if (m_Source->BoneCount()>64){
    	ELog.Msg(mtError,"Object cannot handle more than 64 bones.");
     	return false;
    }

    // mem active motion
    CSMotion* active_motion=m_Source->ResetSAnimation();

    R_ASSERT(m_Source->IsDynamic()&&m_Source->IsSkeleton());

    SPBItem* pb = UI->ProgressStart(5+m_Source->MeshCount()*2+m_Source->SurfaceCount(),"Export skeleton geometry...");
    pb->Inc		();

    BoneVec& bones 			= m_Source->Bones();
    xr_vector<FvectorVec>	bone_points;
	bone_points.resize	(m_Source->BoneCount());

    u32 mtl_cnt=0;
	UI->SetStatus("Split meshes...");
    for(EditMeshIt mesh_it=m_Source->FirstMesh();mesh_it!=m_Source->LastMesh();mesh_it++){
        CEditableMesh* MESH = *mesh_it;
        // generate vertex offset
        if (!MESH->m_LoadState.is(CEditableMesh::LS_SVERTICES)) MESH->GenerateSVertices();
        pb->Inc		();
        // fill faces
        for (SurfFacesPairIt sp_it=MESH->m_SurfFaces.begin(); sp_it!=MESH->m_SurfFaces.end(); sp_it++){
            IntVec& face_lst = sp_it->second;
            CSurface* surf = sp_it->first;
            u32 dwTexCnt = ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
            R_ASSERT(dwTexCnt==1);
            int mtl_idx = FindSplit(surf->_ShaderName(),surf->_Texture());
            if (mtl_idx<0){
            	m_Splits.push_back(SSplit(surf,m_Source->GetBox()));
                mtl_idx=mtl_cnt++;
            }
            SSplit& split=m_Splits[mtl_idx];
            for (IntIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
            	int f_idx = *f_it;
                st_Face& face = MESH->m_Faces[f_idx];
                {
                    SSkelVert v[3];
                    for (int k=0; k<3; k++){
                        st_FaceVert& 	fv 	= face.pv[k];
                        st_SVert& 		sv 	= MESH->m_SVertices[f_idx*3+k];
                        if ((sv.bone1==BI_NONE)||(sv.bone0==sv.bone1)){
	                        v[k].set	(sv.offs,sv.norm,sv.uv,sv.w,sv.bone0,sv.bone0);
                        }else{                                   
                        	split.m_b2Link = TRUE;     
	                        v[k].set	(sv.offs,sv.norm,sv.uv,sv.w,sv.bone0,sv.bone1);
                        }
                    }
                    split.add_face(v[0], v[1], v[2]);
			        if (surf->m_Flags.is(CSurface::sf2Sided)){
                    	v[0].N.invert(); v[1].N.invert(); v[2].N.invert();
                    	split.add_face	(v[0], v[2], v[1]);
                    }
                }
            }
            if (!split.valid()){
				ELog.Msg(mtError,"Degenerate split found (Material '%s'). Removed.",surf->_Name());
                m_Splits.pop_back();
            }
        }
        // mesh fin
        MESH->UnloadSVertices();
        MESH->UnloadPNormals();
        MESH->UnloadFNormals();
        pb->Inc		();
    }
    UI->SetStatus("Make progressive...");
    // fill per bone vertices
    for (SplitIt split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++){
		split_it->CalculateTB();
		if (m_Source->m_Flags.is(CEditableObject::eoProgressive)) split_it->MakeProgressive();
		SkelVertVec& lst = split_it->getV_Verts();
	    for (SkelVertIt sv_it=lst.begin(); sv_it!=lst.end(); sv_it++){
		    bone_points[sv_it->B0].push_back(sv_it->O);
            bones[sv_it->B0]->_RITransform().transform_tiny(bone_points[sv_it->B0].back());
        }
        pb->Inc		();
    }
    pb->Inc		();

    // coumpute bounding
    ComputeBounding	();

	// create OGF
    // Header
    ogf_header 		H;
    H.format_version= xrOGF_FormatVersion;
    H.type			= m_Source->IsAnimated()?MT_SKELETON_ANIM:MT_SKELETON_RIGID;
    H.shader_id		= 0;
    H.bb.min		= m_Box.min;
    H.bb.max		= m_Box.max;
    m_Box.getsphere	(H.bs.c,H.bs.r);
    F.w_chunk		(OGF_HEADER,&H,sizeof(H));

    // Desc
    ogf_desc		desc;
    m_Source->PrepareOGFDesc(desc);
    F.open_chunk	(OGF_S_DESC);
    desc.Save		(F);
    F.close_chunk	();
	
    // OGF_CHILDREN
    F.open_chunk	(OGF_CHILDREN);
    int chield=0;
    for (split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++){
	    F.open_chunk(chield++);
        split_it->Save(F);
	    F.close_chunk();
    }
    F.close_chunk();
    pb->Inc		();


    UI->SetStatus("Compute bounding volume...");
    pb->Inc		();

    // BoneNames
    F.open_chunk(OGF_S_BONE_NAMES);
    F.w_u32(m_Source->BoneCount());
    int bone_idx=0;
    for (BoneIt bone_it=m_Source->FirstBone(); bone_it!=m_Source->LastBone(); bone_it++,bone_idx++){
        F.w_stringZ	((*bone_it)->Name());
        F.w_stringZ	((*bone_it)->Parent()?(*bone_it)->ParentName():"");
        Fobb	obb;
        ComputeOBB	(obb,bone_points[bone_idx]);
        F.w			(&obb,sizeof(Fobb));
    }
    F.close_chunk();

    bool bRes = true;
                    
    F.open_chunk(OGF_S_IKDATA);
    for (bone_it=m_Source->FirstBone(); bone_it!=m_Source->LastBone(); bone_it++,bone_idx++)
        if (!(*bone_it)->ExportOGF(F)) bRes=false; 
    F.close_chunk();

    if (m_Source->GetClassScript().size()){
        F.open_chunk(OGF_S_USERDATA);
        std::string aaa;
        F.w_stringZ(aaa);//m_Source->m_ClassScript);
        F.close_chunk();
    }

    pb->Inc		();
    UI->ProgressEnd(pb);

    // restore active motion
    m_Source->SetActiveSMotion(active_motion);

    return bRes;
}
//----------------------------------------------------

bool CExportSkeleton::ExportMotionKeys(IWriter& F)
{
    if (!!m_Source->m_SMotionRefs.size()||(m_Source->SMotionCount()<1)){
    	ELog.Msg(mtError,"Object doesn't have own motion.");
     	return !!m_Source->m_SMotionRefs.size();
    }

	SPBItem* pb = UI->ProgressStart(1+m_Source->SMotionCount(),"Export skeleton motions keys...");
    pb->Inc		();

    // mem active motion
    CSMotion* active_motion=m_Source->ResetSAnimation();

    // Motions
    F.open_chunk			(OGF_S_MOTIONS);
    F.open_chunk			(0);
    F.w_u32					(m_Source->SMotionCount());
    F.close_chunk			();
    int smot 				= 1;

    // use global transform
    Fmatrix	mGT,mTranslate,mRotate;
    mRotate.setHPB			(m_Source->a_vRotate.y, m_Source->a_vRotate.x, m_Source->a_vRotate.z);
    mTranslate.translate	(m_Source->a_vPosition);
    mGT.mul					(mTranslate,mRotate);

    for (SMotionIt motion_it=m_Source->FirstSMotion(); motion_it!=m_Source->LastSMotion(); motion_it++, smot++){
        CSMotion* motion 	= *motion_it;
        F.open_chunk(smot);
        F.w_stringZ(motion->Name());
        F.w_u32(motion->Length());

        u32 dwLen			= motion->Length();
		CKeyQR* _keysQR 	= xr_alloc<CKeyQR>(dwLen); 
		CKeyQT* _keysQT 	= xr_alloc<CKeyQT>(dwLen); 
		Fvector* _keysT 	= xr_alloc<Fvector>(dwLen);
        
        BoneMotionVec& lst=motion->BoneMotions();
        int bone_id = 0;
        for (BoneMotionIt bm_it=lst.begin(); bm_it!=lst.end(); bm_it++,bone_id++){
            Flags8 flags = motion->GetMotionFlags(bone_id);
            CBone* B 	= m_Source->GetBone(bone_id);
            CBone* PB 	= B->Parent();
            for (int frm=motion->FrameStart(); frm<motion->FrameEnd(); frm++){
                float t = (float)frm/motion->FPS();
                Fvector T,R;
                Fquaternion q;
                motion->_Evaluate	(bone_id,t,T,R);
                B->_Update			(T,R);
                m_Source->CalculateAnimation(B,motion);
                Fmatrix mat			= B->_MTransform();
                if (flags.is(st_BoneMotion::flWorldOrient)){
                    Fmatrix 	parent;
                    Fmatrix 	inv_parent;
                    if(PB){
                        m_Source->GetBoneWorldTransform(PB->index,t,motion,parent);
                        inv_parent.invert(parent);
                    }else{
                        parent 		= Fidentity;
                        inv_parent	= Fidentity;
                    }
                    Fmatrix 	rot;
                    rot.setXYZi	(R.x,R.y,R.z);
                    mat.mul		(inv_parent,rot);
                }
                // apply global transform
                if (B->IsRoot()){
                	mGT.transform_tiny(T);
                    mat.mulA(mGT);
                }

                q.set		(mat);

                CKeyQR&	Kr 	= _keysQR[frm-motion->FrameStart()];
                Fvector&Kt 	= _keysT [frm-motion->FrameStart()];
                
                // Quantize quaternion
                int	_x 		= int(q.x*KEY_Quant); clamp(_x,-32767,32767); Kr.x =  _x;
                int	_y 		= int(q.y*KEY_Quant); clamp(_y,-32767,32767); Kr.y =  _y;
                int	_z 		= int(q.z*KEY_Quant); clamp(_z,-32767,32767); Kr.z =  _z;
                int	_w 		= int(q.w*KEY_Quant); clamp(_w,-32767,32767); Kr.w =  _w;
                Kt.set	(T);
            }
            
            // check T
            u8 t_present	= FALSE;
            R_ASSERT		(dwLen);
            Fvector Mt		= {0,0,0};
            Fvector Ct		= {0,0,0};
            Fvector St		= {0,0,0};
            Fvector At		= _keysT[0];
            Fvector Bt		= _keysT[0];
            for (u32 t_idx=0; t_idx<dwLen; t_idx++){
            	Fvector& t	= _keysT[t_idx];
            	Mt.add		(t);
                At.x		= _min(At.x,t.x);
                At.y		= _min(At.y,t.y);
                At.z		= _min(At.z,t.z);
                Bt.x		= _max(Bt.x,t.x);
                Bt.y		= _max(Bt.y,t.y);
                Bt.z		= _max(Bt.z,t.z);
            }
            Mt.div			(dwLen);
            Ct.add			(Bt,At);
            Ct.mul			(0.5f);
            St.sub			(Bt,At);
            St.mul			(0.5f);
            for (t_idx=0; t_idx<dwLen; t_idx++){
            	Fvector& t	= _keysT[t_idx];
                if (!Mt.similar(t,EPS_L)){t_present=TRUE;}
                
                CKeyQT&	Kt 	= _keysQT[t_idx];
                int	_x 		= int(127.f*(t.x-Ct.x)/St.x); clamp(_x,-128,127); Kt.x =  _x;
                int	_y 		= int(127.f*(t.y-Ct.y)/St.y); clamp(_y,-128,127); Kt.y =  _y;
                int	_z 		= int(127.f*(t.z-Ct.z)/St.z); clamp(_z,-128,127); Kt.z =  _z;
            }
            St.div	(127.f);
            // save
            F.w_u8	(t_present);
			F.w_u32	(crc32(_keysQR,dwLen*sizeof(CKeyQR)));
            F.w		(_keysQR,dwLen*sizeof(CKeyQR));
            if (t_present){	
	            F.w_u32(crc32(_keysQT,u32(dwLen*sizeof(CKeyQT))));
            	F.w	(_keysQT,dwLen*sizeof(CKeyQT));
	            F.w_fvector3(St);
    	        F.w_fvector3(Ct);
            }else{
                F.w_fvector3(Mt);
            }
        }
        // free temp storage
        xr_free(_keysQR);
        xr_free(_keysQT);
        xr_free(_keysT);

        F.close_chunk();
    pb->Inc		();
    }
    F.close_chunk();
	UI->ProgressEnd(pb);

    // restore active motion
    m_Source->SetActiveSMotion(active_motion);
    return true;
}

bool CExportSkeleton::ExportMotionDefs(IWriter& F)
{
    if (!m_Source->IsAnimated()){ 
    	ELog.Msg(mtError,"Object doesn't have any motion or motion refs.");
    	return false;
    }

    bool bRes=true;

	SPBItem* pb = UI->ProgressStart(3,"Export skeleton motions defs...");
    pb->Inc		();

    if (m_Source->m_SMotionRefs.size()){
	    F.open_chunk	(OGF_S_MOTION_REFS);
    	F.w_stringZ		(m_Source->m_SMotionRefs);
	    F.close_chunk	();
	    pb->Inc		();
    }else{
        // save smparams
        F.open_chunk	(OGF_S_SMPARAMS);
        F.w_u16			(xrOGF_SMParamsVersion);
        // bone parts
        BPVec& bp_lst 	= m_Source->BoneParts();
        if (bp_lst.size()){
            if (m_Source->VerifyBoneParts()){
                F.w_u16(bp_lst.size());
                for (BPIt bp_it=bp_lst.begin(); bp_it!=bp_lst.end(); bp_it++){
                    F.w_stringZ	(LowerCase(bp_it->alias.c_str()).c_str());
                    F.w_u16		(bp_it->bones.size());
                    for (int i=0; i<int(bp_it->bones.size()); i++){
                        F.w_stringZ	(bp_it->bones[i].c_str());
                        int idx 	= m_Source->FindBoneByNameIdx(bp_it->bones[i].c_str()); VERIFY(idx>=0);
                        F.w_u32		(idx);
                    }
                }
            }else{
                ELog.Msg(mtError,"Invalid bone parts (missing or duplicate bones).");
                bRes 	= false;
            }
        }else{
            F.w_u16(1);
            F.w_stringZ("default");
            F.w_u16(m_Source->BoneCount());
            for (int i=0; i<m_Source->BoneCount(); i++) F.w_u32(i);
        }
	    pb->Inc		();
        // motion defs
        SMotionVec& sm_lst	= m_Source->SMotions();
        F.w_u16(sm_lst.size());
        for (SMotionIt motion_it=sm_lst.begin(); motion_it!=sm_lst.end(); motion_it++){
            CSMotion* motion = *motion_it;
            // verify
            if (!motion->m_Flags.is(esmFX)){
                if (!((motion->m_BoneOrPart==BI_NONE)||(motion->m_BoneOrPart<(int)bp_lst.size()))){
                    ELog.Msg(mtError,"Invalid Bone Part of motion: '%s'.",motion->Name());
                    bRes=false;
                    continue;
                }
            }
            if (bRes){
                // export
                F.w_stringZ	(motion->Name());
                F.w_u32		(motion->m_Flags.get());
                F.w_u16		(motion->m_BoneOrPart);
                F.w_u16		(motion_it-sm_lst.begin());
                F.w_float	(motion->fSpeed);
                F.w_float	(motion->fPower);
                F.w_float	(motion->fAccrue);
                F.w_float	(motion->fFalloff);
            }
        }
	    pb->Inc		();
        F.close_chunk();
    }
    
	UI->ProgressEnd(pb);
    return bRes;
}

bool CExportSkeleton::ExportMotions(IWriter& F)
{
	if (!ExportMotionKeys(F)) 	return false;
	if (!ExportMotionDefs(F)) 	return false;
    return true;
}
//----------------------------------------------------

bool CExportSkeleton::Export(IWriter& F)
{
    if (!ExportGeometry(F)) 						return false;
    if (m_Source->IsAnimated()&&!ExportMotions(F))	return false;
    return true;
};
//----------------------------------------------------



