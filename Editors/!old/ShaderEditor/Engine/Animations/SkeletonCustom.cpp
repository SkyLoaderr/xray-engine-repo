//---------------------------------------------------------------------------
#include 	"stdafx.h"
#pragma hdrstop

#include 	"SkeletonCustom.h"
#include	"SkeletonX.h"
#include	"fmesh.h"
#include	"Render.h"

int			psSkeletonUpdate	= 32;

//////////////////////////////////////////////////////////////////////////
// BoneInstance methods
void		CBoneInstance::construct	()
{
	ZeroMemory					(this,sizeof(*this));
	mTransform.identity			();
	mRenderTransform.identity	();
	Callback_overwrite			= FALSE;
}
void		CBoneInstance::set_callback	(BoneCallback C, void* Param, BOOL overwrite)
{	
	Callback			= C; 
	Callback_Param		= Param; 
	Callback_overwrite	= overwrite;
}
void		CBoneInstance::set_param	(u32 idx, float data)
{
	VERIFY		(idx<MAX_BONE_PARAMS);
	param[idx]	= data;
}
float		CBoneInstance::get_param	(u32 idx)
{
	VERIFY		(idx<MAX_BONE_PARAMS);
	return		param[idx];
}

#ifdef	DEBUG
void	CBoneData::DebugQuery		(BoneDebug& L)
{
	for (u32 i=0; i<children.size(); i++)
	{
		L.push_back(SelfID);
		L.push_back(children[i]->SelfID);
		children[i]->DebugQuery(L);
	}
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool	pred_N(const std::pair<shared_str,u32>&	N, LPCSTR B)			{
	return xr_strcmp(*N.first,B)<0;
}
u16		CKinematics::LL_BoneID		(LPCSTR B)			{
	accel::iterator I	= std::lower_bound	(bone_map_N->begin(),bone_map_N->end(),B,pred_N);
	if (I == bone_map_N->end())			return BI_NONE;
	if (0 != xr_strcmp(*(I->first),B))	return BI_NONE;
	return				u16(I->second);
}
bool	pred_P(const std::pair<shared_str,u32>&	N, const shared_str& B)	{
	return N.first._get() < B._get();
}
u16		CKinematics::LL_BoneID		(const shared_str& B)	{
	accel::iterator I	= std::lower_bound	(bone_map_P->begin(),bone_map_P->end(),B,pred_P);
	if (I == bone_map_P->end())			return BI_NONE;
	if (I->first._get() != B._get() )	return BI_NONE;
	return				u16(I->second);
}

//
LPCSTR CKinematics::LL_BoneName_dbg	(u16 ID)
{
	CKinematics::accel::iterator _I, _E=bone_map_N->end();
	for (_I	= bone_map_N->begin(); _I!=_E; ++_I)	if (_I->second==ID) return *_I->first;
	return 0;
}

#ifdef DEBUG
void CKinematics::DebugRender(Fmatrix& XFORM)
{
	CalculateBones	();

	CBoneData::BoneDebug	dbgLines;
	(*bones)[iRoot]->DebugQuery	(dbgLines);

	Fvector Z;  Z.set(0,0,0);
	Fvector H1; H1.set(0.01f,0.01f,0.01f);
	Fvector H2; H2.mul(H1,2);
	for (u32 i=0; i<dbgLines.size(); i+=2)	{
		Fmatrix& M1 = bone_instances[dbgLines[i]].mTransform;
		Fmatrix& M2 = bone_instances[dbgLines[i+1]].mTransform;

		Fvector P1,P2;
		M1.transform_tiny(P1,Z);
		M2.transform_tiny(P2,Z);
		RCache.dbg_DrawLINE(XFORM,P1,P2,D3DCOLOR_XRGB(0,255,0));

		Fmatrix M;
		M.mul_43(XFORM,M2);
		RCache.dbg_DrawOBB(M,H1,D3DCOLOR_XRGB(255,255,255));
		RCache.dbg_DrawOBB(M,H2,D3DCOLOR_XRGB(255,255,255));
	}

	for (u32 b=0; b<bones->size(); b++)
	{
		Fobb&		obb		= (*bones)[b]->obb;
		Fmatrix&	Mbone	= bone_instances[b].mTransform;
		Fmatrix		Mbox;	obb.xform_get(Mbox);
		Fmatrix		X;		X.mul(Mbone,Mbox);
		Fmatrix		W;		W.mul(XFORM,X);
		RCache.dbg_DrawOBB(W,obb.m_halfsize,D3DCOLOR_XRGB(0,0,255));
	}
}
#endif

CKinematics::~CKinematics	()
{
	IBoneInstances_Destroy	();
}

void	CKinematics::IBoneInstances_Create()
{
	// VERIFY2				(bones->size() < 64, "More than 64 bones is a crazy thing!");
	u32				size	= bones->size();
	void*			ptr		= xr_malloc(size*sizeof(CBoneInstance));
	bone_instances			= (CBoneInstance*)ptr;
	for (u32 i=0; i<size; i++)
		bone_instances[i].construct();
}

void	CKinematics::IBoneInstances_Destroy()
{
	if (bone_instances) {
		xr_free(bone_instances);
		bone_instances = NULL;
	}
}

bool	pred_sort_N(const std::pair<shared_str,u32>& A, const std::pair<shared_str,u32>& B)	{
	return xr_strcmp(A.first,B.first)<0;
}
bool	pred_sort_P(const std::pair<shared_str,u32>& A, const std::pair<shared_str,u32>& B)	{
	return A.first._get() < B.first._get();
}

void CBoneData::CalculateM2B(const Fmatrix& parent)
{
    // Build matrix
    m2b_transform.mul_43	(parent,bind_transform);

    // Calculate children
    for (xr_vector<CBoneData*>::iterator C=children.begin(); C!=children.end(); C++)
        (*C)->CalculateM2B	(m2b_transform);

    m2b_transform.invert	();            
}

CSkeletonX* CKinematics::LL_GetChild(u32 idx)
{
	IRender_Visual*	V = children[idx];
	CSkeletonX*		B = NULL;
	switch (V->Type){
		case MT_SKELETON_GEOMDEF_PM:{
			CSkeletonX_PM*	X = (CSkeletonX_PM*)V;
			B = (CSkeletonX*)X;
									}break;
		case MT_SKELETON_GEOMDEF_ST:{
			CSkeletonX_ST*	X = (CSkeletonX_ST*)V;
			B = (CSkeletonX*)X;
									}break;
		default: NODEFAULT;
	}
	return B;
}

void	CKinematics::Load(const char* N, IReader *data, u32 dwFlags)
{
	//Msg				("skeleton: %s",N);
	inherited::Load	(N, data, dwFlags);

	// User data
	IReader* UD 	= data->open_chunk(OGF_S_USERDATA);
    pUserData		= UD?xr_new<CInifile>(UD):0;
    if (UD)			UD->close();

	// Globals
	bone_map_N		= xr_new<accel>		();
	bone_map_P		= xr_new<accel>		();
	bones			= xr_new<vecBones>	();
	bone_instances	= NULL;

	// Load bones
	xr_vector<shared_str>	L_parents;

	R_ASSERT		(data->find_chunk(OGF_S_BONE_NAMES));

    visimask.zero	();
	u32 dwCount 	= data->r_u32();
	// Msg				("!!! %d bones",dwCount);
	// if (dwCount >= 64)	Msg			("!!! More than 64 bones is a crazy thing! (%d), %s",dwCount,N);
	VERIFY3			(dwCount < 64, "More than 64 bones is a crazy thing!",N);
	for (; dwCount; dwCount--)		{
		string256	buf;

		// Bone
		u16			ID				= u16(bones->size());
		data->r_stringZ(buf);		strlwr(buf);
		CBoneData* pBone 			= CreateBoneData(ID);
		pBone->child_faces.resize	(children.size());
		bones->push_back			(pBone);
		shared_str bname			= shared_str(buf);
		bone_map_N->push_back		(mk_pair(bname,ID));
		bone_map_P->push_back		(mk_pair(bname,ID));

		// It's parent
		data->r_stringZ				(buf);	strlwr(buf);
		L_parents.push_back			(buf);

		data->r						(&pBone->obb,sizeof(Fobb));
        visimask.set				(u64(1)<<ID,TRUE);
	}
	std::sort	(bone_map_N->begin(),bone_map_N->end(),pred_sort_N);
	std::sort	(bone_map_P->begin(),bone_map_P->end(),pred_sort_P);

	// Attach bones to their parents
	iRoot = BI_NONE;
	for (u32 i=0; i<bones->size(); i++) {
		shared_str	P 		= L_parents[i];
		CBoneData* B	= (*bones)[i];
		if (!P||!P[0]) {
			// no parent - this is root bone
			R_ASSERT	(BI_NONE==iRoot);
			iRoot		= u16(i);
			B->ParentID	= BI_NONE;
			continue;
		} else {
			u16 ID		= LL_BoneID(P);
			R_ASSERT	(ID!=BI_NONE);
			(*bones)[ID]->children.push_back(B);
			B->ParentID	= ID;
		}
	}
	R_ASSERT	(BI_NONE != iRoot);

	// Free parents
    L_parents.clear();

    // IK data
	IReader* IKD 	= data->open_chunk(OGF_S_IKDATA);
    if (IKD){
        for (u32 i=0; i<bones->size(); i++) {
            CBoneData*	B 	= (*bones)[i];
            u16 vers		= (u16)IKD->r_u32();
            IKD->r_stringZ	(B->game_mtl_name);
            IKD->r			(&B->shape,sizeof(SBoneShape));
            B->IK_data.Import(*IKD,vers);
            Fvector vXYZ,vT;
            IKD->r_fvector3	(vXYZ);
            IKD->r_fvector3	(vT);
            B->bind_transform.setXYZi(vXYZ);
            B->bind_transform.translate_over(vT);
	        B->mass			= IKD->r_float();
    	    IKD->r_fvector3	(B->center_of_mass);
        }
        // calculate model to bone converting matrix
        (*bones)[LL_GetBoneRoot()]->CalculateM2B(Fidentity);
    	IKD->close();
    }
	// after load process
	{
		for (u16 child_idx=0; child_idx<children.size(); child_idx++)
			LL_GetChild(child_idx)->AfterLoad	(this,child_idx);
	}

	// unique bone faces
	{
		for (u32 bone_idx=0; bone_idx<bones->size(); bone_idx++) {
			CBoneData*	B 	= (*bones)[bone_idx];
			for (u32 child_idx=0; child_idx<children.size(); child_idx++){
				CBoneData::FacesVec faces		= B->child_faces[child_idx];
				std::sort						(faces.begin(),faces.end());
				CBoneData::FacesVecIt new_end	= std::unique(faces.begin(),faces.end());
				faces.erase						(new_end,faces.end());
				B->child_faces[child_idx].clear_and_free();
				B->child_faces[child_idx]		= faces;
			}
		}
	}
/*
	for (u16 rr=0; rr<LL_BoneCount(); rr++){
		CBoneData& BD	= LL_GetData(rr);
		Log("bones:",rr);
		for (u32 ch=0; ch<BD.child_faces.size(); ch++){
			Log("child:",ch);
			for (u32 fc=0; fc<BD.child_faces[ch].size(); fc++){
				Log("F:",BD.child_faces[ch][fc]);
			}
		}
	}
*/
	// reset update_callback
	Update_Callback	= NULL;
}

#define PCOPY(a)	a = pFrom->a
void CKinematics::Copy(IRender_Visual *P) 
{
	inherited::Copy	(P);

	CKinematics* pFrom = (CKinematics*)P;
    PCOPY(pUserData	);
	PCOPY(bones		);
	PCOPY(iRoot		);
	PCOPY(bone_map_N);
	PCOPY(bone_map_P);
    PCOPY(visimask	);

	IBoneInstances_Create	();

	for (u32 i=0; i<children.size(); i++) 
		LL_GetChild(i)->SetParent(this);

	CalculateBones_Invalidate	();
}

void CKinematics::CalculateBones_Invalidate	()
{	
	UCalc_Time		= 0x0; 
	UCalc_Visibox	= psSkeletonUpdate;		
}

void CKinematics::Spawn	()
{
	inherited::Spawn		();

	for (u32 i=0; i<bones->size(); i++)
		bone_instances[i].construct();

	Update_Callback				= NULL;
	CalculateBones_Invalidate	();
}

void CKinematics::Release()
{
	// xr_free bones
	for (u32 i=0; i<bones->size(); i++)
    {
		CBoneData* &B = (*bones)[i];
		xr_delete(B);
	}

	// destroy shared data
    xr_delete(pUserData	);
	xr_delete(bones		);
	xr_delete(bone_map_N);
	xr_delete(bone_map_P);

	inherited::Release();
}

void CKinematics::LL_SetBoneVisible(u16 bone_id, BOOL val, BOOL bRecursive)
{
	VERIFY(bone_id<LL_BoneCount());      
    u64 mask 			= u64(1)<<bone_id;
    visimask.set		(mask,val);
	if (!visimask.is(mask))
        bone_instances[bone_id].mTransform.scale(0.f,0.f,0.f);
        bone_instances[bone_id].mRenderTransform.mul_43(bone_instances[bone_id].mTransform,(*bones)[bone_id]->m2b_transform);
    if (bRecursive){
        for (xr_vector<CBoneData*>::iterator C=(*bones)[bone_id]->children.begin(); C!=(*bones)[bone_id]->children.end(); C++)
            LL_SetBoneVisible((*C)->SelfID,val,bRecursive);
    }
}

void CKinematics::LL_SetBonesVisible(u64 mask)
{
	visimask.assign			(0);	
	for (u32 b=0; b<bones->size(); b++){
    	u64 bm				= u64(1)<<b;
    	if (mask&bm){
        	visimask.set	(bm,TRUE);
        }else{
	    	Fmatrix& A		= bone_instances[b].mTransform;
	    	Fmatrix& B		= bone_instances[b].mRenderTransform;
        	A.scale			(0.f,0.f,0.f);
	        B.mul_43		(A,(*bones)[b]->m2b_transform);
        }
	}
}

IC static void RecursiveBindTransform(CKinematics* K, xr_vector<Fmatrix>& matrices, u16 bone_id, const Fmatrix& parent)
{
	CBoneData& BD			= K->LL_GetData	(bone_id);
	Fmatrix& BM				= matrices[bone_id];
	// Build matrix
	BM.mul_43				(parent,BD.bind_transform);
    for (xr_vector<CBoneData*>::iterator C=BD.children.begin(); C!=BD.children.end(); C++)
		RecursiveBindTransform(K,matrices,(*C)->SelfID,BM);	
}

void CKinematics::LL_GetBindTransform(xr_vector<Fmatrix>& matrices)
{
	matrices.resize			(LL_BoneCount());
	RecursiveBindTransform	(this,matrices,iRoot,Fidentity);
}

void BuildMatrix		(Fmatrix &mView, float invsz, const Fvector norm, const Fvector& from)
{
	// build projection
	Fmatrix				mScale;
	Fvector				at,up,right,y;
	at.sub				(from,norm);
	y.set				(0,1,0);
	if (_abs(norm.y)>.99f) y.set(1,0,0);
	right.crossproduct	(y,norm);
	up.crossproduct		(norm,right);
	mView.build_camera	(from,at,up);
	mScale.scale		(invsz,invsz,invsz);
	mView.mulA			(mScale);
}

void CKinematics::AddWallmark(const Fmatrix* parent_xf, const Fvector3& start, const Fvector3& dir, u16 bone_id, ref_shader shader, float size)
{
	Fvector S,D,normal		= {0,0,0};
	// transform ray from world to model
	Fmatrix P;	P.invert	(*parent_xf);
	P.transform_tiny		(S,start);
	P.transform_dir			(D,dir);
	// find pick point
	float dist				= flt_max;
	BOOL picked				= FALSE;
	for (u32 i=0; i<children.size(); i++)
		if (LL_GetChild(i)->PickBone(normal,dist,S,D,bone_id)) picked=TRUE;
	if (!picked) return;

	// calc contact point
	Fvector cp;	cp.mad		(S,D,dist);

	// find similar wm
	for (u32 wm_idx=0; wm_idx<wallmarks.size(); wm_idx++){
		CSkeletonWallmark*& wm = wallmarks[wm_idx];
		if (wm->Similar(shader,cp,0.02f)){ 
			xr_delete		(wm);
			if (wm_idx<wallmarks.size()-1) 
				wm = wallmarks.back();
			wallmarks.pop_back();
			break;
		}
	}

	// ok. allocate wallmark
	CSkeletonWallmark* wm	= xr_new<CSkeletonWallmark>(this,parent_xf,shader,cp,Device.fTimeGlobal);
	parent_xf->transform_tiny(wm->m_Bounds.P,cp);
	wm->m_Bounds.R			= size;

	// build UV projection matrix
	Fmatrix					mView,mRot;
	BuildMatrix				(mView,1/size,normal,cp);
	mRot.rotateZ			(::Random.randF(deg2rad(-20.f),deg2rad(20.f)));
	mView.mulA_43			(mRot);
	// fill vertices
	for (u32 i=0; i<children.size(); i++){
		CSkeletonX* S		= LL_GetChild(i);
		CBoneData& BD		= LL_GetData(bone_id);
		// this bone
		S->FillVertices		(mView,*wm,normal,size,BD.SelfID);
		// parent
		if (BD.ParentID!=BI_NONE)	
			S->FillVertices	(mView,*wm,normal,size,BD.ParentID);
		// children
		for (u32 i=0; i<BD.children.size(); i++)
			S->FillVertices	(mView,*wm,normal,size,BD.children[i]->SelfID);
	}

	wallmarks.push_back		(wm);
}

static const float LIFE_TIME=10.f;

void CKinematics::CalculateWallmarks()
{
	if (!wallmarks.empty()){
		bool need_remove=false; 
		for (SkeletonWMVecIt it=wallmarks.begin(); it!=wallmarks.end(); it++){
			CSkeletonWallmark*& wm = *it;
			float w	= (Device.fTimeGlobal-wm->TimeStart())/LIFE_TIME;
			if (w<=1.f){
				// append wm to WallmarkEngine
				if (::Render->ViewBase.testSphere_dirty(wm->m_Bounds.P,wm->m_Bounds.R))
					::Render->add_SkeletonWallmark	(wm);
			}else{
				// remove wallmark
				xr_delete							(wm);
				need_remove							= true;
			}
		}
		if (need_remove){
			SkeletonWMVecIt new_end= std::remove(wallmarks.begin(),wallmarks.end(),(CSkeletonWallmark*)0);
			wallmarks.erase	(new_end,wallmarks.end());
		}
	}
}

void CKinematics::RenderWallmark(CSkeletonWallmark* wm, FVF::LIT* &V)
{
	Fbox bb;	bb.invalidate();
	// skin vertices
	for (u32 f_idx=0; f_idx<wm->m_Faces.size(); f_idx++){
		CSkeletonWallmark::WMFace& F=wm->m_Faces[f_idx];
		float w	= (Device.fTimeGlobal-wm->TimeStart())/LIFE_TIME;
		for (u32 k=0; k<3; k++){
			Fvector P;
			if (F.bone_id[k][0]==F.bone_id[k][1]){
				// 1-link
				Fmatrix& xform0			= LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform; 
				xform0.transform_tiny	(P,F.vert[k]);
			}else{
				// 2-link
				Fvector P0,P1;
				Fmatrix& xform0			= LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform; 
				Fmatrix& xform1			= LL_GetBoneInstance(F.bone_id[k][1]).mRenderTransform; 
				xform0.transform_tiny	(P0,F.vert[k]);
				xform1.transform_tiny	(P1,F.vert[k]);
				P.lerp					(P0,P1,F.weight[k]);
			}
			wm->XFORM()->transform_tiny	(V->p,P);
			V->t.set					(F.uv[k]);
			int			aC				= iFloor	( w * 255.f);	clamp	(aC,0,255);
			V->color					= color_rgba(128,128,128,aC);
			bb.modify					(V->p);
			V++;
		}
	}
	bb.getsphere						(wm->m_Bounds.P,wm->m_Bounds.R);
}
