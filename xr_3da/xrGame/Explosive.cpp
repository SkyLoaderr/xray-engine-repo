// Explosive.cpp: ��������� ��� ������������� ��������
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "explosive.h"

#include "PhysicsShell.h"
#include "entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"

//��� ������ ����������� ������� ��������� ���������
#include "Weapon.h"

#include "explode_effector.h" 
#include "actor.h"
#include "actoreffector.h"
#include "level.h"
#include "level_bullet_manager.h"
#include "xrmessages.h"
#include "gamemtllib.h"
#include "clsid_game.h"
#ifdef DEBUG
#include "../StatGraph.h"
#include "PHDebug.h"
#endif
#include "Physics.h"
#include "MathUtils.h"
#include "phvalidevalues.h"
#include "PHActivationShape.h"
#define EFFECTOR_RADIUS 30.f
const u16	TEST_RAYS_PER_OBJECT=5;
const u16	BLASTED_OBJ_PROCESSED_PER_FRAME=3;
const float	exp_dist_extinction_factor=3.f;//(>1.f, 1.f -means no dist change of exp effect)	on the dist of m_fBlastRadius exp. wave effect in exp_dist_extinction_factor times less than maximum
CExplosive::CExplosive(void) 
{
	m_fBlastHit = 50.f;
	m_fBlastRadius = 10.f;
	m_iFragsNum = 20;
	m_fFragsRadius = 30.f;
	m_fFragHit = 50;

	m_fUpThrowFactor = 0.f;


	m_eSoundExplode = ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);

	m_pLight = ::Render->light_create();
	m_pLight->set_shadow(true);


	m_eHitTypeBlast = ALife::eHitTypeExplosion;
	m_eHitTypeFrag = ALife::eHitTypeFireWound;


	m_iCurrentParentID = 0xffff;
	m_bReadyToExplode = false;

	m_bExploding = false;
	m_bExplodeEventSent = false;
	m_vExplodeSize.set(0.001f,0.001f,0.001f);
}

CExplosive::~CExplosive(void) 
{
	m_pLight.destroy	();
	sndExplode.destroy	();
}

void CExplosive::Load(LPCSTR section) 
{
	Load				(pSettings,section);
}

void CExplosive::Load(CInifile *ini,LPCSTR section)
{
	m_fBlastHit			= ini->r_float(section,"blast");
	m_fBlastRadius		= ini->r_float(section,"blast_r");
	m_fBlastHitImpulse	= ini->r_float(section,"blast_impulse");

	m_iFragsNum			= ini->r_s32(section,"frags");
	m_fFragsRadius		= ini->r_float(section,"frags_r");
	m_fFragHit			= ini->r_float(section,"frag_hit");
	m_fFragHitImpulse	= ini->r_float(section,"frag_hit_impulse");

	m_eHitTypeBlast		= ALife::g_tfString2HitType(ini->r_string(section, "hit_type_blast"));
	m_eHitTypeFrag		= ALife::g_tfString2HitType(ini->r_string(section, "hit_type_frag"));

	m_fUpThrowFactor	= ini->r_float(section,"up_throw_factor");


	fWallmarkSize		= ini->r_float(section,"wm_size");
	R_ASSERT			(fWallmarkSize>0);

	m_sExplodeParticles = ini->r_string(section,"explode_particles");

	sscanf				(ini->r_string(section,"light_color"), "%f,%f,%f", &m_LightColor.r, &m_LightColor.g, &m_LightColor.b);
	m_fLightRange		= ini->r_float(section,"light_range");
	m_fLightTime		= ini->r_float(section,"light_time");

	//������ ��� ������� ��������
	tracerHeadSpeed		= ini->r_float		(section,"tracer_head_speed"	);
	tracerMaxLength		= ini->r_float		(section,"tracer_max_length"	);

	shared_str				snd_name = ini->r_string(section,"snd_explode");
	sndExplode.create	(TRUE,*snd_name, m_eSoundExplode);

	m_fExplodeDurationMax	= ini->r_float(section, "explode_duration");

	effector.time			= ini->r_float("explode_effector","time");
	effector.amplitude		= ini->r_float("explode_effector","amplitude");
	effector.period_number	= ini->r_float("explode_effector","period_number");
}

