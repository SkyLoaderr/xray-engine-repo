//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMeshVLight.h"
#include "EditMesh.h"
#include "EditObject.h"
#include "UI_Main.h"
#include "bottombar.h"
//----------------------------------------------------

void CEditMesh::ClearRenderBuffers(){
	if (!m_RenderBuffers.empty()){
        for (RBMapPairIt rbmp_it=m_RenderBuffers.begin(); rbmp_it!=m_RenderBuffers.end(); rbmp_it++)
    		for(RBVecIt rb_it=rbmp_it->second.begin(); rb_it!=rbmp_it->second.end(); rb_it++)
        		_RELEASE(rb_it->buffer);
	    m_RenderBuffers.clear();
    }
}
//----------------------------------------------------

#define F_LIM (10000)
#define V_LIM (F_LIM*3)

void CEditMesh::UpdateRenderBuffers(LPDIRECT3D7 pD3D,DWORD dwCaps){
	ClearRenderBuffers();
    
	UI->ProgressStart(m_SurfFaces.size(),"Update RB:");

    if (!(m_LoadState&EMESH_LS_PNORMALS)) GeneratePNormals();
    VERIFY(m_PNormals.size());
    
    for (SurfFacesPairIt sp_it=m_SurfFaces.begin(); sp_it!=m_SurfFaces.end(); sp_it++){
		INTVec& face_lst = sp_it->second;
        st_Surface* _S = sp_it->first;
        int num_verts=face_lst.size()*3;
        RBVector rb_vec;
		int v_cnt=num_verts;
        int start_face=0;
        int num_face;
        do{
	        rb_vec.push_back	(st_RenderBuffer(0,(v_cnt<V_LIM)?v_cnt:V_LIM));
            st_RenderBuffer& rb	= rb_vec.back();
            if (_S->sideflag) 	rb.dwNumVertex *= 2;
            num_face			= (v_cnt<V_LIM)?v_cnt/3:F_LIM;
            D3DVERTEXBUFFERDESC desc;
            desc.dwSize 		= sizeof(desc);
            desc.dwCaps 		= dwCaps;
            desc.dwFVF			= _S->dwFVF;
            desc.dwNumVertices 	= rb.dwNumVertex;
            pD3D->CreateVertexBuffer(&desc,&rb.buffer,0);
            void* data;
            rb.buffer->Lock		(DDLOCK_WRITEONLY,&data,0);
			FillRenderBuffer	(face_lst,start_face,num_face,_S,data);
            rb.buffer->Unlock	();
            rb.buffer->Optimize	(UI->Device.GetDevice(),0);

            v_cnt				-= V_LIM;
            start_face			+= (_S->sideflag)?rb.dwNumVertex/6:rb.dwNumVertex/3;
        }while(v_cnt>0);
        if (num_verts>0) m_RenderBuffers.insert(make_pair(_S,rb_vec));
		UI->ProgressInc();
    }
    UnloadPNormals();
	UI->ProgressEnd();
}
//----------------------------------------------------
void CEditMesh::FillRenderBuffer(INTVec& face_lst, int start_face, int num_face, const st_Surface* surf, LPVOID& src_data){
    BYTE* data = (BYTE*)src_data;
    DWORD dwFVF = surf->dwFVF;
	DWORD dwTexCnt = ((dwFVF&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
    for (int fl_i=start_face; fl_i<start_face+num_face; fl_i++){
        DWORD f_index = face_lst[fl_i];
        VERIFY(f_index<m_Faces.size());
    	st_Face& face = m_Faces[f_index];
        for (int k=0; k<3; k++){
            st_FaceVert& fv = face.pv[k];
            DWORD norm_id = f_index*3+k;
	        VERIFY(norm_id<m_PNormals.size());
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
            for (DWORD t=0; t<dwTexCnt; t++){
                VERIFY2(t<m_VMRefs[fv.vmref].size(),"- VMap layer index out of range");
            	st_VMapPt& vm_pt 	= m_VMRefs[fv.vmref][t+offs];
                if (m_VMaps[vm_pt.vmap_index].type!=vmtUV){
                	offs++;
                    t--; 
                    continue;
                }
                VERIFY2(vm_pt.vmap_index<int(m_VMaps.size()),"- VMap index out of range");
				st_VMap& vmap		= m_VMaps[vm_pt.vmap_index];
                VERIFY2(vm_pt.index<vmap.size(),"- VMap point index out of range");
                CopyMemory(data,&vmap.getUV(vm_pt.index),sz); data+=sz;
            }
        }
        if (surf->sideflag){
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
                for (DWORD t=0; t<dwTexCnt; t++){
	                VERIFY2(t<m_VMRefs[fv.vmref].size(),"- VMap layer index out of range");
                    st_VMapPt& vm_pt 	= m_VMRefs[fv.vmref][t];
                    if (m_VMaps[vm_pt.vmap_index].type!=vmtUV){
                        offs++;
                        t--; 
                        continue;
                    }
	                VERIFY2(vm_pt.vmap_index<int(m_VMaps.size()),"- VMap index out of range");
                    st_VMap& vmap		= m_VMaps[vm_pt.vmap_index];
    	            VERIFY2(vm_pt.index<vmap.size(),"- VMap point index out of range");
                    CopyMemory(data,&vmap.getUV(vm_pt.index),sz); data+=sz;
                }
            }
        }
    }
}
//----------------------------------------------------

void CEditMesh::Render(const Fmatrix& parent, st_Surface* S){
	// visibility test
    if (!m_Visible) return;
	// frustum test 
	Fvector C; float r; 
    Fbox bb; bb.set(m_Box);
    bb.transform(parent);
    bb.getsphere(C,r);
	if (!UI->Device.m_Frustum.testSphere(C,r)) return;
    // render
	RBVector& rb_vec = m_RenderBuffers.find(S)->second;
    for (RBVecIt rb_it=rb_vec.begin(); rb_it!=rb_vec.end(); rb_it++)
		UI->Device.DPVB(D3DPT_TRIANGLELIST, rb_it->buffer, rb_it->dwStartVertex, rb_it->dwNumVertex);
}
//----------------------------------------------------
struct st_RenderVertex{
	Fvector point;
    DWORD	clr;
    IC void set(Fvector& v, DWORD c){point.set(v);clr=c;}
};

#define MAX_VERT_COUNT 0x5208
static st_RenderVertex RB[MAX_VERT_COUNT];
static RB_cnt=0;

void CEditMesh::RenderList(const Fmatrix& parent, DWORD color, bool bEdge, DWORDVec& fl)
{
	if (!m_Visible) return;

	if (fl.size()==0) return;
	UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,parent);
	UI->Device.RenderNearer(0.0006);
	RB_cnt = 0;
    if (bEdge){	
    	UI->Device.Shader.Set(UI->Device.m_WireShader);
	    UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,D3DFILL_WIREFRAME);
    }else		
    	UI->Device.Shader.Set(UI->Device.m_SelectionShader);
    for (DWORDIt dw_it=fl.begin(); dw_it!=fl.end(); dw_it++){
        st_Face& face = m_Faces[*dw_it];
        for (int k=0; k<3; k++)	RB[RB_cnt++].set(m_Points[face.pv[k].pindex],color);
		if (RB_cnt==MAX_VERT_COUNT){
            UI->Device.DP(D3DPT_TRIANGLELIST, FVF::F_L, RB, RB_cnt);
			RB_cnt = 0;
        }
    }
	if (RB_cnt)	UI->Device.DP(D3DPT_TRIANGLELIST, FVF::F_L, RB, RB_cnt);
    if (bEdge)  UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,UI->dwRenderFillMode);
	UI->Device.ResetNearer();
}
//----------------------------------------------------

