//----------------------------------------------------
// file: BuilderRModel.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "fmesh.h"
#include "std_classes.h"
#include "SkeletonFace.h"
#include "progmesh.h"
#include "bone.h"
#include "motion.h"
#include "xr_ini.h"
#include "MgcCont3DMinBox.h"

//----------------------------------------------------
struct st_SPLIT
{
	vSVERT			Vlist;
	vSFACE			Flist;

	WORDVec			Indices;		// valid only after call to MakeValidToRender
	
	DWORD			mtl;
	DWORD			dwBonesPerVert;
	Fbox			bb;

	// Progressive
	int				I_Current;
	int				V_Minimal;
	std::vector<Vsplit>	pmap_vsplit;
	std::vector<WORD>	pmap_faces;

	st_SVERT*	AddVert(st_SVERT* pTestV){
		for (SVERTIt vI=Vlist.begin(); vI!=Vlist.end(); vI++)
			if ((*vI)->similar(*pTestV)) return *vI;

		st_SVERT* V = new st_SVERT;
		*V		= *pTestV;
		Vlist.push_back(V);
		return V;
	}
	void	MakeValidToRender()	{
		// correct vertices and copy them to local list
		for(SFACEIt FI=Flist.begin(); FI!=Flist.end(); FI++){
			st_SFACE*	F = *FI;
			for (int i=0; i<3; i++)
				F->v[i] = AddVert(F->v[i]);
		}

		// Progressive
		I_Current=V_Minimal=0;
		for (SFACEIt F=Flist.begin(); F!=Flist.end(); F++){
			st_SFACE* pF = *F;
			for (int i=0; i<3; i++) {
				SVERTIt	found	= std::find(Vlist.begin(),Vlist.end(),pF->v[i]);
				R_ASSERT	(found!=Vlist.end());
				DWORD		ID		= found - Vlist.begin();
				Indices.push_back	(WORD(ID));
			}
		}
	}
	void	MakeProgressive(){
		if (Flist.size()>1) {
			// Options
			PM_Options(1,1,4,0.1f,1,1,120,0.15f,0.95f);
			
			// Transfer vertices
			for (SVERTIt V=Vlist.begin(); V!=Vlist.end(); V++){
				st_SVERT		&iV = **V;
				PM_CreateVertex(iV.P.x,iV.P.y,iV.P.z,V - Vlist.begin(),(P_UV*)(&iV.uv[0]));
			}
			
			// Convert
			PM_Result R;
			I_Current = PM_Convert(Indices.begin(),Indices.size(), &R);
			if (I_Current>=0) {
				// Permute vertices
				vSVERT temp_list = Vlist;
				
				// Perform permutation
				for(DWORD i=0; i<temp_list.size(); i++)
					Vlist[R.permutePTR[i]]=temp_list[i];
				
				// Copy results
				pmap_vsplit.resize(R.splitSIZE);
				CopyMemory(pmap_vsplit.begin(),R.splitPTR,R.splitSIZE*sizeof(Vsplit));
				
				pmap_faces.resize(R.facefixSIZE);
				CopyMemory(pmap_faces.begin(),R.facefixPTR,R.facefixSIZE*sizeof(WORD));
				
				V_Minimal = R.minVertices;
			}
		}
	}