void CExplosive::net_Destroy	()
{
	m_blasted_objects.clear();
	m_bExploding = false;
	StopLight();
}


/////////////////////////////////////////////////////////
// ����� 
/////////////////////////////////////////////////////////
struct SExpQParams
{
#ifdef DEBUG

	SExpQParams(const Fvector& ec,const Fvector& d)
	{
		shoot_factor=			1.f			;
		source_p				.set(ec)	;
		l_dir					.set(d)		;
	}
	Fvector		source_p			;					
	Fvector 	l_dir				;
#else
	SExpQParams()
	{
		shoot_factor=			1.f			;
	}
#endif

	float		shoot_factor		;
};
//�������� �� ��������� "��������" �� �������
ICF static BOOL grenade_hit_callback(collide::rq_result& result, LPVOID params)
{
	SExpQParams& ep	= *(SExpQParams*)params;
	u16 mtl_idx			= GAMEMTL_NONE_IDX;
	if(result.O){
		CKinematics* V  = 0;
		if (0!=(V=smart_cast<CKinematics*>(result.O->Visual()))){
			CBoneData& B= V->LL_GetData((u16)result.element);
			mtl_idx		= B.game_mtl_idx;
		}
	}else{
		//�������� ����������� � ������ ��� ��������
		CDB::TRI* T		= Level().ObjectSpace.GetStaticTris()+result.element;
		mtl_idx			= T->material;
	}	
	SGameMtl* mtl		= GMLib.GetMaterialByIdx(mtl_idx);
	ep.shoot_factor		*=mtl->fShootFactor;
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgDrawExplosions))
	{
		Fvector p;p.set(ep.l_dir);p.mul(result.range);p.add(ep.source_p);
		u8 c	=u8(mtl->fShootFactor*255.f);
		DBG_DrawPoint(p,0.1f,D3DCOLOR_XRGB(255-c,0,c));
	}
#endif
	return				(ep.shoot_factor>0.01f);
}



float CExplosive::ExplosionEffect(collide::rq_results& storage, CExplosive*exp_obj,CPhysicsShellHolder*blasted_obj,  const Fvector &expl_centre, const float expl_radius) 
{
	
	const Fmatrix	&obj_xform=blasted_obj->XFORM();
	const Fbox &l_b1 = blasted_obj->BoundingBox();
	Fvector l_c, l_d;l_b1.get_CD(l_c,l_d);
	float effective_volume=l_d.x*l_d.y*l_d.z;
	float max_s=l_d.x*l_d.y*l_d.z/(_min(_min(l_d.x,l_d.y),l_d.z));
	if(blasted_obj->PPhysicsShell()&&blasted_obj->PPhysicsShell()->bActive)
	{
		float ph_volume=blasted_obj->PPhysicsShell()->getVolume();
		if(ph_volume<effective_volume)effective_volume=ph_volume;
	}
	float effect=0.f;
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgDrawExplosions))
	{
		Fmatrix dbg_box_m;dbg_box_m.set(obj_xform);
		dbg_box_m.c.set(l_c);obj_xform.transform(dbg_box_m.c);
		DBG_DrawOBB(dbg_box_m,l_d,D3DCOLOR_XRGB(255,255,0));
	}
