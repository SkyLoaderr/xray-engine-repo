#include "stdafx.h"
#include "Actor.h"
#include "../CameraBase.h"
#include "Car.h"

#include "Weapon.h"
#include "Inventory.h"

#include "ShootingHitEffector.h"
#include "SleepEffector.h"
#include "ActorEffector.h"
#include "level.h"
#include "../cl_intersect.h"

#include "d3dx9math.h"
#pragma comment (lib,"d3dx.lib")

void CActor::cam_Set	(EActorCameras style)
{
	CCameraBase* old_cam = cam_Active();
	cam_active = style;
	old_cam->OnDeactivate();
	cam_Active()->OnActivate(old_cam);
}
float CActor::f_Ladder_cam_limit=1.f;
void CActor::cam_SetLadder()
{
	CCameraBase* C			= cameras[eacFirstEye];
	g_LadderOrient			();
	C->yaw					= -XFORM().k.getH();
	C->lim_yaw[0]			= C->yaw-1.f;
	C->lim_yaw[1]			= C->yaw+1.f;
	C->bClampYaw			= true;
}
void CActor::cam_UnsetLadder()
{
	CCameraBase* C			= cameras[eacFirstEye];
	C->lim_yaw[0]			= 0;
	C->lim_yaw[1]			= 0;
	C->bClampYaw			= false;
}
float CActor::CameraHeight()
{
	Fvector						R;
	m_PhysicMovementControl->Box().getsize		(R);
	return m_fCamHeightFactor*R.y;
}

IC float viewport_near(float& w, float& h)
{
	float a = Device.fFOV;
	w = 2.f*VIEWPORT_NEAR*tan(deg2rad(Device.fFOV)/2.f);
	h = w*Device.fASPECT;
	float	c	= _sqrt					(w*w + h*h);
	return	_max(_max(VIEWPORT_NEAR,_max(w,h)),c);
}
/*
float	a0	= deg2rad(Device.fFOV*Device.fASPECT/2.f);
float	a1	= deg2rad(Device.fFOV/2.f);
h			= VIEWPORT_NEAR/_cos	(a0);
w			= VIEWPORT_NEAR/_cos	(a1);
float	c	= _sqrt					(w*w + h*h);
return	_max(_max(VIEWPORT_NEAR,_max(w,h)),c);
*/
struct SPassableParams{
	BOOL	solid;
	float	range;
	SPassableParams():solid(FALSE),range(0.f){}
};
IC BOOL __stdcall passable_callback(Collide::rq_result& result, LPVOID params)
{
	SPassableParams* V = (SPassableParams*)params;
	CDB::TRI* T	= g_pGameLevel->ObjectSpace.GetStaticTris()+result.element;
	if (!GMLib.GetMaterialByIdx(T->material)->Flags.is(SGameMtl::flPassable)){ 
		V->solid	= TRUE;
		V->range	= result.range;
		return FALSE;
	}
	return TRUE;
}

IC void calc_point(Fvector& pt, float radius, float depth, float alpha)
{
	pt.x	= radius*_sin(alpha);
	pt.y	= radius+radius*_cos(alpha);
	pt.z	= depth;
}

