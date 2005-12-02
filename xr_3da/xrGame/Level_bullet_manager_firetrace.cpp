// Level_Bullet_Manager.cpp:	��� ����������� ������ ���� �� ����������
//								��� ���� � ������� ���������� ����
//								(��� �������� ������������ � �� ������������)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Level_Bullet_Manager.h"
#include "entity.h"
#include "gamemtllib.h"
#include "level.h"
#include "gamepersistent.h"
#include "game_cl_base.h"
#include "xrmessages.h"
#include "clsid_game.h"
#include "../skeletoncustom.h"
#include "Actor.h"
#include "AI/Stalker/ai_stalker.h"
#include "character_info.h"

//��������� shoot_factor, ������������ 
//��������� ���� ��� ������������ � ��������
#define RICOCHET_THRESHOLD		0.1
#define STUCK_THRESHOLD			0.4

//���������� �� �������� �������� ���� �� ������� ���� ��� �� ������
#define PARENT_IGNORE_DIST		3.f
extern float gCheckHitK;

//test callback ������� 
//  object - object for testing
//return TRUE-����������� ������ / FALSE-���������� ������
BOOL CBulletManager::test_callback(const collide::ray_defs& rd, CObject* object, LPVOID params)
{
	SBullet* bullet = (SBullet*)params;
	if( (object->ID() == bullet->parent_id)		&&  
		(bullet->fly_dist<PARENT_IGNORE_DIST)	&&
		(!bullet->flags.ricochet_was))			return FALSE;

	BOOL bRes						= TRUE;
	if (object){
		CEntity*	entity			= smart_cast<CEntity*>(object);
		if (entity&&entity->g_Alive()&&(entity->ID()!=bullet->parent_id)){
			ICollisionForm*	cform	= entity->collidable.model;
			if ((NULL!=cform) && (cftObject==cform->Type())){
				CActor* actor		= smart_cast<CActor*>(entity);
				CAI_Stalker* stalker= smart_cast<CAI_Stalker*>(entity);
				if (actor||stalker){
					// test sphere intersection
					Fsphere S		= cform->getSphere();
					entity->XFORM().transform_tiny	(S.P)	;
					float dist		= rd.range;
					if (Fsphere::rpNone!=S.intersect(bullet->pos, bullet->dir, dist)){
						bool play_whine				= true;
						CObject* initiator			= Level().Objects.net_Find	(bullet->parent_id);
						if (actor){
							// actor special case
							CAI_Stalker* i_stalker	= smart_cast<CAI_Stalker*>(initiator);
							float hpf				= (i_stalker)?i_stalker->SpecificCharacter().hit_probability_factor():1.f;
							if (Random.randF(0.f,1.f)>(actor->HitProbability()*hpf)){ 
								bRes				= FALSE;	// don't hit actor
								play_whine			= true;		// play whine sound
							}else{
								// real test actor CFORM
								collide::rq_results	r_temp;
								if (cform->_RayQuery(rd,r_temp)){
									bRes			= TRUE;		// hit actor
									play_whine		= false;	// don't play whine sound
								}else{
									bRes			= FALSE;	// don't hit actor
									play_whine		= true;		// play whine sound
								}
							}
						}
						// play whine sound
						if (play_whine){
							Fvector					pt;
							pt.mad					(bullet->pos, bullet->dir, dist);
							Level().BulletManager().PlayWhineSound				(bullet,initiator,pt);
						}
					}else{
						// don't test this object again (return FALSE)
						bRes		= FALSE;
					}

				}
			}
		}
	}
	
	return bRes;
}
//callback ������� 
//	result.O;		// 0-static else CObject*
//	result.range;	// range from start to element 
//	result.element;	// if (O) "num tri" else "num bone"
//	params;			// user defined abstract data
//	Device.Statistic.TEST0.End();
//return TRUE-���������� ����������� / FALSE-��������� �����������
BOOL  CBulletManager::firetrace_callback(collide::rq_result& result, LPVOID params)
{
	SBullet* bullet = (SBullet*)params;

	//��������� ����� ���������
	Fvector end_point;
	end_point.mad(bullet->pos, bullet->dir, result.range);

	u16 hit_material_idx = GAMEMTL_NONE_IDX;

	//������������ ������
	if(result.O){
		//�������� �������� � �� ��������
		CKinematics* V = 0;
		//���� �� ������ �� �������� �� ������ ��
		//�����, �� ������������ ���, ��� ��� ��� ��
		//� �������
		VERIFY( !(result.O->ID() == bullet->parent_id &&  bullet->fly_dist<PARENT_IGNORE_DIST) );
		if (0!=(V=smart_cast<CKinematics*>(result.O->Visual()))){
			CBoneData& B = V->LL_GetData((u16)result.element);
			hit_material_idx = B.game_mtl_idx;
			Level().BulletManager().RegisterEvent(EVENT_HIT, TRUE,bullet, end_point, result, hit_material_idx);
		}
	} else {
		//����������� ������
		//�������� ����������� � ������ ��� ��������
		CDB::TRI* T			= Level().ObjectSpace.GetStaticTris()+result.element;
		hit_material_idx	= T->material;
		Level().BulletManager().RegisterEvent(EVENT_HIT, FALSE,bullet, end_point, result, hit_material_idx);
	}

	//��������� ���������� �� ���� ����, ����� ��������� ������
	if(bullet->speed<m_fMinBulletSpeed || bullet->flags.ricochet_was)
		return FALSE;
	else
		return TRUE;
}

