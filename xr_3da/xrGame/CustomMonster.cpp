// CustomMonster.cpp: implementation of the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\xr_mac.h"
#include "..\bodyinstance.h"
#include "..\fmesh.h"
#include "..\xr_hudfont.h"
#include "..\fstaticrender.h"
#include "ai_console.h"
#include "CustomMonster.h"
#include "xr_weapon_list.h"
#include "customitem.h"
#include "hudmanager.h"

const DWORD NET_Latency	= 100;

DWORD psAI_Flags	= 0;
ENGINE_API extern float psGravity;

void CCustomMonster::SAnimState::Create(CKinematics* K, LPCSTR base)
{
	char	buf[128];
	fwd		= K->ID_Cycle(strconcat(buf,base,"_fwd"));
	back	= K->ID_Cycle(strconcat(buf,base,"_back"));
	ls		= K->ID_Cycle(strconcat(buf,base,"_ls"));
	rs		= K->ID_Cycle(strconcat(buf,base,"_rs"));
}
void __stdcall CCustomMonster::SpinCallback(CBoneInstance* B)
{
	CCustomMonster*		M = (CCustomMonster*)B->Callback_Param;

	Fmatrix				spin;
	spin.setXYZ			(0, M->NET_Last.o_torso.pitch, 0);
	B->mTransform.mul_43(spin);
/*
	Fmatrix				spin;
	float				bone_yaw	= A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta;
	float				bone_pitch	= A->r_torso.pitch;
	clamp				(bone_pitch,-PI_DIV_8,PI_DIV_4);
	spin.setXYZ			(bone_yaw,bone_pitch,0);
	B->mTransform.mul_43(spin);
*/
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define X_SIZE_2 0.35f
#define Y_SIZE_2 0.8f
#define Z_SIZE_2 0.35f
CCustomMonster::CCustomMonster()
{
//	Device.seqRender.Add	(this,REG_PRIORITY_LOW-999);

	Weapons					= 0;
}

CCustomMonster::~CCustomMonster()
{
	_DELETE		(Weapons);

	Device.seqRender.Remove	(this);

	State_Pop	();
	State_Pop	();
	State_Pop	();
	State_Pop	();
	State_Pop	();
}

void CCustomMonster::Load(CInifile* ini, const char* section)
{
	Msg("Loading AI entity: %s",section);
	
	inherited::Load(ini,section);

	vPosition.y += EPS_L;
	
	R_ASSERT	(pVisual->Type==MT_SKELETON);
	
	Movement.SetPosition	(vPosition);
	
	// Health & Armor
	iArmor					= 0;
	
	// Eyes
	eye_bone				= PKinematics(pVisual)->LL_BoneID("head");
	eye_fov					= ini->ReadFLOAT(section,"eye_fov");
	eye_range				= ini->ReadFLOAT(section,"eye_range");

	// movement
	m_fWalkAccel			= ini->ReadFLOAT(section,"walk_accel");	
	m_fJumpSpeed			= ini->ReadFLOAT(section,"jump_speed");
	m_fRunCoef				= ini->ReadFLOAT(section,"run_coef");

	// Motions
	PKinematics	V			= PKinematics(pVisual);
	m_current				= 0;
	m_idle					= V->ID_Cycle("norm_idle");
	m_death					= V->ID_Cycle("norm_death");
	m_walk.Create			(V,"norm_walk");
	m_run.Create			(V,"norm_run");
	PKinematics(pVisual)->PlayCycle(m_idle);

	// weapons
	Weapons					= new CWeaponList(this);
	Weapons->Init			("torso2","head");
	Weapons->TakeItem		(CLSID_OBJECT_W_M134_en,0);

	// take index spine bone
	int spine_bone			= PKinematics(pVisual)->LL_BoneID("torso1");
	PKinematics(pVisual)->LL_GetInstance(spine_bone).set_callback(SpinCallback,this);

	// Pathfinding
	fuzzyFollow.Load		(ini,section);
	fuzzyAttack.Load		(ini,section);
	fuzzyPursuit.Load		(ini,section);

	// Sheduler
	dwMinUpdate	= 25;
	dwMaxUpdate	= 200;
}

void CCustomMonster::g_fireParams(Fvector &fire_pos, Fvector &fire_dir)
{
	fire_pos.set			(eye_matrix.c);
	fire_dir.set			(eye_matrix.k);
}

void CCustomMonster::g_cl_fireStart	( )
{
	Weapons->FireStart	();
}

void CCustomMonster::g_fireEnd	( )
{
	Weapons->FireEnd	();
}

BOOL CCustomMonster::TakeItem	( DWORD CID )
{
	CCustomItem* O = (CCustomItem*) pCreator->Objects.GetObjectByCID(CID);
	int iValue = O->iValue;
	switch (O->clsid_target) {
	case CLSID_OBJECT_W_RAIL:
	case CLSID_OBJECT_A_RAIL:
		return Weapons->TakeItem(O->clsid_target,iValue);
		break;
	case CLSID_OBJECT_HEALTH:
		if (iHealth<iMAX_Health){
			iHealth += iValue;
			if (iHealth>iMAX_Health) iHealth = iMAX_Health;
		}else return false;
		break;
	case CLSID_OBJECT_ARMOR:
		if (iArmor<iMAX_Armor){
			iArmor += iValue;
			if (iArmor>iMAX_Armor) iArmor = iMAX_Armor;
		}else return false;
		break;
	default:
		return false;
	}
	return true;
}

void CCustomMonster::mk_orientation(Fvector &dir, Fmatrix& mR)
{
	// orient only in XZ plane
	dir.y		= 0;
	float		len = dir.magnitude();
	if			(len>EPS_S)
	{
		// normalize
		dir.x /= len;
		dir.z /= len;
		Fvector up;	up.set(0,1,0);
		mR.rotation	(dir,up);
	}
}

void CCustomMonster::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	CMotionDef*	S=0;

	bool bCrouched = false; 

	if (iHealth<=0) {
		// Die
		S = m_death;
	} else {
		if (speed<0.2f) {
			// idle
			S = m_idle;
		} else {
			Fvector view = _view; view.y=0; view.normalize_safe();
			Fvector move = _move; move.y=0; move.normalize_safe();
			float	dot  = view.dotproduct(move);
			
			SAnimState* AState = &m_walk;
//			if (bCrouched)	AState = &m_crouch_walk;
//			else			
			AState = &m_walk;
			
			if (speed>2.f){
//				if (bCrouched)	AState = &m_crouch_run;
//				else			
				AState = &m_run;
			}
			
			if (dot>0.7f){
				S = AState->fwd;
			}else if ((dot<=0.7f)&&(dot>=-0.7f)){
				Fvector cross; cross.crossproduct(view,move);
				if (cross.y>0){
					S = AState->rs;
				}else{
					S = AState->ls;
				}
			}else if (dot<-0.7f){
				S = AState->back;
			}
		}
	}
	if (S!=m_current){ 
		m_current = S;
		if (S) PKinematics(pVisual)->PlayCycle(S);
	}
}