Fmatrix s_mat;
Fvector s_ext;
void CActor::cam_Update(float dt, float fFOV)
{
	if(m_holder)
	{
        m_holder->cam_Update(dt);
		EffectorManager().Update(m_holder->Camera());
		EffectorManager().ApplyDevice();
		return;
	}
	
	Fvector point={0,CameraHeight(),0}, dangle={0,0,0};
	
	// apply inertion
	switch (cam_active){
	case eacFirstEye:		break;
	case eacLookAt: 		break;
	case eacFreeLook: 		break;
	}

	Fmatrix xform,xformR;
	xform.setXYZ		(0,r_torso.yaw,0);
	xform.translate_over(XFORM().c);
/*
	Fvector c[4];
	{
		Fmatrix	ex_project, ex_full, ex_full_inverse;
		{
			D3DXMatrixInverse			((D3DXMATRIX*)&ex_full_inverse,0,(D3DXMATRIX*)&Device.mFullTransform);
			static Fvector3		corners [4]			= {
				{ -1, -1,  0 },		{ -1, +1,  0},
				{ +1, +1,  0},		{ +1, -1,  0}
			};
			for(int p=0; p<4; p++)	{
				ex_full_inverse.transform(c[p],corners[p]);
			}
		}
		float w,h;
		viewport_near(w,h);
		Fvector v;
		float y0 = v.sub(c[3],c[0]).magnitude();
		float y1 = v.sub(c[2],c[3]).magnitude();
		int y=0;
	}
*/






	// lookout
	if (!fis_zero(r_torso_tgt_roll)){
//		if (_abs(r_torso_tgt_roll)>_abs(r_torso.roll))
		{
			// up
			Fvector src_pt,tgt_pt;
			float radius		= point.y*0.5f;
			float alpha			= r_torso_tgt_roll/2.f;
			float dZ			= ((PI_DIV_2-((PI+alpha)/2)));
			calc_point			(tgt_pt,radius,0,alpha);
			src_pt.set			(0,tgt_pt.y,0);
			// init valid angle
			float valid_angle	= alpha;
			// xform with roll
			xformR.setXYZ		(-r_torso.pitch,r_torso.yaw,-dZ);
//			xformZ.mulA_43		(xform);
			Fmatrix33			mat; 
			mat.i				= xformR.i;
			mat.j				= xformR.j;
			mat.k				= xformR.k;
			s_mat				= xformR;
			// get viewport params
			float w,h;
			float c				= viewport_near(w,h); w/=2.f;h/=2.f;
			// find tris
			Fbox box;
			box.invalidate		();
			box.modify			(src_pt);
			box.modify			(tgt_pt);
			box.grow			(c);
			g_pGameLevel->ObjectSpace.BoxQuery(box,xform,clGET_TRIS|clQUERY_STATIC);
			u32 tri_count		= g_pGameLevel->ObjectSpace.q_result.tris.size();	
			if (tri_count){
				// test 
				Fvector			trans,ext;
				ext.set			(w,h,VIEWPORT_NEAR/2);  
				s_ext			= ext;

				float da		= PI/1000.f;
				if (!fis_zero(r_torso.roll))
					da			*= r_torso.roll/_abs(r_torso.roll);

				BOOL bIntersect	= FALSE;
				for (float angle=da; _abs(angle)<_abs(alpha); angle+=da){
					calc_point		(tgt_pt,radius,VIEWPORT_NEAR/2,angle);
					xform.transform_tiny(tgt_pt);
					s_mat.c			= tgt_pt;
					for (u32 i_t=0; i_t<tri_count; i_t++){
						clQueryTri&	O	= g_pGameLevel->ObjectSpace.q_result.tris[i_t];
						if (CDB::TestBBoxTri(mat,tgt_pt,ext,O.p,FALSE)){
							bIntersect	= TRUE;
							valid_angle = angle;
							break;
						}
					}
					if (bIntersect) break;
				}
			}
			r_torso.roll		= valid_angle*2.f;
		}
		r_torso_tgt_roll		= r_torso.roll;
		float radius			= point.y*0.5f;
		float alpha				= r_torso.roll/2.f;
		point.x					= radius*_sin(alpha);
		point.y					= radius+radius*_cos(alpha);
		dangle.z				= (PI_DIV_2-((PI+alpha)/2));
	}

	// soft crouch
	float dS					= point.y-fPrevCamPos;
	if (_abs(dS)>EPS_L){
		point.y					= 0.7f*fPrevCamPos+0.3f*point.y;
	}
	// save previous position of camera
	fPrevCamPos					= point.y;

	xform.transform_tiny		(point);


	CCameraBase* C			= cam_Active();
	C->Update				(point,dangle);
	C->f_fov				= fFOV;
	if(eacFirstEye != cam_active){
		cameras[eacFirstEye]->Update	(point,dangle);
		cameras[eacFirstEye]->f_fov		= fFOV;
	}
	EffectorManager().Update			(cameras[eacFirstEye]);

//	if(eacLookAt == cam_active)
//		EffectorManager().Update			(C);

	if (Level().CurrentEntity() == this)
	{
		Level().Cameras.Update	(C);
		if(eacFirstEye == cam_active && !Level().Cameras.GetEffector(cefDemo))
			EffectorManager().ApplyDevice();
	}
}

