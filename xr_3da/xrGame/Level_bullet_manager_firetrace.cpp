// Level_Bullet_Manager.cpp:	��� ����������� ������ ���� �� ����������
//								��� ���� � ������� ���������� ����
//								(��� �������� ������������ � �� ������������)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Level_Bullet_Manager.h"



//callback ������� 
//	result.O;		// 0-static else CObject*
//	result.range;	// range from start to element 
//	result.element;	// if (O) "num tri" else "num bone"
//	params;			// user defined abstract data
//	Device.Statistic.TEST0.End();
//return TRUE-���������� ����������� / FALSE-��������� �����������
BOOL __stdcall CBulletManager::firetrace_callback(Collide::rq_result& result, LPVOID params)
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
		if (0!=(V=PKinematics(result.O->Visual())))
		{
			CBoneData& B = V->LL_GetData((u16)result.element);
			hit_material_idx = B.game_mtl_idx;
			Level().BulletManager().DynamicObjectHit(bullet, end_point, result, hit_material_idx);
		}
		else
		{
			hit_material_idx = 0*GAMEMTL_NONE_IDX;
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



void CBulletManager::FireShotmark (const SBullet* bullet, const Fvector& vDir, const Fvector &vEnd, Collide::rq_result& R, u16 target_material, Fvector& vNormal)
{
	SGameMtlPair* mtl_pair	= GMLib.GetMaterialPair(bullet_material_idx, target_material);

	Fvector particle_dir;

	if (R.O) 
	{
		if (R.O->CLS_ID==CLSID_ENTITY)
		{
			//��� �������� ������� ����� �� ����� ��������
		}
		particle_dir = vDir;
		particle_dir.invert();
	} 
	else 
	{
		//��������� ������� � ���������� �����������
		Fvector N;
		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
		particle_dir = N;


		ref_shader* pWallmarkShader = (!mtl_pair || mtl_pair->CollideMarks.empty())?
NULL:&mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];;

		if (pWallmarkShader)
		{
			//�������� ������� �� ���������
			::Render->add_Wallmark	(*pWallmarkShader, vEnd,
				bullet->wallmark_size, pTri, pVerts);
		}
	}		

	ref_sound* pSound = (!mtl_pair || mtl_pair->CollideSounds.empty())?
NULL:&mtl_pair->CollideSounds[::Random.randI(0,mtl_pair->CollideSounds.size())];

	//��������� ����
	if(pSound)
	{
		CObject* O = Level().Objects.net_Find(bullet->parent_id );
		pSound->play_at_pos_unlimited(O, vEnd, false);
	}

	LPCSTR ps_name = (!mtl_pair || mtl_pair->CollideParticles.empty())?
NULL:*mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];

	if(ps_name)
	{
		//�������� �������� ��������� � ��������
		CParticlesObject* ps = xr_new<CParticlesObject>(ps_name);

		Fmatrix pos;
		pos.k.normalize(particle_dir);
		Fvector::generate_orthonormal_basis(pos.k, pos.i, pos.j);
		pos.c.set(vEnd);

		ps->UpdateParent(pos,zero_vel);
		Level().ps_needtoplay.push_back(ps);
	}

	//������� ������� �� ������� ������ �������� � ������� ��������
	vNormal = particle_dir;
}