void CCustomMonster::net_Export(NET_Packet* P)					// export to server
{
	R_ASSERT				(net_Local);
	VERIFY					(Weapons);

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P->w_u32				(N.dwTimeStamp);
	P->w_u8					(0);
	P->w_vec3				(N.p_pos);
	P->w_angle8				(N.o_model);
	P->w_angle8				(N.o_torso.yaw);
	P->w_angle8				(N.o_torso.pitch);
}

void CCustomMonster::net_Import(NET_Packet* P)
{
	R_ASSERT				(!net_Local);
	VERIFY					(Weapons);
	net_update				N;

	N.dwTimeStamp			= P->r_u32	();
	u8 flags				= P->r_u8	();
	N.p_pos					= P->r_vec3	();
	N.o_model				= P->r_angle8();
	N.o_torso.yaw			= P->r_angle8();
	N.o_torso.pitch			= P->r_angle8();

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	bVisible				= TRUE;
	bEnabled				= TRUE;
}

void CCustomMonster::Update	( DWORD DT )
{
	// Queue shrink
	DWORD	dwTimeCL	= Level().timeServer()-NET_Latency;
	VERIFY				(!NET.empty());
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();
	
	// Queue setup
	float dt			= float(DT)/1000.f;
	
	if (Remote())		{
	} else {
		// State parsing
		R_ASSERT			(!STATE.empty());
		while				(State_Get()->Parse(this)) ;
		
		// Look and action streams
		if (iHealth>0)		
		{
			Exec_Look		(dt);
			Exec_Movement	(dt);
			Exec_Visibility	(dt);
			
			net_update			uNext;
			uNext.dwTimeStamp	= Level().timeServer();
			uNext.o_model		= r_current.yaw;
			uNext.o_torso		= r_current;
			uNext.p_pos			= vPosition;
			NET.push_back		(uNext);
		}
		Exec_Action			(dt);
	}

	// weapons
	Weapons->Update		(dt,false);
	
	// *** general stuff
	inherited::Update	(DT);
}

void CCustomMonster::net_update::lerp(CCustomMonster::net_update& A, CCustomMonster::net_update& B, float f)
{
	// 
	o_model			= CEntity::u_lerp_angle	(A.o_model,B.o_model,		f);
	o_torso.yaw		= CEntity::u_lerp_angle	(A.o_torso.yaw,B.o_torso.yaw,f);
	o_torso.pitch	= CEntity::u_lerp_angle	(A.o_torso.pitch,B.o_torso.pitch,f);
	p_pos.lerp		(A.p_pos,B.p_pos,f);
}

