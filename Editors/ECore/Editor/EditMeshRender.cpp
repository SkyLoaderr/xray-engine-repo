//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMeshVLight.h"
#include "EditMesh.h"
#include "EditObject.h"
#include "ui_main.h"
#include "d3dutils.h"
#include "render.h"
//----------------------------------------------------
#define F_LIM (10000)
#define V_LIM (F_LIM*3)
//----------------------------------------------------
void CEditableMesh::CreateRenderBuffers()
{
    if (m_LoadState.is(LS_RBUFFERS)) return;

    if (!m_LoadState.is(LS_PNORMALS)) GeneratePNormals();
    R_ASSERT2(m_RenderBuffers.empty(),"Render buffer already exist.");

	SPBItem* pb 	= UI->ProgressStart((float)m_SurfFaces.size(),"Loading RB...");

    VERIFY(m_PNormals.size());

    for (SurfFacesPairIt sp_it=m_SurfFaces.begin(); sp_it!=m_SurfFaces.end(); sp_it++){
		IntVec& face_lst = sp_it->second;
        CSurface* _S = sp_it->first;
        int num_verts=face_lst.size()*3;
        RBVector rb_vec;
		int v_cnt=num_verts;
        int start_face=0;
        int num_face;
        VERIFY3	(v_cnt,"Empty surface arrive.",_S->_Name());
        do{
	        rb_vec.push_back	(st_RenderBuffer(0,(v_cnt<V_LIM)?v_cnt:V_LIM));
            st_RenderBuffer& rb	= rb_vec.back();
            if (_S->m_Flags.is(CSurface::sf2Sided)) 	rb.dwNumVertex *= 2;
            num_face			= (v_cnt<V_LIM)?v_cnt/3:F_LIM;

            int buf_size		= D3DXGetFVFVertexSize(_S->_FVF())*rb.dwNumVertex;
            R_ASSERT2			(buf_size,"Empty buffer size or bad FVF.");
			u8*	bytes			= 0;
			IDirect3DVertexBuffer9*	pVB=0;
//			IDirect3DIndexBuffer9*	pIB=0;
			R_CHK(HW.pDevice->CreateVertexBuffer(buf_size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &pVB, 0));
//            R_CHK(HW.pDevice->CreateIndexBuffer(i_cnt*sizeof(u16),D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&pIB,NULL));
			rb.pGeom.create		(_S->_FVF(),pVB,0);

			R_CHK				(pVB->Lock(0,0,(LPVOID*)&bytes,0));
			FillRenderBuffer	(face_lst,start_face,num_face,_S,bytes);
			pVB->Unlock			();

            v_cnt				-= V_LIM;
            start_face			+= (_S->m_Flags.is(CSurface::sf2Sided))?rb.dwNumVertex/6:rb.dwNumVertex/3;
        }while(v_cnt>0);
        if (num_verts>0) m_RenderBuffers.insert(mk_pair(_S,rb_vec));
		pb->Inc		();
    }
    UnloadPNormals();
    m_LoadState.set(LS_RBUFFERS,TRUE);
	UI->ProgressEnd(pb);
}
//----------------------------------------------------

void CEditableMesh::ClearRenderBuffers()
{
    for (RBMapPairIt rbmp_it=m_RenderBuffers.begin(); rbmp_it!=m_RenderBuffers.end(); rbmp_it++){
        for(RBVecIt rb_it=rbmp_it->second.begin(); rb_it!=rbmp_it->second.end(); rb_it++)
            if (rb_it->pGeom){
                _RELEASE		(rb_it->pGeom->vb);
                _RELEASE		(rb_it->pGeom->ib);
                rb_it->pGeom.destroy();
            }
    }
    m_RenderBuffers.clear();
    m_LoadState.set(LS_RBUFFERS,FALSE);
}
//----------------------------------------------------

