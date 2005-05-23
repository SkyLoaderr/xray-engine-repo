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

//��������� shoot_factor, ������������ 
//��������� ���� ��� ������������ � ��������
#define RICOCHET_THRESHOLD		0.1
#define STUCK_THRESHOLD			0.4

//���������� �� �������� �������� ���� �� ������� ���� ��� �� ������
#define PARENT_IGNORE_DIST 3.f

//callback ������� 
//	result.O;		// 0-static else CObject*
//	result.range;	// range from start to element 
//	result.element;	// if (O) "num tri" else "num bone"
//	params;			// user defined abstract data
//	Device.Statistic.TEST0.End();
//return TRUE-���������� ����������� / FALSE-��������� �����������
BOOL __stdcall CBulletManager::firetrace_callback(collide::rq_result& result, LPVOID params)
{
	SBullet* bullet = (SBullet*)params;

	//��������� ����� ���������
	Fvector end_point;
	end_point.mad(bullet->pos, bullet->dir, result.range);

	u16 hit_material_idx = GAMEMTL_NONE_IDX;

	//������������ ������
	if(result.O)
	{
		//�������� �������� � �� ��������
		CKinematics* V = 0;
		//���� �� ������ �� �������� �� ������ ��
		//�����, �� ������������ ���, ��� ��� ��� ��
		//� �������
		if( !(result.O->ID() == bullet->parent_id &&  bullet->fly_dist<PARENT_IGNORE_DIST) )

			if (0!=(V=smart_cast<CKinematics*>(result.O->Visual())))
			{
				CBoneData& B = V->LL_GetData((u16)result.element);
				hit_material_idx = B.game_mtl_idx;
				Level().BulletManager().DynamicObjectHit(bullet, end_point, result, hit_material_idx);
			}
	}
	//����������� ������
	else
	{
		//�������� ����������� � ������ ��� ��������
		CDB::TRI* T			= Level().ObjectSpace.GetStaticTris()+result.element;
		hit_material_idx	= T->material;
		Level().BulletManager().StaticObjectHit(bullet, end_point, result, hit_material_idx);
	}

	//��������� ���������� �� ���� ����, ����� ��������� ������
	if(bullet->speed<m_fMinBulletSpeed || bullet->flags.test(SBullet::RICOCHET_FLAG))
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


void CBulletManager::FireShotmark (const SBullet* bullet, const Fvector& vDir, const Fvector &vEnd, collide::rq_result& R, u16 target_material, Fvector& vNormal, bool ShowMark)
{
	SGameMtlPair* mtl_pair	= GMLib.GetMaterialPair(bullet->bullet_material_idx, target_material);

	Fvector particle_dir;

	if (R.O)
	{
		particle_dir = vDir;
		particle_dir.invert();

		//������� ������� �� ������� ������ �������� � ������� ��������

		CCF_Skeleton* skeletion = smart_cast<CCF_Skeleton*>(R.O->CFORM());
		if(skeletion)
		{
			xr_vector<CCF_OBB>::iterator it = std::find_if(skeletion->_GetElements().begin(),
				skeletion->_GetElements().end(),
				CFindByIDPred(R.element));
			VERIFY(skeletion->_GetElements().end() != it);
			CCF_OBB& ccf_obb = *it;
			vNormal.sub(vEnd, ccf_obb.OBB.m_translate);
			if(!fis_zero(vNormal.magnitude()))
				vNormal.normalize();
			else
				vNormal = particle_dir;
		}

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
		Fvector N;
		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
		particle_dir = N;

		//������� ������� �� ������� ������ �������� � ������� ��������
		vNormal = particle_dir;

		ref_shader* pWallmarkShader = (!mtl_pair || mtl_pair->CollideMarks.empty())?
									NULL:&mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];;

		if (pWallmarkShader && ShowMark)
		{
			//�������� ������� �� ���������
			::Render->add_StaticWallmark	(*pWallmarkShader, vEnd,
				bullet->wallmark_size, pTri, pVerts);
		}
	}		

	ref_sound* pSound = (!mtl_pair || mtl_pair->CollideSounds.empty())?
						NULL:&mtl_pair->CollideSounds[::Random.randI(0,mtl_pair->CollideSounds.size())];

	//��������� ����
	if(pSound && ShowMark)
	{
		CObject* O = Level().Objects.net_Find(bullet->parent_id );
		pSound->play_at_pos_unlimited(O, vEnd, false);
	}

	LPCSTR ps_name = (!mtl_pair || mtl_pair->CollideParticles.empty())?
NULL:*mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];

	if(ps_name && ShowMark)
	{
		//�������� �������� ��������� � ��������
		CParticlesObject* ps = xr_new<CParticlesObject>(ps_name,TRUE);

		Fmatrix pos;
		pos.k.normalize(particle_dir);
		Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);
		pos.c.set(vEnd);

		ps->UpdateParent(pos,zero_vel);
		GamePersistent().ps_needtoplay.push_back(ps);
	}
}