void CCustomMonster::UpdateCL	()
{ 
	if	(NET.empty())	return;

//	Log("- CAI: UpdateCL");

	// distinguish interpolation/extrapolation
	DWORD	dwTime		= Level().timeServer()-NET_Latency;
	net_update&	N		= NET.back();
	if ((dwTime > N.dwTimeStamp) || (NET.size()<2))
	{
		// BAD.	extrapolation
//		Log("Extrapolation");
		NET_Last		= N;
	} else {
		// OK.	interpolation
//		Log("Interpolation");
		
		// Search 2 keyframes for interpolation
		int select		= -1;
		for (DWORD id=0; id<NET.size()-1; id++)
		{
			if ((NET[id].dwTimeStamp<=dwTime)&&(dwTime<=NET[id+1].dwTimeStamp))	select=id;
		}
		if (select>=0)		
		{
			// Interpolate state
			net_update&	A		= NET[select+0];
			net_update&	B		= NET[select+1];
			DWORD	d1			= dwTime-A.dwTimeStamp;
			DWORD	d2			= B.dwTimeStamp - A.dwTimeStamp;
			float	factor		= (float(d1)/float(d2));
			NET_Last.lerp		(A,B,factor);
			
			Fvector				dir;
			AI_Path.Direction	(dir);
			SelectAnimation		(clTransform.k,dir,AI_Path.fSpeed);
			
			// Signal, that last time we used interpolation
			NET_WasInterpolating	= TRUE;
			NET_Time				= dwTime;
		}
	}

	// Use interpolated/last state
	// mTransformCL	= mTransform;
	clTransform.rotateY			(-NET_Last.o_model);
	clTransform.translate_over	(NET_Last.p_pos);

	if (Remote())		{
		svTransform.rotateY			(-N.o_model);
		svTransform.translate_over	(N.p_pos);
		vPosition.set				(NET_Last.p_pos);
	}
}

BOOL __fastcall	Qualifier				(CObject* O, void* P)
{
	if (O->CLS_ID!=CLSID_ENTITY)			return FALSE;
	CEntity* E = (CEntity*)O;
	if (E->g_Team() == int(*LPDWORD(P)))	return FALSE;
	return TRUE;
}

void CCustomMonster::GetVisible			(objVisible& R)
{
	ai_Track.o_get	(R);
}

void CCustomMonster::Exec_Visibility	( float dt )
{
	if (0==pSector) return;

	// ************** first - detect visibility
	// 1. VIEW: From, Dir and Up vector
	PKinematics V		= PKinematics(Visual());
	V->Calculate		();
	Fmatrix&	mEye	= V->LL_GetTransform(eye_bone);
	Fmatrix		X;		X.mul_43(svTransform,mEye);

	eye_matrix.setHPB			(-r_current.yaw,-r_current.pitch,0);
	eye_matrix.c.set			(X.c);
	
	Device.Statistic.AI_Vis.Begin();		//--------------

	// 2. Build matrix and frustum
	Fmatrix		mProject,mFull,mView;
	CFrustum	Frustum;
	
	mView.build_camera_dir		(eye_matrix.c,eye_matrix.k,eye_matrix.j);
	mProject.build_projection	(deg2rad(eye_fov),1,0.1f,eye_range);
	mFull.mul					(mProject,mView);

	Frustum.CreateFromViewMatrix(mFull);
	
	// 3. Detection itself
	Device.Statistic.AI_Vis_Query.Begin	();
	R_ASSERT			(pSector);
	objSET				seen;
	seen.clear			();
	pSector->GetObjects	(Frustum,seen,Qualifier,&id_Team);
	Device.Statistic.AI_Vis_Query.End	();

	// 4. Visibility processing
	Device.Statistic.AI_Vis_RayTests.Begin	();
	ai_Track.o_update	(seen,this,eye_matrix.c,dt);
	Device.Statistic.AI_Vis_RayTests.End	();

	Device.Statistic.AI_Vis.End();			//--------------

	// 5. Camera
	if (IsMyCamera())
		pCreator->Cameras.Update(eye_matrix.c,eye_matrix.k,eye_matrix.j,eye_fov,eye_range);
}

void CCustomMonster::OnMoveVisible()
{
	inherited::OnMoveVisible	();
/*
	Fvector P;	clCenter(P);
	TEST.SetPosition	(P);
	TEST.SetRange		(4.f);
	TEST.SetColor		(0,0,.7f);
	::Render.Lights_Dynamic.Add	(&TEST);
*/
	// weapons
//	Weapons->OnRender			(false);
}