#ifdef DEBUG
void dbg_draw_frustum (float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U);
extern	Flags32	dbg_net_Draw_Flags;

void CActor::OnRender	()
{
	RCache.dbg_DrawOBB	(s_mat,s_ext,0xFFFFFFFF);

	/*
	u32			dwOffset = 0,dwCount = 0;
	FVF::LIT* pv_start				= (FVF::LIT*)RCache.Vertex.Lock(4,hFriendlyIndicator->vb_stride,dwOffset);
	FVF::LIT* pv						= pv_start;
	// base rect
	Fvector2 		lt,rb;
	lt.x = - 1.f; lt.y = + 1.f;
	rb.x = + 1.f; rb.y = - 1.f;
	pv->set					(lt.x,rb.y,0.f, 0xffffffff, 0.f, 1.f); pv++;	// 0
	pv->set					(lt.x,lt.y,0.f, 0xffffffff, 0.f, 0.f); pv++; 	// 1
	pv->set					(rb.x,rb.y,0.f, 0xffffffff, 1.f, 1.f); pv++;	// 2
	pv->set					(rb.x,lt.y,0.f, 0xffffffff, 1.f, 0.f); pv++;	// 3
	// render	
	dwCount 				= u32(pv-pv_start);
	RCache.Vertex.Unlock	(dwCount,hFriendlyIndicator->vb_stride);

	RCache.set_xform_world		(Fidentity);
	RCache.set_Shader			(hIndicatorShader);
	RCache.set_Geometry			(hFriendlyIndicator);
	RCache.Render	   			(D3DPT_TRIANGLESTRIP,dwOffset,0, dwCount, 0, 2);
	//*/

	if (!bDebug)				return;

	if ((dbg_net_Draw_Flags.is_any((1<<5))))
		m_PhysicMovementControl->dbg_Draw	();

	OnRender_Network();
}
#endif

void CActor::LoadShootingEffector (LPCSTR section)
{
	if(!m_pShootingEffector) 
		m_pShootingEffector = xr_new<SShootingEffector>();


	m_pShootingEffector->ppi.duality.h		= pSettings->r_float(section,"duality_h");
	m_pShootingEffector->ppi.duality.v		= pSettings->r_float(section,"duality_v");
	m_pShootingEffector->ppi.gray				= pSettings->r_float(section,"gray");
	m_pShootingEffector->ppi.blur				= pSettings->r_float(section,"blur");
	m_pShootingEffector->ppi.noise.intensity	= pSettings->r_float(section,"noise_intensity");
	m_pShootingEffector->ppi.noise.grain		= pSettings->r_float(section,"noise_grain");
	m_pShootingEffector->ppi.noise.fps		= pSettings->r_float(section,"noise_fps");
	VERIFY(!fis_zero(m_pShootingEffector->ppi.noise.fps));

	sscanf(pSettings->r_string(section,"color_base"),	"%f,%f,%f", &m_pShootingEffector->ppi.color_base.r, &m_pShootingEffector->ppi.color_base.g, &m_pShootingEffector->ppi.color_base.b);
	sscanf(pSettings->r_string(section,"color_gray"),	"%f,%f,%f", &m_pShootingEffector->ppi.color_gray.r, &m_pShootingEffector->ppi.color_gray.g, &m_pShootingEffector->ppi.color_gray.b);
	sscanf(pSettings->r_string(section,"color_add"),	"%f,%f,%f", &m_pShootingEffector->ppi.color_add.r,  &m_pShootingEffector->ppi.color_add.g,	&m_pShootingEffector->ppi.color_add.b);

	m_pShootingEffector->time				= pSettings->r_float(section,"time");
	m_pShootingEffector->time_attack		= pSettings->r_float(section,"time_attack");
	m_pShootingEffector->time_release		= pSettings->r_float(section,"time_release");

	m_pShootingEffector->ce_time			= pSettings->r_float(section,"ce_time");
	m_pShootingEffector->ce_amplitude		= pSettings->r_float(section,"ce_amplitude");
	m_pShootingEffector->ce_period_number	= pSettings->r_float(section,"ce_period_number");
	m_pShootingEffector->ce_power			= pSettings->r_float(section,"ce_power");
}