void CBulletManager::StaticObjectHit(SBullet* bullet, const Fvector& end_point, collide::rq_result& R, u16 target_material)
{
	Fvector hit_normal;
	FireShotmark(bullet, bullet->dir, end_point, R, target_material, hit_normal);
	ObjectHit(bullet, end_point, R, target_material, hit_normal);
}


void CBulletManager::DynamicObjectHit (SBullet* bullet, const Fvector& end_point, collide::rq_result& R, u16 target_material)
{
	//������ ��� ������������ ��������
	VERIFY(R.O);

	//���� �� ������ �� �������� �� ������ ��
	//�����, �� ������������ ���, ��� ��� ��� ��
	//� �������
// moved to firetrace_callback
//	if(R.O->ID() == bullet->parent_id &&  bullet->fly_dist<PARENT_IGNORE_DIST)
//		return;

	bool NeedShootmark = true;
	
	if (R.O->CLS_ID == CLSID_OBJECT_ACTOR)
	{
		game_PlayerState* ps = Game().GetPlayerByGameID(R.O->ID());
		if (ps && ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE))
		{
			NeedShootmark = false;
		};
	}
	
	//���������� ����������� ��������� �� �������
	Fvector hit_normal;
	FireShotmark(bullet, bullet->dir, end_point, R, target_material, hit_normal, NeedShootmark);
	
	Fvector original_dir = bullet->dir;
	float power, impulse;
	std::pair<float,float> hit_result = ObjectHit(bullet, end_point, R, target_material, hit_normal);
	power = hit_result.first;
	impulse = hit_result.second;

	// object-space
	//��������� ���������� ���������
	Fvector p_in_object_space,position_in_bone_space;
	Fmatrix m_inv;
	m_inv.invert(R.O->XFORM());
	m_inv.transform_tiny(p_in_object_space, end_point);

	// bone-space
	CKinematics* V = smart_cast<CKinematics*>(R.O->Visual());

	if(V)
	{
		Fmatrix& m_bone = (V->LL_GetBoneInstance(u16(R.element))).mTransform;
		Fmatrix  m_inv_bone;
		m_inv_bone.invert(m_bone);
		m_inv_bone.transform_tiny(position_in_bone_space, p_in_object_space);
	}
	else
	{
		position_in_bone_space.set(p_in_object_space);
	}

	//��������� ��� ����������� �������
//	if(!OnServer())
	if (bullet->m_bSendHit)
	{
		//-------------------------------------------------
		NET_Packet		P;
		bool AddStatistic = false;
		if (GameID() != GAME_SINGLE && bullet->m_bSendHit && R.O->CLS_ID == CLSID_OBJECT_ACTOR
			&& Game().m_WeaponUsageStatistic.CollectData())
		{
			Game().m_WeaponUsageStatistic.OnBullet_Hit(bullet, R.O->ID(), (s16)R.element, end_point);
			AddStatistic = true;			
		}
		
		CGameObject::u_EventGen	(P,(AddStatistic)? GE_HIT_STATISTIC : GE_HIT,R.O->ID());
		P.w_u16			(bullet->parent_id);
		P.w_u16			(bullet->weapon_id);
		P.w_dir			(original_dir);
		P.w_float		(power);
		P.w_s16			((s16)R.element);
		P.w_vec3		(position_in_bone_space);
		P.w_float		(impulse);
		P.w_u16			(u16(bullet->hit_type));
		//-------------------------------------------------
		if (AddStatistic)
		{
			P.w_u32(bullet->m_dwID);
		};
		CGameObject::u_EventSend (P);
		//-------------------------------------------------
	}
}