	void Save(char *name)
	{
		CFS_File F(name);

		// Header
		F.open_chunk		(OGF_HEADER);
		ogf_header			H;
		H.format_version	= xrOGF_FormatVersion;
		H.type				= (I_Current>=0)?MT_SKELETON_PART:MT_SKELETON_PART_STRIPPED;
		H.flags				= 0;
		F.write				(&H,sizeof(H));
		F.close_chunk		();
		
		// Texture
		F.open_chunk(OGF_TEXTURE);
        b_material& M 	= Builder->l_materials[mtl];
        b_shader& B 	= Builder->l_shaders[M.shader];
        R_ASSERT(M.dwTexCount);
        b_texture& T 	= Builder->l_textures[M.surfidx[0]];
		AnsiString Tname= AnsiString(T.name);
		F.WstringZ		(Tname.c_str());
		Tname 			= AnsiString(B.name);
		F.WstringZ		(Tname.c_str());
		F.close_chunk();
		
		// Vertices
		bb.invalidate();
		F.open_chunk(OGF_VERTICES);
		F.Wdword(0x12071980);
		F.Wdword(Vlist.size());
		for (SVERTIt V=Vlist.begin(); V!=Vlist.end(); V++){
			st_SVERT* pV = *V;
			bb.modify(pV->P);
			F.write(&(pV->O),sizeof(float)*3);		// position (offset)
			F.write(&(pV->N),sizeof(float)*3);		// normal
			F.Wfloat(pV->uv[0].x); F.Wfloat(pV->uv[0].y);		// tu,tv
			F.Wdword(pV->bone);
		}
		F.close_chunk();
		
		// Faces
		F.open_chunk(OGF_INDICES);
		F.Wdword(Indices.size());
		F.write(Indices.begin(),Indices.size()*sizeof(WORD));
		F.close_chunk();
		
		// PMap
		if (I_Current>=0) {
			F.open_chunk(OGF_P_MAP);
			{
				F.open_chunk(0x1);
				F.Wdword(V_Minimal);
				F.Wdword(I_Current);
				F.close_chunk();
			}
			{
				F.open_chunk(0x2);
				F.write(pmap_vsplit.begin(),pmap_vsplit.size()*sizeof(Vsplit));
				F.close_chunk();
			}
			{
				F.open_chunk(0x3);
				F.Wdword(pmap_faces.size());
				F.write(pmap_faces.begin(),pmap_faces.size()*sizeof(WORD));
				F.close_chunk();
			}
			F.close_chunk();
		}
		
		// BBox (already computed)
		F.open_chunk(OGF_BBOX);
		F.write(&bb,sizeof(Fvector)*2);
		F.close_chunk();
	}
};
//----------------------------------------------------
DEFINE_VECTOR(st_SPLIT,vSPLIT,SPLITIt);
static vSPLIT 	g_splits;

static vSVERT	g_verts;
static vSFACE	g_faces;
static vSVERT	g_reg_verts;
static vSFACE	g_reg_faces;

void RegisterSVERT(st_SVERT* v){g_reg_verts.push_back(v);}
void RegisterSFACE(st_SFACE* f){g_reg_faces.push_back(f);}

