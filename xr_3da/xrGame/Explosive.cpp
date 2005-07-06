// Explosive.cpp: интерфейс для взврывающихся объектов
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "explosive.h"

#include "PhysicsShell.h"
#include "entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"

//для вызова статических функций поражения осколками
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
#define EFFECTOR_RADIUS 30.f

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

	//трассы для разлета осколков
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
	m_bExploding = false;
	StopLight();
}


/////////////////////////////////////////////////////////
// Взрыв 
/////////////////////////////////////////////////////////
struct SExpQParams
{
	SExpQParams(const Fvector& ec,const Fvector& d)
	{
		shoot_factor=1.f;
		expl_centre.set(ec);
		l_dir.set(d);
	}
	Fvector	expl_centre			;					
	Fvector l_dir				;
	float	shoot_factor		;
};
//проверка на попадание "осколком" по объекту
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
		//получить треугольник и узнать его материал
		CDB::TRI* T		= Level().ObjectSpace.GetStaticTris()+result.element;
		mtl_idx			= T->material;
	}	
	SGameMtl* mtl		= GMLib.GetMaterialByIdx(mtl_idx);
	ep.shoot_factor		*=mtl->fShootFactor;
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgDrawExplosions))
	{
		Fvector p;p.set(ep.l_dir);p.mul(result.range);p.add(ep.expl_centre);
		u8 c	=u8(mtl->fShootFactor*255.f);
		DBG_DrawPoint(p,0.1f,D3DCOLOR_XRGB(255-c,0,c));
	}
#endif
	return				(ep.shoot_factor>0.01f);
}