void CBulletManager::StaticObjectHit(SBullet* bullet, const Fvector& end_point, Collide::rq_result& R, u16 target_material)
{
	Fvector hit_normal;
	FireShotmark(bullet, bullet->dir, end_point, R, target_material, hit_normal);

	SGameMtl* mtl = GMLib.GetMaterialByIdx(target_material);

	//����������������� ��������� (1 - ��������� ���������������)
	float shoot_factor = mtl->fShootFactor * bullet->pierce_k;
	clamp(shoot_factor, 0.f, 1.f);

	float material_pierce = 1.f - shoot_factor;
	clamp(material_pierce, 0.f, 1.f);


	if(bullet->flags.test(SBullet::RICOCHET_ENABLED_FLAG))
	{
		//���������� �������� ������ � ����������� 
		//�� ���� ������� ���� (��� ������ ����, ��� ������ ������)
		float scale = 1.f -_abs(bullet->dir.dotproduct(hit_normal))*m_fCollisionEnergyMin;
		clamp(scale, 0.f, m_fCollisionEnergyMax);
			
		//���������� ��������, �������� ������� ������,
		//�.�. ���� �������� � ����� ������������
		//� ����� ������� �� RayQuery()
		if(fis_zero(shoot_factor))
		{
			Fvector rand_normal;
			rand_normal.random_dir(hit_normal, 10.f, Random);
			rand_normal.add(hit_normal);
			rand_normal.normalize();

			bullet->dir.mad(rand_normal,-2*bullet->dir.dotproduct(hit_normal));
			
			//return mad(dir,norm,-2*dir.dotproduct(norm));
			//bullet->dir.reflect(bullet->dir, hit_normal);
			//bullet->dir.random_dir(bullet->dir, 10.f, Random);
			
			bullet->prev_pos = bullet->pos;
			bullet->pos = end_point;
			bullet->flags.set(SBullet::RICOCHET_FLAG, 1);

			bullet->speed *= material_pierce*scale;
		}
		else
			bullet->speed *= shoot_factor*scale;
	}
	else
	{
		bullet->speed = 0;
	}
}

#define PARENT_IGNORE_DIST 3.f

void CBulletManager::DynamicObjectHit (SBullet* bullet, const Fvector& end_point, Collide::rq_result& R, u16 target_material)
{
	//������ ��� ������������ ��������
	VERIFY(R.O);

	//���� �� ������ �� �������� �� ������ ��
	//�����, �� ������������ ���, ��� ��� ��� ��
	//� �������
	if(R.O->ID() == bullet->parent_id &&  bullet->fly_dist<PARENT_IGNORE_DIST)
		return;

	//����������� ���������� ���� � �������� ��������
	float speed_factor = bullet->speed/bullet->max_speed;

	SGameMtl* mtl = GMLib.GetMaterialByIdx(target_material);
	float shoot_factor = mtl->fShootFactor;

	//�������� ���� ���� �������� � ������ �������
	float power = bullet->hit_power*bullet->pierce_k*bullet->hit_k*speed_factor;

	//���� ���� ����������� ��������
	//����������� � ������ ������������� ����������
	float material_pierce = 1.f - shoot_factor * bullet->pierce_k;
	clamp(material_pierce, 0.1f, 1.f);
	float impulse = bullet->hit_impulse*material_pierce*
		bullet->impulse_k*speed_factor;

	//��������� �������� ��������������� ����������� ��������
	float speed_lost =fis_zero(bullet->hit_impulse) ?	1.f : 1.f - impulse/bullet->hit_impulse;
	clamp (speed_lost, 0.f , 1.f);
	bullet->speed *= speed_lost;

	VERIFY(impulse>=0);

	CEntity* E = dynamic_cast<CEntity*>(R.O);
	//��������� ��������� � ������ ����� 
	if(E) power *= E->HitScale(R.element);

	// object-space
	//��������� ���������� ���������
	Fvector p_in_object_space,position_in_bone_space;
	Fmatrix m_inv;
	m_inv.invert(R.O->XFORM());
	m_inv.transform_tiny(p_in_object_space, end_point);

	// bone-space
	CKinematics* V = PKinematics(R.O->Visual());

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
	if(OnServer())
	{
		NET_Packet		P;
		CGameObject::u_EventGen	(P,GE_HIT,R.O->ID());
		P.w_u16			(bullet->parent_id);
		P.w_u16			(bullet->weapon_id);
		P.w_dir			(bullet->dir);
		P.w_float		(power);
		P.w_s16			((s16)R.element);
		P.w_vec3		(position_in_bone_space);
		P.w_float		(impulse);
		P.w_u16			(u16(bullet->hit_type));
		CGameObject::u_EventSend (P);
	}

	//���������� ����������� ��������� �� �������
	Fvector hit_normal;
	FireShotmark(bullet, bullet->dir, end_point, R, target_material, hit_normal);


}