static void sfinalize(){
	for (SVERTIt v_it=g_reg_verts.begin(); v_it!=g_reg_verts.end(); v_it++) { _DELETE(*v_it); }
	for (SFACEIt f_it=g_reg_faces.begin(); f_it!=g_reg_faces.end(); f_it++) { _DELETE(*f_it); }
	g_verts.clear();
	g_faces.clear();
	g_splits.clear();
    g_reg_verts.clear();
    g_reg_faces.clear();
}
//----------------------------------------------------
void ComputeOBB	(Fobb &B, FvectorVec& V){
	if (V.size()<3) {
		B.invalidate();
		return;
	}
	Mgc::Box3	BOX		= Mgc::MinBox(V.size(), (const Mgc::Vector3*) V.begin());
	Mgc::Vector3& R		= BOX.Axis(0);
	Mgc::Vector3& N		= BOX.Axis(1);
	Mgc::Vector3& D		= BOX.Axis(2);
	Mgc::Vector3& T		= BOX.Center();
	float* S			= BOX.Extents();
	B.m_rotate.i.set	(R.x,R.y,R.z);
	B.m_rotate.j.set	(N.x,N.y,N.z);
	B.m_rotate.k.set	(D.x,D.y,D.z);
	B.m_translate.set	(T.x,T.y,T.z);
	B.m_halfsize.set	(S[0],S[1],S[2]);
}
bool SceneBuilder::SaveObjectSkeletonLTX(const char* name, CEditObject* obj){
	FS.DeleteFileByName(name);
	CInifile ini(name,false);
    ini.WriteString("general",0,0,"general params");

    // temporary partition
    ini.WriteString("partition",0,0,"list of partitions");
    ini.WriteString("partition","all",0);
    for (BoneIt b_it=obj->FirstBone(); b_it!=obj->LastBone(); b_it++)
	    ini.WriteString("all",(*b_it)->Name(),0);

    ini.WriteString("cycle",0,0,"cycle motions");
    ini.WriteString("fx",0,0,"fx motions");
    for (SMotionIt m_it=obj->FirstSMotion(); m_it!=obj->LastSMotion(); m_it++){
    	CSMotion* M = *m_it;
        AnsiString sect = AnsiString(M->Name());
        ini.WriteString(M->bFX?"fx":"cycle",sect.c_str(),sect.c_str());
        ini.WriteString(sect.c_str(),"stop@end",M->bStopAtEnd?"on":"off");
        ini.WriteString(sect.c_str(),"bone",(M->cStartBone[0])?M->cStartBone:"ROOT");
        ini.WriteString(sect.c_str(),"part",(M->cBonePart[0])?M->cBonePart:"--none--");
        ini.WriteString(sect.c_str(),"motion",sect.c_str());
        ini.WriteFloat(sect.c_str(),"speed",M->fSpeed);
        ini.WriteFloat(sect.c_str(),"accrue",M->fAccrue);
        ini.WriteFloat(sect.c_str(),"falloff",M->fFalloff);
        ini.WriteFloat(sect.c_str(),"power",M->fPower);
    }

    return true;
}