extern void dbg_draw_frustum (float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U);
void CCustomMonster::OnRender()
{
	if (0 == (psAI_Flags&aiDebug)) return;
	
	{
	for (DWORD I=1; I<AI_Path.TravelPath.size(); I++)
	{
		CTrevelNode&	N1 = AI_Path.TravelPath[I-1];	Fvector	P1; P1.set(N1.P); P1.y+=0.1f;
		CTrevelNode&	N2 = AI_Path.TravelPath[I];		Fvector	P2; P2.set(N2.P); P2.y+=0.1f;
		Device.Primitive.dbg_DrawLINE(precalc_identity,P1,P2,D3DCOLOR_XRGB(0,255,0));
		Device.Primitive.dbg_DrawAABB(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
	}
	}
	/*
	{
	for (DWORD I=1; I<AI_Path.TravelPath_dbg.size(); I++)
	{
		CTrevelNode&	N1 = AI_Path.TravelPath_dbg[I-1];	Fvector	P1; P1.set(N1.P); P1.y+=0.1f;
		CTrevelNode&	N2 = AI_Path.TravelPath_dbg[I];		Fvector	P2; P2.set(N2.P); P2.y+=0.1f;
		Device.Primitive.dbg_DrawLINE(precalc_identity,P1,P2,D3DCOLOR_XRGB(255,0,0));
		Device.Primitive.dbg_DrawAABB(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(255,0,0));
	}
	}
	*/
	{
	for (DWORD I=0; I<AI_Path.Segments.size(); I++)
	{
		PSegment& S = AI_Path.Segments[I];
		Fvector P1;	P1.set(S.v1); P1.y+=0.1f;
		Fvector P2;	P2.set(S.v2); P2.y+=0.1f;
		Device.Primitive.dbg_DrawAABB(P1,.01f,.01f,.01f,D3DCOLOR_XRGB(255,255,255));
		Device.Primitive.dbg_DrawAABB(P2,.01f,.01f,.01f,D3DCOLOR_XRGB(255,255,255));
		Device.Primitive.dbg_DrawLINE(precalc_identity,P1,P2,D3DCOLOR_XRGB(255,255,255));
	}
	}

	if (psAI_Flags&aiFrustum) 
		dbg_draw_frustum(eye_fov,eye_range,1,eye_matrix.c,eye_matrix.k,eye_matrix.j);
	
	if (psAI_Flags&aiMotion) {
		Movement.dbg_Draw();
	}
//	if (bDebug) PKinematics(pVisual)->DebugRender(mTransform);
}

void CCustomMonster::HitSignal(int perc, Fvector& vLocalDir, CEntity* who)
{
}

void CCustomMonster::Die()
{
}

BOOL CCustomMonster::Spawn( BOOL bLocal, int sid, int team, int squad, int group, Fvector4& o_pos )
{
	if (!inherited::Spawn(bLocal,sid,team,squad,group,o_pos))	return FALSE;
	AI_Path.DestNode		= AI_NodeID;

//	Log("- CAI: Spawning");
	if (Local())	
	{
		net_update		N;
		N.dwTimeStamp	= Level().timeServer()-NET_Latency;
		N.o_model		= o_pos.w;
		N.o_torso.yaw	= o_pos.w;
		N.o_torso.pitch	= 0;
		N.p_pos.set		(o_pos.x,o_pos.y,o_pos.z);
		NET.push_back	(N);

		N.dwTimeStamp	+= NET_Latency;
		NET.push_back	(N);

		bVisible		= TRUE;
		bEnabled		= TRUE;
	} else {

	}

	return TRUE;
}

void CCustomMonster::OnHUDDraw(CCustomHUD* hud)
{
	ai_Track.o_dump			();

	CHUDManager* H			= (CHUDManager*)hud;
	H->pHUDFont->Color		(D3DCOLOR_XRGB(255,0,0));
	H->pHUDFont->OutSet		(0,200);
	H->pHUDFont->OutNext	("NET_Q_size: %d",NET.size());
	
	/*
	const float			fy = -1;
	const float			fx = 0.5f;
	pApp->pFont->OutSet	(-1,fy+0*fx);
	pApp->pFont->Color	(D3DCOLOR_XRGB(0,0xff,0));
	pApp->pFont->OutNext("LOOK");
	DumpStream			(&q_look);

	pApp->pFont->OutSet	(-1,fy+1*fx);
	pApp->pFont->Color	(D3DCOLOR_XRGB(0,0xff,0));
	pApp->pFont->OutNext("MOVE");
	DumpStream			(&q_move);
	
	pApp->pFont->OutSet	(-1,fy+2*fx);
	pApp->pFont->Color	(D3DCOLOR_XRGB(0,0xff,0));
	pApp->pFont->OutNext("ACTION");
	DumpStream			(&q_action);
	*/
}