float CExplosive::ExplosionEffect(collide::rq_results& storage, CGameObject* pExpObject,  const Fvector &expl_centre, const float expl_radius, xr_list<s16> &elements, xr_list<Fvector> &bs_positions) 
{
	Fvector l_pos				; 
	pExpObject->Center			(l_pos);
	Fvector l_dir				;
	l_dir.sub					(l_pos, expl_centre); 
	float range=l_dir.magnitude	();
	float shoot_factor=1.f;
	if(range>EPS_L)
	{
		pExpObject->setEnabled	(FALSE);
		l_dir.mul(1.f/range);
		collide::ray_defs	RD		(expl_centre,l_dir,range,CDB::OPT_CULL,collide::rqtBoth);
		SExpQParams			ep		(expl_centre,l_dir);
		g_pGameLevel->ObjectSpace.RayQuery(storage,RD,grenade_hit_callback,&ep);
		shoot_factor=ep.shoot_factor;
		pExpObject->setEnabled	(TRUE);
	}
	//if(!Level().ObjectSpace.RayPick(expl_centre, l_dir, expl_radius, collide::rqtBoth, RQ)) return 0;
	//осколок не попал или попал, но не по нам
	//if(pExpObject != RQ.O) return 0;

	/*	//предотвращение вылетания
	if(-1 != (s16)RQ.element)
	{
	elements.push_back((s16)RQ.element);
	}
	else
	{
	elements.push_back(0);
	}*/
	elements.push_back(PKinematics(pExpObject->Visual())->LL_GetBoneRoot());

	l_pos.sub(expl_centre,pExpObject->Position());
	bs_positions.push_back(l_pos);
	return shoot_factor;
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
	//играем звук взрыва
	Sound->play_at_pos(sndExplode, 0, pos, false);
	
	//показываем эффекты
	CParticlesObject* pStaticPG; 
	pStaticPG = xr_new<CParticlesObject>(*m_sExplodeParticles,TRUE); 
	
	Fvector vel;
	smart_cast<CPhysicsShellHolder*>(cast_game_object())->PHGetLinearVell(vel);

	Fmatrix explode_matrix;
	explode_matrix.identity();
	explode_matrix.j.set(dir);
	Fvector::generate_orthonormal_basis(explode_matrix.j, explode_matrix.i, explode_matrix.k);
	explode_matrix.c.set(pos);

	pStaticPG->UpdateParent(explode_matrix,vel);
	pStaticPG->Play();

	//включаем подсветку от взрыва
	StartLight();

	//trace frags
	Fvector frag_dir; 
	
	//////////////////////////////
	//осколки
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
	//взрывная волна
	////////////////////////////////

	Fvector l_goPos;
	Fvector l_dir;
	float l_dst;

	//---------------------------------------------------------------------
	xr_vector<ISpatial*>	ISpatialResult;
	g_SpatialSpace->q_sphere(ISpatialResult,0,STYPE_COLLIDEABLE,pos,m_fBlastRadius);
	
	m_blasted_objects.clear	();
	for (u32 o_it=0; o_it<ISpatialResult.size(); o_it++)
	{
		ISpatial*		spatial	= ISpatialResult[o_it];
//		feel_touch_new(spatial->dcast_CObject());

		CGameObject *pGameObject = static_cast<CGameObject*>(spatial->dcast_CObject());
		if(pGameObject && cast_game_object()->ID() != pGameObject->ID()) 
			m_blasted_objects.push_back(pGameObject);
	}
	//---------------------------------------------------------------------
//	m_blasted.clear();	
//	feel_touch_update(pos, m_fBlastRadius);

	xr_list<s16>			l_elements		;
	xr_list<Fvector>		l_bs_positions	;
	collide::rq_results		rq_storage		;
	
	while(m_blasted_objects.size()) 
	{
		CGameObject *l_pGO = m_blasted_objects.back();
		
		if(l_pGO->Visual())		l_pGO->Center	(l_goPos); 
		else					l_goPos.set		(l_pGO->Position());

		l_dir.sub(l_goPos, pos); 
		l_dst = l_dir.magnitude(); 
		if(l_dst>m_fBlastRadius){m_blasted_objects.pop_back();continue;}
#ifdef DEBUG
		if(ph_dbg_draw_mask.test(phDbgDrawExplosions))
		{
			DBG_DrawPoint(l_goPos,0.3f,D3DCOLOR_XRGB(0,0,255));
			DBG_DrawLine(pos,l_goPos,D3DCOLOR_XRGB(0,0,255));
		}
#endif
		l_dir.div(l_dst); 
		l_dir.y += m_fUpThrowFactor;
		
		f32 l_S = (l_pGO->Visual()?l_pGO->Radius()*l_pGO->Radius():0);
		
		if(l_pGO->Visual()) 
		{
			const Fbox &l_b1 = l_pGO->BoundingBox(); 
			Fbox l_b2; 
			l_b2.invalidate();
			Fmatrix l_m; l_m.identity(); 
			l_m.k.set(l_dir); 
			Fvector::generate_orthonormal_basis(l_m.k, l_m.j, l_m.i);
			
			for(int i = 0; i < 8; ++i) 
			{ 
				Fvector l_v; 
				l_b1.getpoint(i, l_v); 
				l_m.transform_tiny(l_v); 
				l_b2.modify(l_v); 
			}
			
			Fvector l_c, l_d; 
			l_b2.get_CD(l_c, l_d);
			l_S = l_d.x*l_d.y;
		}
		
		float l_dist_factor=(1.f - (l_dst/m_fBlastRadius)*(l_dst/m_fBlastRadius)) * l_S;
		float l_impuls	= m_fBlastHitImpulse * l_dist_factor;
		float l_hit		= m_fBlastHit * l_dist_factor;

		if(l_impuls > .001f) 
		{
			float l_effect=ExplosionEffect(rq_storage,l_pGO, pos, m_fBlastRadius, l_elements, l_bs_positions);
			l_impuls *= l_effect;
			l_hit*=l_effect;
		}

		if(l_impuls > .001f) 
		{
			while(l_elements.size()) 
			{
				s16 l_element = *l_elements.begin();
				Fvector l_bs_pos = *l_bs_positions.begin();
				NET_Packet		P;
				cast_game_object()->u_EventGen		(P,GE_HIT,l_pGO->ID());
				P.w_u16			(Initiator());
				P.w_u16			(cast_game_object()->ID());
				P.w_dir			(l_dir);
				P.w_float		(l_hit);
				P.w_s16			(l_element);
				P.w_vec3		(l_bs_pos);
				P.w_float		(l_impuls);
				P.w_u16			(u16(m_eHitTypeBlast));
				cast_game_object()->u_EventSend		(P);
				l_elements.pop_front();
				l_bs_positions.pop_front();
			}
		}
		m_blasted_objects.pop_back();
	}	
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

	//время вышло, убираем объект взрывчатки
	if(m_fExplodeDuration < 0.f) 
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
		//обновить подсветку взрыва
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
	//ликвидировать сам объект 
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

	VERIFY(0xffff != m_iCurrentParentID);

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
	//если лежим на статике
	//найти треугольник и вычислить нормаль по нему
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