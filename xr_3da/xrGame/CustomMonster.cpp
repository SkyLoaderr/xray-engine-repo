// CustomMonster.cpp: implementation of the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\xr_hudfont.h"
#include "ai_console.h"
#include "CustomMonster.h"
#include "xr_weapon_list.h"
#include "customitem.h"
#include "hudmanager.h"

DWORD psAI_Flags	= 0;
ENGINE_API extern float psGravity;

void CCustomMonster::SAnimState::Create(CKinematics* K, LPCSTR base)
{
	char	buf[128];
	fwd		= K->ID_Cycle_Safe(strconcat(buf,base,"_fwd"));
	back	= K->ID_Cycle_Safe(strconcat(buf,base,"_back"));
	ls		= K->ID_Cycle_Safe(strconcat(buf,base,"_ls"));
	rs		= K->ID_Cycle_Safe(strconcat(buf,base,"_rs"));
}

void __stdcall CCustomMonster::TorsoSpinCallback(CBoneInstance* B)
{
	CCustomMonster*		M = dynamic_cast<CCustomMonster*> (static_cast<CObject*>(B->Callback_Param));

	Fmatrix					spin;
	spin.setXYZ				(0, M->NET_Last.o_torso.pitch, 0);
	B->mTransform.mulB_43	(spin);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCustomMonster::CCustomMonster()
{
	Device.seqRender.Add	(this,REG_PRIORITY_LOW-999);

	Weapons				= 0;
	tWatchDirection = Direction();
	m_cBodyState = BODY_STATE_STAND;
	r_torso_speed = PI;
	r_spine_speed = PI;
	q_look.o_look_speed = PI;
	m_fSoundPower = m_fStartPower = 0;
	m_dwSoundUpdate = 0;
	m_fBananPadlaCorrection = 0;
}

CCustomMonster::~CCustomMonster	()
{
	_DELETE		(Weapons);

	Device.seqRender.Remove	(this);
}

void CCustomMonster::Load		(LPCSTR section)
{
	Msg("Loading AI entity: %s",section);
	
	inherited::Load				(section);

	vPosition.y += EPS_L;
	
	R_ASSERT					(pVisual->Type==MT_SKELETON);
	
	Movement.SetPosition		(vPosition);
	
	// Health & Armor
	iArmor					= 0;
	
	// Eyes
	eye_bone				= PKinematics(pVisual)->LL_BoneID(pSettings->ReadSTRING(section,"bone_head"));
	eye_fov					= pSettings->ReadFLOAT(section,"eye_fov");
	eye_range				= pSettings->ReadFLOAT(section,"eye_range");

	// movement
	m_fJumpSpeed			= pSettings->ReadFLOAT(section,"jump_speed");
	//
	m_fMinSpeed				= pSettings->ReadFLOAT(section,"min_speed");
	m_fMaxSpeed				= pSettings->ReadFLOAT(section,"max_speed");
	m_fCurSpeed				= m_fMaxSpeed;

	// Motions
	CKinematics* V			= PKinematics(pVisual);
	m_current				= 0;

	// weapons
	if (pSettings->ReadINT(section,"weapon_usage")) {
		Weapons					= new CWeaponList(this);
		LPCSTR S1 = pSettings->ReadSTRING(section,"bone_torso_weapon"),S2 = pSettings->ReadSTRING(section,"bone_head_weapon");
		Weapons->Init			(S1,S2);
		Weapons->TakeItem		(CLSID_OBJECT_W_AK74, 0);
	}

	// take index spine bone
	//"torso1"
	int torso_bone			= PKinematics(pVisual)->LL_BoneID(pSettings->ReadSTRING(section,"bone_torso"));
	PKinematics(pVisual)->LL_GetInstance(torso_bone).set_callback(TorsoSpinCallback,this);

	//
	m_iHealth = pSettings->ReadINT(section,"health");

	// Sheduler
	dwMinUpdate	= 50;
	dwMaxUpdate	= 500; // 30 * NET_Latency / 4;

	//
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

/*
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
*/

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
	R_ASSERT(fsimilar(_view.magnitude(),1));
	R_ASSERT(fsimilar(_move.magnitude(),1));

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
			}else //if (dot<-0.7f)
			{
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

	u8 flags;
	P->r_u32				(N.dwTimeStamp);
	P->r_u8					(flags);
	P->r_vec3				(N.p_pos);
	P->r_angle8				(N.o_model);
	P->r_angle8				(N.o_torso.yaw);
	P->r_angle8				(N.o_torso.pitch);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	bVisible				= TRUE;
	bEnabled				= TRUE;
}

void CCustomMonster::Exec_Physics( float dt )
{
	// Test nearest object
	Fvector C; float R;	
	svCenter(C);
	R = Radius();
	Level().ObjectSpace.TestNearestObject(cfModel, C, R);
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
		// here is monster AI call
		m_fTimeUpdateDelta = dt;
		Think();
		if (m_fCurSpeed < EPS_L) {
			AI_Path.TravelPath.clear();
			AI_Path.TravelStart = 0;
		}
		// Look and action streams
		if (iHealth>0) {
			Exec_Look			(dt);
			Exec_Movement		(dt);
			Exec_Visibility		(dt);
			Exec_Physics		(dt);
			
			net_update			uNext;
			uNext.dwTimeStamp	= Level().timeServer();
			uNext.o_model		= r_torso_current.yaw;
			uNext.o_torso		= r_torso_current;
			uNext.p_pos			= vPosition;
			NET.push_back		(uNext);
		}
		else {
			Exec_Physics	(dt);
			if (bfExecMovement()) {
				Exec_Movement	(dt);
				net_update			uNext;
				uNext.dwTimeStamp	= Level().timeServer();
				uNext.o_model		= r_torso_current.yaw;
				uNext.o_torso		= r_torso_current;
				uNext.p_pos			= vPosition;
				NET.push_back		(uNext);
			}
		}
		Exec_Action	(dt);
	}

	// weapons
	if (Weapons)		Weapons->Update		(dt,false);
	
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
	clTransform.rotateY			(NET_Last.o_model);
	clTransform.translate_over	(NET_Last.p_pos);

	if (Remote())		{
		svTransform.rotateY			(N.o_model);
		svTransform.translate_over	(N.p_pos);
		vPosition.set				(NET_Last.p_pos);
	}
}