void CActor::LoadSleepEffector	(LPCSTR section)
{
	if(!m_pSleepEffector) 
		m_pSleepEffector = xr_new<SSleepEffector>();

	m_pSleepEffector->ppi.duality.h			= pSettings->r_float(section,"duality_h");
	m_pSleepEffector->ppi.duality.v			= pSettings->r_float(section,"duality_v");
	m_pSleepEffector->ppi.gray				= pSettings->r_float(section,"gray");
	m_pSleepEffector->ppi.blur				= pSettings->r_float(section,"blur");
	m_pSleepEffector->ppi.noise.intensity	= pSettings->r_float(section,"noise_intensity");
	m_pSleepEffector->ppi.noise.grain		= pSettings->r_float(section,"noise_grain");
	m_pSleepEffector->ppi.noise.fps			= pSettings->r_float(section,"noise_fps");
	VERIFY(!fis_zero(m_pSleepEffector->ppi.noise.fps));

	sscanf(pSettings->r_string(section,"color_base"),	"%f,%f,%f", &m_pSleepEffector->ppi.color_base.r, &m_pSleepEffector->ppi.color_base.g, &m_pSleepEffector->ppi.color_base.b);
	sscanf(pSettings->r_string(section,"color_gray"),	"%f,%f,%f", &m_pSleepEffector->ppi.color_gray.r, &m_pSleepEffector->ppi.color_gray.g, &m_pSleepEffector->ppi.color_gray.b);
	sscanf(pSettings->r_string(section,"color_add"),	"%f,%f,%f", &m_pSleepEffector->ppi.color_add.r,  &m_pSleepEffector->ppi.color_add.g,  &m_pSleepEffector->ppi.color_add.b);

	m_pSleepEffector->time				= pSettings->r_float(section,"time");
	m_pSleepEffector->time_attack		= pSettings->r_float(section,"time_attack");
	m_pSleepEffector->time_release		= pSettings->r_float(section,"time_release");
}

/*
Fvector world_dir;
Fvector local_pt=point,world_pt;
float radius		= point.y*0.5f;
float alpha			= r_torso_tgt_roll/2.f;
local_pt.x			= radius*_sin(alpha);
local_pt.y			= radius+radius*_cos(alpha);
//			dangle.z			= (PI_DIV_2-((PI+alpha)/2));
// check tgt_pt
world_dir.sub		(local_pt,point);
float	range		= world_dir.magnitude();
if (!fis_zero(range,EPS_L)){
XFORM().transform_tiny	(world_pt,point);
XFORM().transform_dir	(world_dir.div(range));
Collide::rq_result R;
float safe_radius	= viewport_near();
if (_abs(r_torso.roll)<_abs(r_torso_tgt_roll)){
if (g_pGameLevel->ObjectSpace.RayPick(world_pt,world_dir,range+safe_radius+EPS_L,Collide::rqtStatic,R)){
r_torso_tgt_roll= (mstate_real&mcLLookout)?-ACTOR_LOOKOUT_ANGLE:ACTOR_LOOKOUT_ANGLE;
}else{
r_torso.roll	= r_torso_tgt_roll;
}
}else{
r_torso.roll	= r_torso_tgt_roll;
}
}
alpha				= r_torso.roll/2.f;
local_pt.x			= radius*_sin(alpha);
local_pt.y			= radius+radius*_cos(alpha);
//			dangle.z			= (PI_DIV_2-((PI+alpha)/2));
Log("PT",local_pt);
// update point
point.set			(local_pt);
*/
/*
Fvector tgt_dir;
tgt_dir.sub			(tgt_pt,src_pt);
float range			= tgt_dir.magnitude();
if (!fis_zero(range)){
tgt_dir.div		(range);
float view_near = viewport_near();
Collide::ray_defs RD	(src_pt,tgt_dir,range+view_near,0,Collide::rqtStatic);
SPassableParams V;
if (!g_pGameLevel->ObjectSpace.RayQuery(RD,passable_callback,&V)||!V.solid){
r_torso.roll= r_torso_tgt_roll;
}else{
float sign	= 1.f;
if (!fis_zero(r_torso.roll))
sign	= r_torso.roll/_abs(r_torso.roll);
r_torso.roll= sign*2.f*asin(_max(0.f,(V.range-view_near))/radius);
}
}
*/