#endif

	blasted_obj->setEnabled(FALSE);
	for(u16 i=0;i<TEST_RAYS_PER_OBJECT;++i)
	{
		
		Fvector l_source_p,l_end_p;
		l_end_p.random_point(l_d);
		l_end_p.add(l_c);
		obj_xform.transform_tiny(l_end_p);
		GetRaySourcePos(exp_obj,expl_centre,l_source_p);
		Fvector l_dir; l_dir.sub(l_end_p,l_source_p);
		float mag=l_dir.magnitude();
		if(fis_zero(mag)) 
		{
			blasted_obj->setEnabled(TRUE);return 1.f;
		}
		l_dir.mul(1.f/mag);
		#ifdef DEBUG
			if(ph_dbg_draw_mask.test(phDbgDrawExplosions))
			{
			DBG_DrawPoint(l_source_p,0.1f,D3DCOLOR_XRGB(0,0,255));
			DBG_DrawPoint(l_end_p,0.1f,D3DCOLOR_XRGB(0,0,255));
			DBG_DrawLine(l_source_p,l_end_p,D3DCOLOR_XRGB(0,0,255));
			}
		#endif
		
	
		float l_S=effective_volume*(_abs(l_dir.dotproduct(obj_xform.i))/l_d.x+_abs(l_dir.dotproduct(obj_xform.j))/l_d.y+_abs(l_dir.dotproduct(obj_xform.k))/l_d.z);

		effect+=l_S/max_s*TestPassEffect(l_source_p,l_dir,mag,expl_radius,storage);

	}
	blasted_obj->setEnabled(TRUE);
	return effect/TEST_RAYS_PER_OBJECT;
	
}
float CExplosive::TestPassEffect(const	Fvector	&source_p,	const	Fvector	&dir,float range,float ef_radius,collide::rq_results& storage)
{
	float sq_ef_radius=ef_radius*ef_radius;
	float dist_factor	=		sq_ef_radius/(range*range*(exp_dist_extinction_factor-1.f)+sq_ef_radius);
	float shoot_factor=1.f;
	if(range>EPS_L)
	{

		
		collide::ray_defs	RD		(source_p,dir,range,CDB::OPT_CULL,collide::rqtBoth);
#ifdef DEBUG
		SExpQParams			ep		(source_p,dir);
#else
		SExpQParams			ep		();
#endif


		g_pGameLevel->ObjectSpace.RayQuery(storage,RD,grenade_hit_callback,&ep);
		shoot_factor=ep.shoot_factor;
	
	}
	else return dist_factor;
	return shoot_factor*dist_factor;
}
void CExplosive::Explode()
{
	VERIFY(0xffff != Initiator());
	VERIFY(m_bReadyToExplode);

	m_bExploding = true;
	cast_game_object()->processing_activate();

	Fvector& pos = m_vExplodePos;
	Fvector& dir = m_vExplodeDir;
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgDrawExplosions))
	{
		DBG_OpenCashedDraw();
		DBG_DrawPoint(pos,0.3f,D3DCOLOR_XRGB(255,0,0));
	}