static BOOL __fastcall Qualifier				(CObject* O, void* P)
{
	CEntity* E = dynamic_cast<CEntity*>		(O);
	if (0==E)								return FALSE;
	if (E->g_Team() == int(*LPDWORD(P)))	return FALSE;
	if (!E->IsVisibleForAI())				return FALSE;
	return TRUE;
}

objQualifier* CCustomMonster::GetQualifier	()
{
	return(&Qualifier);
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
	Device.Statistic.TEST0.Begin	();
	CKinematics* V				= PKinematics(Visual());
	V->Calculate				();
	Fmatrix&	mEye			= V->LL_GetTransform(eye_bone);
	Fmatrix		X;				X.mul_43(svTransform,mEye);

	eye_matrix.setHPB			(-r_current.yaw + 0*m_fBananPadlaCorrection,-r_current.pitch,0);
	eye_matrix.c.set			(X.c);
	Device.Statistic.TEST0.End		();
	
	Device.Statistic.AI_Vis.Begin();		//--------------

	// 2. Build matrix and frustum
	Fmatrix		mProject,mFull,mView;
	CFrustum	Frustum;
	
	mView.build_camera_dir		(eye_matrix.c,eye_matrix.k,eye_matrix.j);
	mProject.build_projection	(deg2rad(eye_fov),1,0.1f,eye_range);
	mFull.mul					(mProject,mView);
	Frustum.CreateFromMatrix	(mFull,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	
	// 3. Detection itself
	Device.Statistic.AI_Vis_Query.Begin	();
	R_ASSERT			(pSector);
	objSET				seen;
	seen.clear			();
	pSector->GetObjects	(Frustum,eye_matrix.c,mFull,seen,GetQualifier(),&id_Team);
	Device.Statistic.AI_Vis_Query.End	();

	// 4. Visibility processing
	Device.Statistic.AI_Vis_RayTests.Begin	();
	ai_Track.o_update	(seen,this,eye_matrix.c,dt);
	Device.Statistic.AI_Vis_RayTests.End	();

	Device.Statistic.AI_Vis.End();			//--------------

	// 5. Camera
	if (IsMyCamera())
		pCreator->Cameras.Update(eye_matrix.c,eye_matrix.k,eye_matrix.j,eye_fov,1.f,eye_range);
}

extern void dbg_draw_frustum (float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U);
void CCustomMonster::OnRender()
{
	if (0 == (psAI_Flags&aiDebug)) return;
	
	Device.Shader.OnFrameEnd	();
	{
	for (DWORD I=1; I<AI_Path.TravelPath.size(); I++)
	{
		CTravelNode&	N1 = AI_Path.TravelPath[I-1];	Fvector	P1; P1.set(N1.P); P1.y+=0.1f;
		CTravelNode&	N2 = AI_Path.TravelPath[I];		Fvector	P2; P2.set(N2.P); P2.y+=0.1f;
		Device.Primitive.dbg_DrawLINE(Fidentity,P1,P2,D3DCOLOR_XRGB(0,255,0));
		Device.Primitive.dbg_DrawAABB(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
		
		/**/
		Fvector         T;
        Fvector4        S;
        
        T.set   (AI_Path.TravelPath[I].P); T.y+=(Radius()*2);
        Device.mFullTransform.transform (S,T);
        
        pApp->pFont->Size       (0.07f/sqrtf(fabsf(S.w)));
        pApp->pFont->Color      (D3DCOLOR_RGBA(0,255,0,(S.z<=0)?0:255));
		pApp->pFont->Out		(S.x,-S.y,"%d",I);
		/**/
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
		Device.Primitive.dbg_DrawLINE(Fidentity,P1,P2,D3DCOLOR_XRGB(255,255,255));
	}
	}

	if (psAI_Flags&aiFrustum) 
		dbg_draw_frustum(eye_fov,eye_range,1,eye_matrix.c,eye_matrix.k,eye_matrix.j);
	
	if (psAI_Flags&aiMotion) {
		Movement.dbg_Draw();
	}
	if (bDebug) PKinematics(pVisual)->DebugRender(clTransform);
}

void CCustomMonster::HitSignal(int perc, Fvector& vLocalDir, CEntity* who)
{
}

void CCustomMonster::Death	()
{
}

BOOL CCustomMonster::Spawn	(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	if (!inherited::Spawn(bLocal,server_id,o_pos,o_angle,P,flags))	return FALSE;
	iHealth = m_iHealth;
	AI_Path.DestNode		= AI_NodeID;

	eye_matrix.identity	();

	r_torso_current.yaw = r_torso_target.yaw = o_angle.y;
	r_torso_current.pitch = r_torso_target.pitch = 0;

	if (Local())	
	{
		net_update		N;
		N.dwTimeStamp	= Level().timeServer()-NET_Latency;
		N.o_model		= o_angle.y;
		N.o_torso.yaw	= o_angle.y;
		N.o_torso.pitch	= 0;
		N.p_pos.set		(o_pos);
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
