//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMeshVLight.h"
#include "EditMesh.h"
#include "EditObject.h"
#include "bottombar.h"
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

    UI.ProgressStart((float)m_SurfFaces.size(),"Loading RB...");

    VERIFY(m_PNormals.size());

	UI.Command(COMMAND_EVICT_TEXTURES);

    for (SurfFacesPairIt sp_it=m_SurfFaces.begin(); sp_it!=m_SurfFaces.end(); sp_it++){
		IntVec& face_lst = sp_it->second;
        CSurface* _S = sp_it->first;
        int num_verts=face_lst.size()*3;
        RBVector rb_vec;
		int v_cnt=num_verts;
        int start_face=0;
        int num_face;
        do{
	        rb_vec.push_back	(st_RenderBuffer(0,(v_cnt<V_LIM)?v_cnt:V_LIM));
            st_RenderBuffer& rb	= rb_vec.back();
            if (_S->m_Flags.is(CSurface::sf2Sided)) 	rb.dwNumVertex *= 2;
            num_face			= (v_cnt<V_LIM)?v_cnt/3:F_LIM;

            int buf_size		= D3DXGetFVFVertexSize(_S->_FVF())*rb.dwNumVertex;
			u8*	bytes			= 0;
			R_CHK(HW.pDevice->CreateVertexBuffer(buf_size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &rb.pVB, 0));
			rb.pGeom	 		= Device.Shader.CreateGeom(_S->_FVF(),rb.pVB,0);

			R_CHK				(rb.pVB->Lock(0,0,(LPVOID*)&bytes,0));
			FillRenderBuffer	(face_lst,start_face,num_face,_S,bytes);
			rb.pVB->Unlock		();

            v_cnt				-= V_LIM;
            start_face			+= (_S->m_Flags.is(CSurface::sf2Sided))?rb.dwNumVertex/6:rb.dwNumVertex/3;
        }while(v_cnt>0);
        if (num_verts>0) m_RenderBuffers.insert(make_pair(_S,rb_vec));
		UI.ProgressInc();
    }
    UnloadPNormals();
    m_LoadState.set(LS_RBUFFERS,TRUE);
    UI.ProgressEnd();
}
//----------------------------------------------------

void CEditableMesh::ClearRenderBuffers()
{
    for (RBMapPairIt rbmp_it=m_RenderBuffers.begin(); rbmp_it!=m_RenderBuffers.end(); rbmp_it++){
        for(RBVecIt rb_it=rbmp_it->second.begin(); rb_it!=rbmp_it->second.end(); rb_it++){
			Device.Shader.DeleteGeom(rb_it->pGeom);
			_RELEASE(rb_it->pVB);
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
            u32 norm_id = f_index*3+k;
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
    if (!m_Visible) return;
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

void CEditableMesh::RenderList(const Fmatrix& parent, u32 color, bool bEdge, U32Vec& fl)
{
//	if (!m_Visible) return;
    if (!m_LoadState.is(LS_RBUFFERS)) CreateRenderBuffers();

	if (fl.size()==0) return;
	RCache.set_xform_world(parent);
	Device.RenderNearer(0.0006);
	RB_cnt = 0;
    if (bEdge){
    	Device.SetShader(Device.m_WireShader);
	    Device.SetRS(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
    }else
    	Device.SetShader(Device.m_SelectionShader);
    for (U32It dw_it=fl.begin(); dw_it!=fl.end(); dw_it++){
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

void CEditableMesh::RenderEdge(const Fmatrix& parent, u32 color){
    if (!m_LoadState.is(LS_RBUFFERS)) CreateRenderBuffers();
//	if (!m_Visible) return;
	RCache.set_xform_world(parent);
	Device.SetShader(Device.m_WireShader);
	Device.RenderNearer(0.001);

    // render
    Device.SetRS(D3DRS_TEXTUREFACTOR,	color);
    Device.SetRS(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
    for (RBMapPairIt p_it=m_RenderBuffers.begin(); p_it!=m_RenderBuffers.end(); p_it++){
		RBVector& rb_vec = p_it->second;
	    u32 vBase;
    	for (RBVecIt rb_it=rb_vec.begin(); rb_it!=rb_vec.end(); rb_it++)
            Device.DP(D3DPT_TRIANGLELIST,rb_it->pGeom,0,rb_it->dwNumVertex/3);
    }
    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    Device.SetRS(D3DRS_FILLMODE,Device.dwFillMode);
    Device.ResetNearer();
}
//----------------------------------------------------

void CEditableMesh::RenderSelection(const Fmatrix& parent, u32 color){
    if (!m_LoadState.is(LS_RBUFFERS)) CreateRenderBuffers();
//	if (!m_Visible) return;
    Fbox bb; bb.set(m_Box);
    bb.xform(parent);
	if (!::Render->occ_visible(bb)) return;
    // render
	RCache.set_xform_world(parent);
    Device.SetRS(D3DRS_TEXTUREFACTOR,	color);
    for (RBMapPairIt p_it=m_RenderBuffers.begin(); p_it!=m_RenderBuffers.end(); p_it++){
		RBVector& rb_vec = p_it->second;
	    u32 vBase;
    	for (RBVecIt rb_it=rb_vec.begin(); rb_it!=rb_vec.end(); rb_it++)
            Device.DP(D3DPT_TRIANGLELIST,rb_it->pGeom,0,rb_it->dwNumVertex/3);
    }
    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
}