class CFindByIDPred
{
public:
	CFindByIDPred(int element_to_find) {element = element_to_find;}
	bool operator () (CCF_OBB& ccf_obb) {return ccf_obb.elem_id == element;}
private:
	int element;
};


void CBulletManager::FireShotmark (SBullet* bullet, const Fvector& vDir, const Fvector &vEnd, collide::rq_result& R, u16 target_material, const Fvector& vNormal, bool ShowMark)
{
	SGameMtlPair* mtl_pair	= GMLib.GetMaterialPair(bullet->bullet_material_idx, target_material);
	Fvector particle_dir;

	if (R.O)
	{
		particle_dir		 = vDir;
		particle_dir.invert	();

		//�� ������� ������ ������� �� ������
		if(Level().CurrentEntity() && Level().CurrentEntity()->ID() == R.O->ID()) return;

		ref_shader* pWallmarkShader = (!mtl_pair || mtl_pair->CollideMarks.empty())?
						NULL:&mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];;

		if (pWallmarkShader && ShowMark)
		{
			//�������� ������� �� ���������
			Fvector p;
			p.mad(bullet->pos,bullet->dir,R.range-0.01f);
			::Render->add_SkeletonWallmark	(&R.O->renderable.xform, 
							PKinematics(R.O->Visual()), *pWallmarkShader,
							p, bullet->dir, bullet->wallmark_size);
		}		
	} 
	else 
	{
		//��������� ������� � ���������� �����������
		particle_dir		= vNormal;
		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+R.element;

		ref_shader* pWallmarkShader =	(!mtl_pair || mtl_pair->CollideMarks.empty())?
										NULL:&mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];;

		if (pWallmarkShader && ShowMark)
		{
			//�������� ������� �� ���������
			::Render->add_StaticWallmark	(*pWallmarkShader, vEnd, bullet->wallmark_size, pTri, pVerts);
		}
	}

	ref_sound* pSound = (!mtl_pair || mtl_pair->CollideSounds.empty())?
						NULL:&mtl_pair->CollideSounds[::Random.randI(0,mtl_pair->CollideSounds.size())];

	//��������� ����
	if(pSound && ShowMark)
	{
		CObject* O			= Level().Objects.net_Find(bullet->parent_id );
		bullet->m_mtl_snd	= *pSound;
		bullet->m_mtl_snd.play_at_pos(O, vEnd, 0);
	}

	LPCSTR ps_name = (!mtl_pair || mtl_pair->CollideParticles.empty())?
NULL:*mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];

	SGameMtl*	tgt_mtl = GMLib.GetMaterialByIdx(target_material);
	BOOL bStatic = !tgt_mtl->Flags.test(SGameMtl::flDynamic);

	if( (ps_name && ShowMark) || (bullet->flags.explosive && bStatic) )
	{
		Fmatrix pos;
		pos.k.normalize(particle_dir);
		Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);
		pos.c.set(vEnd);
		if(ps_name && ShowMark){
			//�������� �������� ��������� � ��������
			CParticlesObject* ps = CParticlesObject::Create(ps_name,TRUE);

			ps->UpdateParent(pos,zero_vel);
			GamePersistent().ps_needtoplay.push_back(ps);
		}

		if(bullet->flags.explosive&&bStatic)
			PlayExplodePS(pos);
	}
}

void CBulletManager::StaticObjectHit	(CBulletManager::_event& E)
{
//	Fvector hit_normal;
	FireShotmark(&E.bullet, E.bullet.dir,	E.point, E.R, E.tgt_material, E.normal);
//	ObjectHit	(&E.bullet,					E.point, E.R, E.tgt_material, hit_normal);
}