void CEditableMesh::FillRenderBuffer(IntVec& face_lst, int start_face, int num_face, const CSurface* surf, LPBYTE& src_data){
	LPBYTE data = src_data;
    u32 dwFVF = surf->_FVF();
	u32 dwTexCnt = ((dwFVF&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
    for (int fl_i=start_face; fl_i<start_face+num_face; fl_i++){
        u32 f_index = face_lst[fl_i];
        VERIFY(f_index<m_Faces.size());
    	st_Face& face = m_Faces[f_index];
        for (int k=0; k<3; k++){
            st_FaceVert& fv = face.pv[k];
            u32 norm_id = f_index*3+k;//fv.pindex;
	        VERIFY2(norm_id<m_PNormals.size(),"Normal index out of range.");
            VERIFY2(fv.pindex<(int)m_Points.size(),"Point index out of range.");
            Fvector& PN = m_PNormals[norm_id];
			Fvector& V 	= m_Points[fv.pindex];
            int sz;
            if (dwFVF&D3DFVF_XYZ){
                sz=sizeof(Fvector);
                VERIFY2(fv.pindex<int(m_Points.size()),"- Face index out of range.");
                CopyMemory(data,&V,sz);
                data+=sz;
            }
            if (dwFVF&D3DFVF_NORMAL){
                sz=sizeof(Fvector);
                CopyMemory(data,&PN,sz);
                data+=sz;
            }
            sz=sizeof(Fvector2);
            int offs = 0;
            for (int t=0; t<(int)dwTexCnt; t++){
                VERIFY2((t+offs)<(int)m_VMRefs[fv.vmref].size(),"- VMap layer index out of range");
            	st_VMapPt& vm_pt 	= m_VMRefs[fv.vmref][t+offs];
                if (m_VMaps[vm_pt.vmap_index]->type!=vmtUV){
                	offs++;
                    t--;
                    continue;
                }
                VERIFY2(vm_pt.vmap_index<int(m_VMaps.size()),"- VMap index out of range");
				st_VMap* vmap		= m_VMaps[vm_pt.vmap_index];
                VERIFY2(vm_pt.index<vmap->size(),"- VMap point index out of range");
                CopyMemory(data,&vmap->getUV(vm_pt.index),sz); data+=sz;
//                Msg("%3.2f, %3.2f",vmap->getUV(vm_pt.index).x,vmap->getUV(vm_pt.index).y);
            }
        }
        if (surf->m_Flags.is(CSurface::sf2Sided)){
            for (int k=2; k>=0; k--){
                st_FaceVert& fv = face.pv[k];
	            Fvector& PN = m_PNormals[f_index*3+k];
                int sz;
                if (dwFVF&D3DFVF_XYZ){
                    sz=sizeof(Fvector);
	                VERIFY2(fv.pindex<int(m_Points.size()),"- Face index out of range.");
                    CopyMemory(data,&m_Points[fv.pindex],sz);
                    data+=sz;
                }
                if (dwFVF&D3DFVF_NORMAL){
                    sz=sizeof(Fvector);
                    Fvector N; N.invert(PN);
                    CopyMemory(data,&N,sz);
                    data+=sz;
                }
                sz=sizeof(Fvector2);
				int offs = 0;
                for (int t=0; t<(int)dwTexCnt; t++){
	                VERIFY2((t+offs)<(int)m_VMRefs[fv.vmref].size(),"- VMap layer index out of range");
                    st_VMapPt& vm_pt 	= m_VMRefs[fv.vmref][t];
                    if (m_VMaps[vm_pt.vmap_index]->type!=vmtUV){
                        offs++;
                        t--;
                        continue;
                    }
	                VERIFY2(vm_pt.vmap_index<int(m_VMaps.size()),"- VMap index out of range");
                    st_VMap* vmap		= m_VMaps[vm_pt.vmap_index];
    	            VERIFY2(vm_pt.index<vmap->size(),"- VMap point index out of range");
                    CopyMemory(data,&vmap->getUV(vm_pt.index),sz); data+=sz;

//	                Msg("%3.2f, %3.2f",vmap->getUV(vm_pt.index).x,vmap->getUV(vm_pt.index).y);
                }
            }
        }
    }
}
//----------------------------------------------------
void CEditableMesh::Render(const Fmatrix& parent, CSurface* S)
{
    if (!m_LoadState.is(LS_RBUFFERS)) CreateRenderBuffers();
	// visibility test
	if (!m_Flags.is(flVisible)) return;
	// frustum test
    Fbox bb; bb.set(m_Box);
    bb.xform(parent);
	if (!::Render->occ_visible(bb)) return;
    // render
    RBMapPairIt rb_pair = m_RenderBuffers.find(S);
    if (rb_pair!=m_RenderBuffers.end()){
        RBVector& rb_vec = rb_pair->second;
        for (RBVecIt rb_it=rb_vec.begin(); rb_it!=rb_vec.end(); rb_it++)
            Device.DP(D3DPT_TRIANGLELIST,rb_it->pGeom,0,rb_it->dwNumVertex/3);
    }
}
//----------------------------------------------------
#define MAX_VERT_COUNT 0xFFFF
static Fvector RB[MAX_VERT_COUNT];
static RB_cnt=0;

void CEditableMesh::RenderList(const Fmatrix& parent, u32 color, bool bEdge, IntVec& fl)
{
//	if (!m_Visible) return;
//	if (!m_LoadState.is(LS_RBUFFERS)) CreateRenderBuffers();

	if (fl.size()==0) return;
	RCache.set_xform_world(parent);
	Device.RenderNearer(0.0006);
	RB_cnt = 0;
    if (bEdge){
    	Device.SetShader(Device.m_WireShader);
	    Device.SetRS(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
    }else
    	Device.SetShader(Device.m_SelectionShader);
    for (IntIt dw_it=fl.begin(); dw_it!=fl.end(); dw_it++){
        st_Face& face = m_Faces[*dw_it];
        for (int k=0; k<3; k++)	RB[RB_cnt++].set(m_Points[face.pv[k].pindex]);
		if (RB_cnt==MAX_VERT_COUNT){
        	DU.DrawPrimitiveL(D3DPT_TRIANGLELIST,RB_cnt/3,RB,RB_cnt,color,true,false);
			RB_cnt = 0;
        }
    }

	if (RB_cnt)	DU.DrawPrimitiveL(D3DPT_TRIANGLELIST,RB_cnt/3,RB,RB_cnt,color,true,false);
    if (bEdge)  Device.SetRS(D3DRS_FILLMODE,Device.dwFillMode);
	Device.ResetNearer();
}
//----------------------------------------------------

void CEditableMesh::RenderSelection(const Fmatrix& parent, CSurface* s, u32 color)
{
    if (!m_LoadState.is(LS_RBUFFERS)) CreateRenderBuffers();
//	if (!m_Visible) return;
    Fbox bb; bb.set(m_Box);
    bb.xform(parent);
	if (!::Render->occ_visible(bb)) return;
    // render
	RCache.set_xform_world(parent);
    if (s){
        SurfFacesPairIt sp_it = m_SurfFaces.find(s);
        if (sp_it!=m_SurfFaces.end()) RenderList(parent,color,false,sp_it->second);
    }else{
	    Device.SetRS(D3DRS_TEXTUREFACTOR,	color);
        for (RBMapPairIt p_it=m_RenderBuffers.begin(); p_it!=m_RenderBuffers.end(); p_it++){
            RBVector& rb_vec = p_it->second;
            u32 vBase;
            for (RBVecIt rb_it=rb_vec.begin(); rb_it!=rb_vec.end(); rb_it++)
                Device.DP(D3DPT_TRIANGLELIST,rb_it->pGeom,0,rb_it->dwNumVertex/3);
        }
	    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    }
}
//----------------------------------------------------

void CEditableMesh::RenderEdge(const Fmatrix& parent, CSurface* s, u32 color)
{
    if (!m_LoadState.is(LS_RBUFFERS)) CreateRenderBuffers();
//	if (!m_Visible) return;
	RCache.set_xform_world(parent);
	Device.SetShader(Device.m_WireShader);
	Device.RenderNearer(0.001);

    // render
    Device.SetRS(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
    if (s){
        SurfFacesPairIt sp_it = m_SurfFaces.find(s);
        if (sp_it!=m_SurfFaces.end()) RenderList(parent,color,true,sp_it->second);
    }else{
	    Device.SetRS(D3DRS_TEXTUREFACTOR,	color);
        for (RBMapPairIt p_it=m_RenderBuffers.begin(); p_it!=m_RenderBuffers.end(); p_it++){
            RBVector& rb_vec = p_it->second;
            u32 vBase;
            for (RBVecIt rb_it=rb_vec.begin(); rb_it!=rb_vec.end(); rb_it++)
                Device.DP(D3DPT_TRIANGLELIST,rb_it->pGeom,0,rb_it->dwNumVertex/3);
        }
	    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    }
    Device.SetRS(D3DRS_FILLMODE,Device.dwFillMode);
    Device.ResetNearer();
}
//----------------------------------------------------

#define SKEL_MAX_FACE_COUNT 10000
struct svertRender
{
	Fvector		P;
	Fvector		N;
	Fvector2 	uv;
};
void CEditableMesh::RenderSkeleton(const Fmatrix& parent, CSurface* S)
{
    if (!m_LoadState.is(CEditableMesh::LS_SVERTICES)) GenerateSVertices();

	R_ASSERT2(!m_SVertices.empty(),"SVertices empty!");
	SurfFacesPairIt sp_it 	= m_SurfFaces.find(S); R_ASSERT(sp_it!=m_SurfFaces.end());
    IntVec& face_lst 		= sp_it->second;
	_VertexStream*	Stream	= &RCache.Vertex;
	u32				vBase;

	svertRender*	pv		= (svertRender*)Stream->Lock(SKEL_MAX_FACE_COUNT*3,m_Parent->vs_SkeletonGeom->vb_stride,vBase);
	Fvector			P0,N0,P1,N1;
    
    int f_cnt=0;
    for (IntIt i_it=face_lst.begin(); i_it!=face_lst.end(); i_it++){
        for (int k=0; k<3; k++,pv++){
        	st_SVert& SV = m_SVertices[*i_it*3+k];
            if (SV.bone1!=BI_NONE){
                Fmatrix& M0		= m_Parent->m_Bones[SV.bone0]->_RenderTransform();
                Fmatrix& M1		= m_Parent->m_Bones[SV.bone1]->_RenderTransform();
                M0.transform_tiny(P0,SV.offs);
                M0.transform_dir (N0,SV.norm);
                M1.transform_tiny(P1,SV.offs);
                M1.transform_dir (N1,SV.norm);
                pv->P.lerp		(P0,P1,SV.w);
                pv->N.lerp		(N0,N1,SV.w);
                pv->uv.set		(SV.uv);
            }else{                 
                Fmatrix& M0		= m_Parent->m_Bones[SV.bone0]->_RenderTransform();
                M0.transform_tiny(pv->P,SV.offs);
                M0.transform_dir (pv->N,SV.norm);
                pv->uv.set		(SV.uv);
            }
        }
        f_cnt++;
        if (S->m_Flags.is(CSurface::sf2Sided)){
        	pv->P.set((pv-1)->P);	pv->N.invert((pv-1)->N);	pv->uv.set((pv-1)->uv); pv++;
        	pv->P.set((pv-3)->P);	pv->N.invert((pv-3)->N);	pv->uv.set((pv-3)->uv); pv++;
        	pv->P.set((pv-5)->P);	pv->N.invert((pv-5)->N);	pv->uv.set((pv-5)->uv); pv++;
	        f_cnt++;
        }
        if (f_cnt>=SKEL_MAX_FACE_COUNT-1){
            Stream->Unlock		(f_cnt*3,m_Parent->vs_SkeletonGeom->vb_stride);
            Device.DP			(D3DPT_TRIANGLELIST,m_Parent->vs_SkeletonGeom,vBase,f_cnt);
			pv					= (svertRender*)Stream->Lock(SKEL_MAX_FACE_COUNT*3,m_Parent->vs_SkeletonGeom->vb_stride,vBase);
            f_cnt				= 0;
        }
    }
	Stream->Unlock				(f_cnt*3,m_Parent->vs_SkeletonGeom->vb_stride);
	if (f_cnt)	Device.DP		(D3DPT_TRIANGLELIST,m_Parent->vs_SkeletonGeom,vBase,f_cnt);    
}
//----------------------------------------------------