void CEditMesh::RenderEdge(Fmatrix& parent, DWORD color){
	if (!m_Visible) return;
    
	UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,parent);
	UI->Device.Shader.Set(UI->Device.m_WireShader);
	UI->Device.RenderNearer(0.0005);

    RB_cnt = 0;
    UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,D3DFILL_WIREFRAME);
    for (FaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
        for (int k=0; k<3; k++)	RB[RB_cnt++].set(m_Points[f_it->pv[k].pindex],color);
        if (RB_cnt==MAX_VERT_COUNT){
            UI->Device.DP(D3DPT_TRIANGLELIST, FVF::F_L, RB, RB_cnt);
			RB_cnt = 0;
        }
    }
    if (RB_cnt)	UI->Device.DP(D3DPT_TRIANGLELIST, FVF::F_L, RB, RB_cnt);

    UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,UI->dwRenderFillMode);
    UI->Device.ResetNearer();
}
//----------------------------------------------------

void CEditMesh::RenderSelection(Fmatrix& parent, DWORD color){
	if (!m_Visible) return;

	Fvector C; float r; 
    Fbox bb; bb.set(m_Box);
    bb.transform(parent);
    bb.getsphere(C,r);
	if (!UI->Device.m_Frustum.testSphere(C,r)) return;
//	RBVector& rb_vec = m_RenderBuffers.find(S)->second;
//    for (RBVecIt rb_it=rb_vec.begin(); rb_it!=rb_vec.end(); rb_it++)
//		UI->Device.DPVB(D3DPT_TRIANGLELIST, rb_it->buffer, rb_it->dwStartVertex, rb_it->dwNumVertex);
    RB_cnt = 0;
    for (FaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
        for (int k=0; k<3; k++)	RB[RB_cnt++].set(m_Points[f_it->pv[k].pindex],color);
        if (RB_cnt==MAX_VERT_COUNT){
            UI->Device.DP(D3DPT_TRIANGLELIST, FVF::F_L, RB, RB_cnt);
			RB_cnt = 0;
        }
    }
    if (RB_cnt)	UI->Device.DP(D3DPT_TRIANGLELIST, FVF::F_L, RB, RB_cnt);
}

 