const float KEY_Quant	= 32767.f;
bool SceneBuilder::SaveObjectSkeletonOGF(const char* fn, CEditObject* O){
    UI->ProgressStart(10,"Export skeleton...");
    UI->ProgressInc();

	vector<FvectorVec>	bone_points;

	O->ResetAnimation();

	CEditObject* obj = (O->IsReference())?O->GetRef():O;
	if( obj->MeshCount() == 0 ) return false;

    ResetStructures();
    AssembleSkeletonObject(obj);

    // fill faces&vertices
    Fbox BB;
    BB.invalidate();
    int i;
	bone_points.resize(obj->BoneCount());
    for (i=0; i<l_faces_cnt; i++){
        b_face*	 gF	= l_faces  + i;
        st_SFACE* nF= new st_SFACE;
        nF->m		= gF->dwMaterial;

        for (int k=0; k<3; k++){
            int vert_id 	= gF->v[k];

            st_SVERT* v		= new st_SVERT; 
            v->P.set		(l_vertices[vert_id]); 
            v->O.set		(l_svertices[vert_id].offs);
            v->SetBone		(l_svertices[vert_id].bone);
            v->AppendUV		(gF->t[0][k].tu,gF->t[0][k].tv);
            v->N.set		(l_vnormals[vert_id]);
			CBone* B		= obj->GetBone(v->bone);
            B->LITransform().transform_dir(v->N);

            g_verts.push_back(v);

            nF->VSet(k,v);

            bone_points[l_svertices[vert_id].bone].push_back(l_svertices[vert_id].offs);
        }
        g_faces.push_back(nF);
    }
    UI->ProgressInc();

    // create OGF
    // materials
    g_splits.resize(l_materials.size());
	for (i=0; i<int(g_splits.size()); i++) g_splits[i].mtl=i;

	// Collapse vertices
    int sz = g_verts.size();
	for (i=0; i<int(g_verts.size()); i++){
		st_SVERT* from = g_verts[i];
		for (int j=i+1; j<int(g_verts.size()); j++){
			if (from->similar(*(g_verts[j]))){
				// replace vertex J by I
				st_SVERT* to = g_verts[j];
				for (SFACEIt fI=g_faces.begin(); fI!=g_faces.end(); fI++){
					(*fI)->ReplaceVert(from,to);
				}
				g_verts.erase(g_verts.begin()+j);
				j--;
			}
		}
	}
    UI->ProgressInc();
    
    // converting face&vertices
	for (SFACEIt f_it=g_faces.begin(); f_it!=g_faces.end(); f_it++){
    	st_SFACE* F = *f_it;
		g_splits[F->m].Flist.push_back(F);
	}
    UI->ProgressInc();

	// VB+IB+PMesh...
	for (i=0; i<int(g_splits.size()); i++){
		g_splits[i].MakeValidToRender();
		g_splits[i].MakeProgressive();
	}
    UI->ProgressInc();

	// Saving geometry...
	AnsiString	root_name,base_name;
	{
		char drive	[_MAX_DRIVE];
		char dir	[_MAX_DIR];
		char fname	[_MAX_FNAME];
		char ext	[_MAX_EXT];
		_splitpath	( fn, drive, dir, fname, ext );
		base_name	= fname;
		root_name	= AnsiString(drive)+AnsiString(dir)+base_name;
	}
	Fbox	rootBB; rootBB.invalidate();
    
	CFS_File F((root_name+".ogf").c_str());

	// Header
	F.open_chunk(OGF_HEADER);
	ogf_header H;
	H.format_version	= xrOGF_FormatVersion;
	H.type				= MT_SKELETON;
	H.flags				= 0;
	F.write(&H,sizeof(H));
	F.close_chunk();

	// OGF_CHIELDS
	F.open_chunk	(OGF_CHIELDS);
	F.Wdword		(g_splits.size());
	for (i=0; i<int(g_splits.size()); i++){
		char N[MAX_PATH];
		sprintf(N,"%s#%d.ogf",root_name.c_str(),i);
		g_splits[i].Save(N);

		rootBB.merge(g_splits[i].bb);
		sprintf(N,"%s#%d.ogf",base_name.c_str(),i);
		F.WstringZ(N);
	}
	F.close_chunk();
    UI->ProgressInc();

	// BBox (already computed)
	F.open_chunk(OGF_BBOX);
	F.write(&rootBB,sizeof(Fvector)*2);
	F.close_chunk();

	// BoneNames
	F.open_chunk(OGF_BONE_NAMES);
	F.Wdword(obj->BoneCount());
    i=0;
    for (BoneIt b_it=obj->FirstBone(); b_it!=obj->LastBone(); b_it++,i++){
		F.WstringZ	((*b_it)->Name());
		F.WstringZ	(((*b_it)->ParentIndex()==-1)?"":(*b_it)->Parent());
		Fobb	obb;
		ComputeOBB	(obb,bone_points[i]);
		F.write	(&obb,sizeof(Fobb));
	}
	F.close_chunk();
    UI->ProgressInc();

	// Motions
	F.open_chunk(OGF_MOTIONS);
    F.open_chunk(0);
    F.Wdword(obj->SMotionCount());
	F.close_chunk();
    int smot = 1;
    for (SMotionIt m_it=obj->FirstSMotion(); m_it!=obj->LastSMotion(); m_it++, smot++){
    	CSMotion* M = *m_it;
	    F.open_chunk(smot);
    	F.WstringZ(M->Name());
        F.Wdword(M->Length());
        BoneMotionVec& lst=M->BoneMotions();
        int bone_id = 0;
        for (BoneMotionIt bm_it=lst.begin(); bm_it!=lst.end(); bm_it++,bone_id++){
			DWORD flag = M->GetMotionFlag(bone_id);
        	CBone* B = obj->GetBone(bone_id);
            int parent_idx = B->ParentIndex();
            for (int frm=M->FrameStart(); frm<M->FrameEnd(); frm++){
                float t = (float)frm/M->FPS();
                Fvector T,R;
                Fmatrix mat;
                Fquaternion q;
                M->Evaluate	(bone_id,t,T,R);

                if (flag&WORLD_ORIENTATION){
			        Fmatrix 	parent;
			        Fmatrix 	inv_parent;
                	if(parent_idx>-1){
			            obj->GetBoneWorldTransform(parent_idx,t,M,parent);
        	            inv_parent.invert(parent);
                 	}else{
                		parent 		= precalc_identity;
	                    inv_parent	= precalc_identity;
    	            }
                	Fmatrix 	rot;
                    rot.setHPB	(-R.x,-R.y,R.z);
//                    Log->Msg(mtInformation,"#%d - HPB: %3.2f, %3.2f, %3.2f",frm,-R.x,-R.y,R.z);
//                    Log->Msg(mtInformation,"#%d - PARENT: [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f]",frm,
//                    			parent.i.x,parent.i.y,parent.i.z,parent._14_,
//                    			parent.j.x,parent.j.y,parent.j.z,parent._24_,
//                    			parent.k.x,parent.k.y,parent.k.z,parent._34_,
//                    			parent.c.x,parent.c.y,parent.c.z,parent._44_);
                    mat.mul	(inv_parent,rot);
//	                mat.setHPB	(1.57f,0,0);
                }else{
	                mat.setHPB	(-R.x,-R.y,R.z);
                }

				q.set		(mat);
        		// Quantize quaternion
	        	int	_x = int(q.x*KEY_Quant); clamp(_x,-32767,32767); short x =  _x; F.write(&x,2);
    		    int	_y = int(q.y*KEY_Quant); clamp(_y,-32767,32767); short y =  _y; F.write(&y,2);
    		    int	_z = int(q.z*KEY_Quant); clamp(_z,-32767,32767); short z =  _z; F.write(&z,2);
	        	int	_w = int(q.w*KEY_Quant); clamp(_w,-32767,32767); short w =  _w; F.write(&w,2);
                F.Wvector(T);

/*
                if (flag&WORLD_ORIENTATION){
                    R.setHPB(-r.x,-r.y,r.z);
                    M.identity();
                    M.c.set	((*b_it)->Offset());
                    M.mul2	(parent);
                    M.i.set	(R.i);
                    M.j.set	(R.j);
                    M.k.set	(R.k);
                    M.mul	(inv_parent);
                }else{
                    M.setHPB(-r.x,-r.y,r.z);
                    M.c.set((*b_it)->Offset());
                }
*/
            }
        }
		F.close_chunk();
    }
	F.close_chunk();
    UI->ProgressInc();

	WriteTextures();
    UI->ProgressInc();
    
	sfinalize();
    ResetStructures();
    
    UI->ProgressInc();
	UI->ProgressEnd();
    
    return true;
}
//----------------------------------------------------