#ifdef DEBUG
FvectorVec g_hit[3];
#endif

std::pair<float, float>  CBulletManager::ObjectHit	(SBullet* bullet, const Fvector& end_point, 
									collide::rq_result& R, u16 target_material, 
									const Fvector& hit_normal)
{
	float old_speed, energy_lost;

	old_speed = bullet->speed;

	//����������� ���������� ���� � �������� ��������
	float speed_factor = bullet->speed/bullet->max_speed;
	//�������� ���� ���� �������� � ������ �������
	float power = bullet->hit_power*bullet->hit_k*speed_factor;

	SGameMtl* mtl = GMLib.GetMaterialByIdx(target_material);

	//shoot_factor: ����������������� ��������� (1 - ��������� ���������������)
	float shoot_factor = mtl->fShootFactor * bullet->pierce_k;
	clamp(shoot_factor, 0.f, 1.f);
	//material_pierce: 0 - ��������� ���������������
	float material_pierce = 1.f - shoot_factor;
	clamp(material_pierce, 0.f, 1.f);

	float impulse;

#ifdef DEBUG
	Fvector dbg_bullet_pos;
	dbg_bullet_pos.mad(bullet->pos,bullet->dir,R.range);
	int bullet_state = 0;
#endif
	//�������
	if(shoot_factor<RICOCHET_THRESHOLD && 
		bullet->flags.test(SBullet::RICOCHET_ENABLED_FLAG))
	{
		//���������� �������� ������ � ����������� 
		//�� ���� ������� ���� (��� ������ ����, ��� ������ ������)
		float scale = 1.f -_abs(bullet->dir.dotproduct(hit_normal))*m_fCollisionEnergyMin;
		clamp(scale, 0.f, m_fCollisionEnergyMax);

		//���������� ��������, �������� ������� ������,
		//�.�. ���� �������� � ����� ������������
		//� ����� ������� �� RayQuery()
		Fvector rand_dir;
		rand_dir.random_dir(rand_dir, PI_DIV_4, Random);
		rand_dir.mul(0.1f);
		Fvector new_dir;
		bullet->dir.reflect(new_dir,hit_normal);
		new_dir.add(rand_dir);
		new_dir.normalize();
		bullet->dir.set(new_dir);
		bullet->prev_pos = bullet->pos;
		bullet->pos = end_point;
		bullet->flags.set(SBullet::RICOCHET_FLAG, 1);


		//��������� �������� � ����������� �� �����������������
		bullet->speed *= material_pierce*scale;
		//������� ������� � ��������� �������� ���� ��� ������������
		float energy_lost = 1.f - bullet->speed/old_speed;
		//������� ���������� ������� ����� ������������������� ���������� �������
		impulse = bullet->hit_impulse*speed_factor*bullet->impulse_k*energy_lost;

		#ifdef DEBUG
		bullet_state = 0;
		#endif		
	}
	//����������� ���� � ���������
	else if(shoot_factor <  STUCK_THRESHOLD)
	{
		bullet->speed  = 0.f;
		//�������� ���� ������� �������
		impulse = bullet->hit_impulse*bullet->impulse_k*speed_factor;
		#ifdef DEBUG
		bullet_state = 1;
		#endif		
	}
	//���������� ���������
	else
	{
		//��������� �������� ��������������� ����������� ��������
		//float speed_lost = fis_zero(bullet->hit_impulse) ?	1.f : 		1.f - impulse/bullet->hit_impulse;
		//clamp (speed_lost, 0.f , 1.f);
		//float speed_lost = shoot_factor;
		
		bullet->speed *= shoot_factor;
		energy_lost = 1.f - bullet->speed/old_speed;
		impulse = bullet->hit_impulse*bullet->impulse_k*speed_factor*energy_lost;
		
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