#endif
//	Msg("---------CExplosive Explode [%d] frame[%d]",cast_game_object()->ID(), Device.dwFrame);
	OnBeforeExplosion();
	//������ ���� ������
	Sound->play_at_pos(sndExplode, 0, pos, false);
	
	//���������� �������
	CParticlesObject* pStaticPG; 
	pStaticPG = CParticlesObject::Create(*m_sExplodeParticles,TRUE); 
	
	Fvector vel;
	smart_cast<CPhysicsShellHolder*>(cast_game_object())->PHGetLinearVell(vel);

	Fmatrix explode_matrix;
	explode_matrix.identity();
	explode_matrix.j.set(dir);
	Fvector::generate_orthonormal_basis(explode_matrix.j, explode_matrix.i, explode_matrix.k);
	explode_matrix.c.set(pos);

	pStaticPG->UpdateParent(explode_matrix,vel);
	pStaticPG->Play();

	//�������� ��������� �� ������
	StartLight();

	//trace frags
	Fvector frag_dir; 
	
	//////////////////////////////
	//�������
	//////////////////////////////
	//-------------------------------------
	bool SendHits = false;
	if (OnServer()) SendHits = true;
	else SendHits = false;
	//-------------------------------------
	for(int i = 0; i < m_iFragsNum; ++i) 
	{
		frag_dir.random_dir();
		frag_dir.normalize();
		
		float		m_fCurrentFireDist = m_fFragsRadius;
		float		m_fCurrentHitPower = m_fFragHit;
		float		m_fCurrentHitImpulse = m_fFragHitImpulse;
		ALife::EHitType m_eCurrentHitType = m_eHitTypeFrag;
		float		m_fCurrentWallmarkSize = fWallmarkSize;
		Fvector		m_vCurrentShootDir = frag_dir;
		Fvector		m_vCurrentShootPos = pos;
		
		CCartridge cartridge;
		cartridge.m_kDist = 1.f;
		cartridge.m_kHit = 1.f;
		cartridge.m_kImpulse = 1.f;
		cartridge.m_kPierce = 1.f;
		cartridge.fWallmarkSize = m_fCurrentWallmarkSize;
#pragma todo("oles to yura: zdes' tozge nebilo materiala, ya ne znayu kakoy nado")
		cartridge.bullet_material_idx = GMLib.GetMaterialIdx(WEAPON_MATERIAL_NAME);	//. hack???

		Level().BulletManager().AddBullet(	m_vCurrentShootPos, m_vCurrentShootDir, tracerHeadSpeed,
											m_fCurrentHitPower, m_fCurrentHitImpulse, Initiator(),
											cast_game_object()->ID(), m_eCurrentHitType, m_fCurrentFireDist, cartridge, SendHits, tracerMaxLength);
	}	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (cast_game_object()->Remote()) return;
	
	/////////////////////////////////
	//�������� �����
	////////////////////////////////
	//---------------------------------------------------------------------
	xr_vector<ISpatial*>	ISpatialResult;
	g_SpatialSpace->q_sphere(ISpatialResult,0,STYPE_COLLIDEABLE,pos,m_fBlastRadius);

	m_blasted_objects.clear	();
	for (u32 o_it=0; o_it<ISpatialResult.size(); o_it++)
	{
		ISpatial*		spatial	= ISpatialResult[o_it];
		//		feel_touch_new(spatial->dcast_CObject());

		CPhysicsShellHolder	*pGameObject = smart_cast<CPhysicsShellHolder*>(spatial->dcast_CObject());
		if(pGameObject && cast_game_object()->ID() != pGameObject->ID()) 
			m_blasted_objects.push_back(pGameObject);
	}

	GetExplosionBox(m_vExplodeSize);
	ActivateExplosionBox(m_vExplodeSize,m_vExplodePos);
	//---------------------------------------------------------------------
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgDrawExplosions))
	{
		DBG_ClosedCashedDraw(100000);
		
	}
#endif
	//////////////////////////////////////////////////////////////////////////
	// Explode Effector	//////////////
	CGameObject* GO = smart_cast<CGameObject*>(Level().CurrentEntity());
	CActor* pActor = smart_cast<CActor*>(GO);
	if(pActor)
	{
		float dist_to_actor = pActor->Position().distance_to(pos);
		float max_dist		= EFFECTOR_RADIUS;
		if (dist_to_actor < max_dist) 
			pActor->EffectorManager().AddEffector(xr_new<CExplodeEffector>(effector.time, effector.amplitude, effector.period_number, (max_dist - dist_to_actor) / max_dist));
	}
	//////////////////////////////////
	
}

void CExplosive::PositionUpdate()
{
	Fvector vel;
	Fvector& pos=m_vExplodePos;
	Fvector& dir=m_vExplodeDir;
	GetExplVelocity(vel);
	GetExplPosition(pos);
	GetExplDirection(dir);
	Fmatrix explode_matrix;
	explode_matrix.identity();
	explode_matrix.j.set(dir);
	Fvector::generate_orthonormal_basis(explode_matrix.j, explode_matrix.i, explode_matrix.k);
	explode_matrix.c.set(pos);
	
}
void CExplosive::GetExplPosition(Fvector &p)
{
	p.set(m_vExplodePos);
}

void CExplosive::GetExplDirection(Fvector &d)
{
	d.set(m_vExplodeDir);
}
void CExplosive::GetExplVelocity(Fvector &v)
{
	smart_cast<CPhysicsShellHolder*>(cast_game_object())->PHGetLinearVell(v);
}