void SceneBuilder::AssembleSkeletonObject(CEditObject* obj){
	VERIFY(obj->IsDynamic());
// compute vertex/face count    
	l_faces_cnt		+= obj->GetFaceCount();
    l_vertices_cnt  += obj->GetVertexCount();
	l_faces			= new b_face[l_faces_cnt];
	l_vertices		= new b_vertex[l_vertices_cnt];
    for(EditMeshIt M=obj->FirstMesh();M!=obj->LastMesh();M++)
    	AssembleSkeletonMesh(*M);
}

//------------------------------------------------------------------------------
// CEditObject build functions
//------------------------------------------------------------------------------
void SceneBuilder::AssembleSkeletonMesh(CEditMesh* mesh){
    int point_offs=l_vertices_it;  // save offset
    
    // fill vertices
    const Fmatrix& T = mesh->m_Parent->GetTransform();
	for (FvectorIt pt_it=mesh->m_Points.begin(); pt_it!=mesh->m_Points.end(); pt_it++)
    	T.transform_tiny(l_vertices[l_vertices_it++],*pt_it);

    // if object skeleton - fill skeleton data
    if (mesh->m_Parent->IsSkeleton()){
        if (!(mesh->m_LoadState&EMESH_LS_SVERTICES)) mesh->GenerateSVertices();
        l_svertices.insert(l_svertices.begin(),mesh->m_SVertices.begin(),mesh->m_SVertices.end());
        mesh->UnloadSVertices();
        for (SVertIt sv_it=l_svertices.begin(); sv_it!=l_svertices.end(); sv_it++)
        	T.transform_tiny(sv_it->offs);
    }

    // generate normals            
    if (!(mesh->m_LoadState&EMESH_LS_FNORMALS)) mesh->GenerateFNormals();
    Fvector N;
    for(FvectorIt pt=mesh->m_Points.begin();pt!=mesh->m_Points.end();pt++){
        N.set(0,0,0);
        INTVec& a_lst = mesh->m_Adjs[pt-mesh->m_Points.begin()];
        VERIFY(a_lst.size());
        for (INTIt i_it=a_lst.begin(); i_it!=a_lst.end(); i_it++)
            N.add(mesh->m_FNormals[*i_it]);
        N.normalize_safe();
        T.transform_dir(N);
        l_vnormals.push_back(N);
    }
    // unload mesh normals
    mesh->UnloadFNormals();
    
//    if (!(mesh->m_LoadState&EMESH_LS_PNORMALS)) mesh->GeneratePNormals();
//    l_vnormals.insert(l_vnormals.end(),mesh->m_PNormals.begin(),mesh->m_PNormals.end());
    // unload mesh normals
//    mesh->UnloadFNormals();
//    mesh->UnloadPNormals();

    // fill faces
    for (SurfFacesPairIt sp_it=mesh->m_SurfFaces.begin(); sp_it!=mesh->m_SurfFaces.end(); sp_it++){
		INTVec& face_lst = sp_it->second;
        st_Surface* surf = sp_it->first;
		DWORD dwTexCnt = ((surf->dwFVF&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
	    for (INTIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
			st_Face& face = mesh->m_Faces[*f_it];
        	{
                b_face& new_face = l_faces[l_faces_it++];
                new_face.dwMaterial = BuildMaterial(mesh,surf,0);
                for (int k=0; k<3; k++){
                    st_FaceVert& fv = face.pv[k];
                    // vertex index
                    new_face.v[k]=point_offs+fv.pindex;
                    // uv maps
                    int offs = 0;
                    for (DWORD t=0; t<dwTexCnt; t++){
                        st_VMapPt& vm_pt 	= mesh->m_VMRefs[fv.vmref][t+offs];
                        st_VMap& vmap		= mesh->m_VMaps[vm_pt.vmap_index];
                        if (vmap.type!=vmtUV){ offs++; t--; continue; }
                        Fvector2& uv		= vmap.getUV(vm_pt.index);
                        new_face.t[t][k].tu = uv.x;
                        new_face.t[t][k].tv = uv.y;
                    }
                }
            }
            
	        if (surf->sideflag){
                b_face& new_face = l_faces[l_faces_it++];
                new_face.dwMaterial = l_faces[l_faces_it-2].dwMaterial;
                for (int k=0; k<3; k++){
                    st_FaceVert& fv = face.pv[2-k];
                    // vertex index
                    new_face.v[k]=point_offs+fv.pindex;
                    // uv maps
                    int offs = 0;
                    for (DWORD t=0; t<dwTexCnt; t++){
                        st_VMapPt& vm_pt 	= mesh->m_VMRefs[fv.vmref][t+offs];
                        st_VMap& vmap		= mesh->m_VMaps[vm_pt.vmap_index];
                        if (vmap.type!=vmtUV){ offs++; t--; continue; }
                        Fvector2& uv		= vmap.getUV(vm_pt.index);
                        new_face.t[t][k].tu = uv.x;
                        new_face.t[t][k].tv = uv.y;
                    }
                }
            }
        }
    }
}