static bool g_clear = false;
void CBulletManager::DynamicObjectHit	(CBulletManager::_event& E)
{
	//������ ��� ������������ ��������
	VERIFY(E.R.O);
	if (g_clear) E.Repeated = false;
	if (GameID() == GAME_SINGLE) E.Repeated = false;
	bool NeedShootmark = !E.Repeated;
	
	if (E.R.O->CLS_ID == CLSID_OBJECT_ACTOR)
	{
		game_PlayerState* ps = Game().GetPlayerByGameID(E.R.O->ID());
		if (ps && ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE))
		{
			NeedShootmark = false;
		};
	}
	
	//���������� ����������� ��������� �� �������
//	Fvector			hit_normal;
	FireShotmark	(&E.bullet, E.bullet.dir, E.point, E.R, E.tgt_material, E.normal, NeedShootmark);
	
	Fvector original_dir = E.bullet.dir;
	float power, impulse;
	std::pair<float,float> hit_result = E.result; //ObjectHit(&E.bullet, E.end_point, E.R, E.tgt_material, hit_normal);
	power = hit_result.first;
	impulse = hit_result.second;

	// object-space
	//��������� ���������� ���������
	Fvector				p_in_object_space,position_in_bone_space;
	Fmatrix				m_inv;
	m_inv.invert		(E.R.O->XFORM());
	m_inv.transform_tiny(p_in_object_space, E.point);

	// bone-space
	CKinematics* V = smart_cast<CKinematics*>(E.R.O->Visual());

	if(V)
	{
		VERIFY3(V->LL_GetBoneVisible(u16(E.R.element)),*E.R.O->cNameVisual(),V->LL_BoneName_dbg(u16(E.R.element)));
		Fmatrix& m_bone = (V->LL_GetBoneInstance(u16(E.R.element))).mTransform;
		Fmatrix  m_inv_bone;
		m_inv_bone.invert(m_bone);
		m_inv_bone.transform_tiny(position_in_bone_space, p_in_object_space);
	}
	else
	{
		position_in_bone_space.set(p_in_object_space);
	}

	//��������� ��� ����������� �������
	if (E.bullet.flags.allow_sendhit && !E.Repeated)
	{
		//-------------------------------------------------
		NET_Packet		P;
		bool AddStatistic = false;
		if (GameID() != GAME_SINGLE && E.bullet.flags.allow_sendhit && E.R.O->CLS_ID == CLSID_OBJECT_ACTOR
			&& Game().m_WeaponUsageStatistic.CollectData())
		{
			CActor* pActor = smart_cast<CActor*>(E.R.O);
			if (pActor)// && pActor->g_Alive())
			{
				Game().m_WeaponUsageStatistic.OnBullet_Hit(&E.bullet, E.R.O->ID(), (s16)E.R.element, E.point);
				AddStatistic = true;
			};
		};
		
		CGameObject::u_EventGen	(P,(AddStatistic)? GE_HIT_STATISTIC : GE_HIT,E.R.O->ID());
		P.w_u16			(E.bullet.parent_id);
		P.w_u16			(E.bullet.weapon_id);
		P.w_dir			(original_dir);
		P.w_float		(power);
		//���� ������ ������ �� ������!!!!!!
///		P.w_float		(power*gCheckHitK);
		P.w_s16			((s16)E.R.element);
		P.w_vec3		(position_in_bone_space);
		P.w_float		(impulse);
		P.w_u16			(u16(E.bullet.hit_type));
		if (E.bullet.hit_type == ALife::eHitTypeFireWound)
		{
			P.w_float	(E.bullet.ap);
		}
		//-------------------------------------------------
		if (AddStatistic)
		{
			P.w_u32(E.bullet.m_dwID);
		};
		CGameObject::u_EventSend (P);
		//-------------------------------------------------
	}
}

#ifdef DEBUG
FvectorVec g_hit[3];
#endif

extern void random_dir	(Fvector& tgt_dir, const Fvector& src_dir, float dispersion);