void CExplosive::UpdateCL() 
{
	//VERIFY(!this->getDestroy());

	if(!m_bExploding) return; 

	//����� �����, ������� ������ ����������
	if(m_fExplodeDuration < 0.f&&m_blasted_objects.empty()) 
	{
		m_bExploding = false;
		cast_game_object()->processing_deactivate();

		StopLight();
		
		OnAfterExplosion();
//		Msg("---------CExplosive OnAfterExplosion [%d] frame[%d]",cast_game_object()->ID(), Device.dwFrame);

	} 
	else
	{
		m_fExplodeDuration -= Device.fTimeDelta;
		UpdateExplosionPos();
		ExplodeWaveProcess();
		//�������� ��������� ������
		if(m_pLight->get_active() && m_fLightTime>0)
		{
			if(m_fExplodeDuration > (m_fExplodeDurationMax - m_fLightTime))
			{
				float scale = (m_fExplodeDuration - (m_fExplodeDurationMax - m_fLightTime))/m_fLightTime;
				m_pLight->set_color(m_LightColor.r*scale, m_LightColor.g*scale, m_LightColor.b*scale);
				m_pLight->set_range(m_fLightRange*scale);
			} 
			else
				StopLight();
		}
	}
}

void CExplosive::OnAfterExplosion()
{
	//������������� ��� ������ 
	if (cast_game_object()->Local()) cast_game_object()->DestroyObject();
//	NET_Packet			P;
//	cast_game_object()->u_EventGen			(P,GE_DESTROY,cast_game_object()->ID());
//	//		Msg					("ge_destroy: [%d] - %s",ID(),*cName());
//	if (cast_game_object()->Local()) cast_game_object()->u_EventSend			(P);
}
void CExplosive::OnBeforeExplosion()
{
	cast_game_object()->setVisible(false);
	cast_game_object()->setEnabled(false);
//	Msg("---------CExplosive OnBeforeExplosion setVisible(false) [%d] frame[%d]",cast_game_object()->ID(), Device.dwFrame);

}

void CExplosive::OnEvent(NET_Packet& P, u16 type) 
{
	switch (type) {
		case GE_GRENADE_EXPLODE : {
			Fvector pos, normal;
			u16 parent_id;
			P.r_u16(parent_id);
			P.r_vec3(pos);
			P.r_vec3(normal);
			
			SetInitiator(parent_id);
			ExplodeParams(pos,normal);
			Explode();
			m_fExplodeDuration = m_fExplodeDurationMax;
			break;
		}
	}
}

void CExplosive::ExplodeParams(const Fvector& pos, 
								const Fvector& dir)
{
	m_bReadyToExplode = true;
	m_vExplodePos = pos;
	m_vExplodeDir = dir;
}

void CExplosive::GenExplodeEvent (const Fvector& pos, const Fvector& normal)
{
	if (OnClient() || cast_game_object()->Remote()) return;
//	if( m_bExplodeEventSent ) 
//		return;

	VERIFY(0xffff != Initiator());

	NET_Packet		P;
	cast_game_object()->u_EventGen		(P,GE_GRENADE_EXPLODE,cast_game_object()->ID());	
	P.w_u16			(Initiator());
	P.w_vec3		(const_cast<Fvector&>(pos));
	P.w_vec3		(const_cast<Fvector&>(normal));
	cast_game_object()->u_EventSend		(P);

//	m_bExplodeEventSent = true;
}

void CExplosive::FindNormal(Fvector& normal)
{
	collide::rq_result RQ;

	Fvector pos, dir;
	dir.set(0,-1.f,0);
	cast_game_object()->Center(pos);

	BOOL result = Level().ObjectSpace.RayPick(pos, dir, cast_game_object()->Radius(), 
											 collide::rqtBoth, RQ);
	if(!result || RQ.O)
		normal.set(0,1,0);
	//���� ����� �� �������
	//����� ����������� � ��������� ������� �� ����
	else
	{
		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris() + RQ.element;
		normal.mknormal	(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
	}
}

void CExplosive::StartLight	()
{
	VERIFY(m_pLight);

	if(m_fLightTime>0)
	{
		m_pLight->set_color(m_LightColor.r, m_LightColor.g, m_LightColor.b);
		m_pLight->set_range(m_fLightRange);
		m_pLight->set_position(m_vExplodePos); 
		m_pLight->set_active(true);
	}
}
void CExplosive::StopLight	()
{
	VERIFY(m_pLight);
	m_pLight->set_active(false);
}
void CExplosive::GetRaySourcePos(CExplosive*exp_obj,const	Fvector	&expl_center,Fvector	&p)
{
	if(exp_obj)
	{
		exp_obj->GetRayExplosionSourcePos(p);
	}
}
void CExplosive::GetRayExplosionSourcePos(Fvector &pos)
{
	pos.set(m_vExplodeSize);pos.mul(0.5f);
	pos.random_point(pos);
	pos.add(m_vExplodePos);
}
void CExplosive::ExplodeWaveProcessObject(collide::rq_results& storage, CPhysicsShellHolder*l_pGO)
{
	Fvector	l_goPos;
	if(l_pGO->Visual())		l_pGO->Center	(l_goPos); 
	else					return; //��� ��������� ����� �������� ��� �� ������ �� �������� �� ������� ������ - ������� ����������

#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgDrawExplosions))
	{
		DBG_OpenCashedDraw();
		
	}
#endif

	float l_effect=ExplosionEffect(storage,this,l_pGO,m_vExplodePos,m_fBlastRadius);
	float l_impuls	= m_fBlastHitImpulse * l_effect;
	float l_hit		= m_fBlastHit * l_effect;

	if(l_impuls > .001f||l_hit> 0.001) 
	{
	
		Fvector l_dir;l_dir.sub(l_goPos,m_vExplodePos);
		
		float rmag=_sqrt(m_fUpThrowFactor*m_fUpThrowFactor+1.f+2.f*m_fUpThrowFactor*l_dir.y);
		l_dir.y += m_fUpThrowFactor;
		//rmag -������ l_dir ����� l_dir.y += m_fUpThrowFactor, ������=_sqrt(l_dir^2+y^2+2.*(l_dir,y)),y=(0,m_fUpThrowFactor,0) (�� ����� ������ l_dir =1)
		l_dir.mul(1.f/rmag);//��������������
 		NET_Packet		P;
		cast_game_object()->u_EventGen		(P,GE_HIT,l_pGO->ID());
		P.w_u16			(Initiator());
		P.w_u16			(cast_game_object()->ID());
		P.w_dir			(l_dir);
		P.w_float		(l_hit);
		P.w_s16			(0);
		P.w_vec3		(l_goPos);
		P.w_float		(l_impuls);
		P.w_u16			(u16(m_eHitTypeBlast));
		cast_game_object()->u_EventSend		(P);
	}
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgDrawExplosions))
	{
		DBG_ClosedCashedDraw(100000);

	}
#endif
}
struct SRemovePred
{
	bool operator () (CGameObject* O)
	{
		return !!O->getDestroy();
	}
};
void CExplosive::ExplodeWaveProcess()
{

	BLASTED_OBJECTS_I I=std::remove_if(m_blasted_objects.begin(),m_blasted_objects.end(),SRemovePred());
	m_blasted_objects.erase(I,m_blasted_objects.end());
	collide::rq_results		rq_storage		;
	u16 i=BLASTED_OBJ_PROCESSED_PER_FRAME	;
	while(m_blasted_objects.size()&&0!=i) 
	{
		ExplodeWaveProcessObject(rq_storage,m_blasted_objects.back());
		m_blasted_objects.pop_back();
		--i;
	}	
}

void CExplosive::GetExplosionBox(Fvector	&size)
{
	size.set(m_vExplodeSize);
}
void CExplosive::SetExplosionSize(const Fvector	&new_size)
{
	m_vExplodeSize.set(new_size);
	
}
void CExplosive::ActivateExplosionBox(const Fvector &size,Fvector &in_out_pos)
{
	CPHActivationShape activation_shape;//Fvector start_box;m_PhysicMovementControl.Box().getsize(start_box);
	activation_shape.Create(in_out_pos,size,NULL);
	dBodySetGravityMode(activation_shape.ODEBody(),0);
	activation_shape.Activate(size,1,1.f,M_PI/8.f);
	in_out_pos.set(activation_shape.Position());
	activation_shape.Size(m_vExplodeSize);
	activation_shape.Destroy();
}