std::pair<float, float>  CBulletManager::ObjectHit	(SBullet* bullet, const Fvector& end_point, 
									collide::rq_result& R, u16 target_material, 
									Fvector& hit_normal)
{
	//----------- normal - start
	if (R.O)
	{
		//������� ������� �� ������� ������ �������� � ������� ��������
		CCF_Skeleton* skeletion = smart_cast<CCF_Skeleton*>(R.O->CFORM());
		if(skeletion)
		{
			xr_vector<CCF_OBB>::iterator it = std::find_if(skeletion->_GetElements().begin(),
				skeletion->_GetElements().end(), CFindByIDPred(R.element) );
			VERIFY(skeletion->_GetElements().end() != it);
			CCF_OBB& ccf_obb = *it;
			hit_normal.sub(end_point, ccf_obb.OBB.m_translate);
			if(!fis_zero(hit_normal.magnitude()))
				hit_normal.normalize();
			else
				hit_normal.invert	(bullet->dir);
		}
	} 
	else 
	{
		//��������� ������� � ���������� �����������
		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+R.element;
		hit_normal.mknormal	(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
	}		
	//----------- normal - end
	float old_speed, energy_lost;
	old_speed = bullet->speed;

	//����������� ���������� ���� � �������� ��������
	float speed_factor = bullet->speed/bullet->max_speed;
	//�������� ���� ���� �������� � ������ �������
	float power = bullet->hit_power*speed_factor;

	SGameMtl* mtl = GMLib.GetMaterialByIdx(target_material);

	//shoot_factor: ����������������� ��������� (1 - ��������� ���������������)
	float shoot_factor = mtl->fShootFactor * bullet->pierce;
	clamp(shoot_factor, 0.f, 1.f);
	//material_pierce: 0 - ��������� ���������������
	float material_pierce = 1.f - shoot_factor;
	clamp(material_pierce, 0.f, 1.f);

	float impulse	= 0.f;

#ifdef DEBUG
	Fvector dbg_bullet_pos;
	dbg_bullet_pos.mad(bullet->pos,bullet->dir,R.range);
	int bullet_state		= 0;
#endif
	//�������
	Fvector			new_dir;
	new_dir.reflect	(bullet->dir,hit_normal);
	Fvector			tgt_dir;
	random_dir		(tgt_dir, new_dir, deg2rad(10.f));

	float ricoshet_factor	= bullet->dir.dotproduct(tgt_dir);

	float f			= Random.randF	(0.5f,1.f);
//	if(shoot_factor<RICOCHET_THRESHOLD &&  )
	if (((f+shoot_factor)<ricoshet_factor) && bullet->flags.allow_ricochet)	{
		//���������� �������� ������ � ����������� 
		//�� ���� ������� ���� (��� ������ ����, ��� ������ ������)
		float scale = 1.f -_abs(bullet->dir.dotproduct(hit_normal))*m_fCollisionEnergyMin;
		clamp(scale, 0.f, m_fCollisionEnergyMax);

		//���������� ��������, �������� ������� ������,
		//�.�. ���� �������� � ����� ������������
		//� ����� ������� �� RayQuery()
		bullet->dir.set	(tgt_dir)		;
		bullet->pos		= end_point		;
		bullet->flags.ricochet_was = 1	;

		//��������� �������� � ����������� �� �����������������
		bullet->speed *= material_pierce*scale;
		//������� ������� � ��������� �������� ���� ��� ������������
		float energy_lost = 1.f - bullet->speed/old_speed;
		//������� ���������� ������� ����� ������������������� ���������� �������
		impulse = bullet->hit_impulse*speed_factor*energy_lost;

		#ifdef DEBUG
		bullet_state = 0;
		#endif		
	} else if(shoot_factor <  STUCK_THRESHOLD) {
		//����������� ���� � ���������
		bullet->speed  = 0.f;
		//�������� ���� ������� �������
		impulse = bullet->hit_impulse*speed_factor;
		#ifdef DEBUG
		bullet_state = 1;
		#endif		
	} else {
		//���������� ���������
		//��������� �������� ��������������� ����������� ��������
		//float speed_lost = fis_zero(bullet->hit_impulse) ?	1.f : 		1.f - impulse/bullet->hit_impulse;
		//clamp (speed_lost, 0.f , 1.f);
		//float speed_lost = shoot_factor;
		
		bullet->speed *= shoot_factor;
		energy_lost = 1.f - bullet->speed/old_speed;
		impulse = bullet->hit_impulse*speed_factor*energy_lost;
		
		bullet->pos.mad(bullet->pos,bullet->dir,EPS);//fake
		//������ ����������� ����������� ��� ��������������
		Fvector rand_normal;
		rand_normal.random_dir(bullet->dir, deg2rad(5.f)*energy_lost, Random);
		bullet->dir.set(rand_normal);
		#ifdef DEBUG
		bullet_state = 2;
		#endif		
	}
#ifdef DEBUG
	extern g_bDrawBulletHit;
	if(g_bDrawBulletHit)
		g_hit[bullet_state].push_back(dbg_bullet_pos);
#endif

	return std::make_pair(power, impulse);
}
