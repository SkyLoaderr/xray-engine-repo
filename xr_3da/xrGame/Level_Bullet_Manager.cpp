// Level_Bullet_Manager.cpp:	��� ����������� ������ ���� �� ����������
//								��� ���� � ������� ���������� ����
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Level_Bullet_Manager.h"


#define STEP_TIME 33

#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f



SBullet::SBullet(const Fvector& position,
				const Fvector& direction,
				float start_speed,
				float power,
				float impulse,
				u16	sender_id,
				ALife::EHitType e_hit_type,
				const CCartridge& cartridge)
{
	pos = position;

	speed = start_speed;
	VERIFY(speed>0);
	
	dir = direction;
	VERIFY(dir.magnitude()>0);
	dir.normalize();

	hit_power		= power;
	hit_impulse		= impulse;
	parent_id		= sender_id;
	hit_type		= e_hit_type;
	
	dist_k			= cartridge.m_kDist;
	hit_k			= cartridge.m_kHit;
	impulse_k		= cartridge.m_kImpulse;
	pierce_k		= cartridge.m_kPierce;
	tracer			= cartridge.m_tracer;
	wallmark_size	= cartridge.fWallmarkSize;
}

SBullet::~SBullet()
{
}

//////////////////////////////////////////////////////////
//

CBulletManager::CBulletManager()
{
	m_dwStepTime = STEP_TIME;
	Device.seqRender.Add		(this,REG_PRIORITY_LOW-1000);
}

CBulletManager::~CBulletManager()
{
	Device.seqRender.Remove		(this);
}


void CBulletManager::Clear		()
{
	for(BULLET_LIST_it it = m_BulletList.begin();
		it != m_BulletList.end() ;it++)
	{
		xr_delete(*it);
	}
	m_BulletList.clear();
}

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
	if(bullet->hit_power>HIT_POWER_EPSILON)
		return TRUE;
	else
		return FALSE;
}



void CBulletManager::FireShotmark (const SBullet* bullet, const Fvector& vDir, const Fvector &vEnd, Collide::rq_result& R, u16 target_material) 
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
}





void CBulletManager::StaticObjectHit(SBullet* bullet, const Fvector& end_point, Collide::rq_result& R, u16 target_material)
{
	FireShotmark(bullet, bullet->dir, end_point, R, target_material);

	SGameMtl* mtl = GMLib.GetMaterialByIdx(target_material);

	float shoot_factor = mtl->fShootFactor;
	float material_pierce = 1.f - shoot_factor * bullet->pierce_k;
	clamp(material_pierce, 0.f, 1.f);

	bullet->hit_power	*= mtl->fShootFactor;
	bullet->hit_power	*= (shoot_factor*bullet->pierce_k);
	bullet->hit_impulse *= material_pierce;
}


void CBulletManager::DynamicObjectHit (SBullet* bullet, const Fvector& end_point, Collide::rq_result& R, u16 target_material)
{
	//������ ��� ������������ ��������
	VERIFY(R.O);

	//���� �� ������ �� �������� �� ������ ��
	//�����, �� ������������ ���, ��� ��� ��� ��
	//� �������
	if(R.O->ID() == bullet->parent_id) 
		return;

	SGameMtl* mtl = GMLib.GetMaterialByIdx(target_material);
	float shoot_factor = mtl->fShootFactor;
	
	//�������� ���� ���� �������� � ������ �������
	float power = bullet->hit_power * bullet->pierce_k * bullet->hit_k;

	//����������� ���������� ���� � �����������
	float scale = 1.f;
	/*
	float scale = 1-(R.range/(bullet->fire_dist*bullet->dist_k));
	clamp(scale,0.f,1.f);
	//!!! ������ �� ����� ���������� ������
	scale = _sqrt(scale);
	power *= scale;
	*/

	//���� ���� ����������� ��������
	//����������� � ������ ������������� ����������
	float material_pierce = 1.f - shoot_factor * bullet->pierce_k;
	clamp(material_pierce, 0.f, 1.f);
	float impulse = bullet->hit_impulse*material_pierce*
					bullet->impulse_k*scale;
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
		P.w_dir			(bullet->dir);
		P.w_float		(power);
		P.w_s16			((s16)R.element);
		P.w_vec3		(position_in_bone_space);
		P.w_float		(impulse);
		P.w_u16			(u16(bullet->hit_type));
		CGameObject::u_EventSend (P);
	}

	//���������� ����������� ��������� �� �������
	FireShotmark(bullet, bullet->dir, end_point, R, target_material);

	//��������� ��� � ������� ����� ��� ��� �������� ��� ������ 
	bullet->hit_power *= scale;
	bullet->hit_impulse *= scale;

	bullet->hit_power *= (shoot_factor*bullet->pierce_k);
	bullet->hit_impulse *= material_pierce;
}


void CBulletManager::AddBullet(SBullet* bullet)
{
	VERIFY(bullet);
	bullet->frame_num = Device.dwFrame;
	m_BulletList.push_back(bullet);
}

void CBulletManager::Update()
{
	u32 delta_time = Device.dwTimeDelta + m_dwTimeRemainder;
	u32 step_num = delta_time/m_dwStepTime;
	m_dwTimeRemainder = delta_time%m_dwStepTime;
	

	for(BULLET_LIST_it it = m_BulletList.begin();
						it != m_BulletList.end();)
	{
		BULLET_LIST_it cur_it = it;
		SBullet* bullet = *cur_it;
		
		//��� ���� �������� �� ���� �� ����� ������� ������ 1 ���
		//(���� �� ������ ������ ������ ������� �� ����)
		//������� ��������� �� ��������� �����, 
		//��� �������� ��� ���� ���� ��� ������� FPS �� ���������
		//� 2� ������
		u32 cur_step_num = step_num;

		u32 frames_pass = Device.dwFrame - bullet->frame_num;
		if(frames_pass == 0)
			cur_step_num = 1;
		else if (frames_pass == 1 && step_num>0)
			cur_step_num -= 1;

		for(u32 i=0; i<cur_step_num; i++)
		{
			if(!CalcBullet(bullet, m_dwStepTime))
			{
				xr_delete(bullet);
				it++;
				m_BulletList.erase(cur_it);
				break;
			}
		}
		if(!bullet) continue;
		it++;
	}
}

#define GRAVITY_CONST 9.81f

bool CBulletManager::CalcBullet (SBullet* bullet, u32 delta_time)
{
	VERIFY(bullet);

	float delta_time_sec = float(delta_time)/1000.f;
	float range = bullet->speed*delta_time_sec;
	
	Collide::ray_defs RD(bullet->pos, bullet->dir, range, 0 ,Collide::rqtBoth);
	Level().ObjectSpace.RayQuery( RD, firetrace_callback, bullet);
	
		
	//�������� ��������� ����
	bullet->prev_pos = bullet->pos;
	bullet->pos.mad(bullet->pos, bullet->dir, range);


	//�������� �������� � ����������� �� ������
	//� ������ ����������
	bullet->dir.y -= (GRAVITY_CONST*delta_time_sec)/bullet->speed;
	

	if(bullet->hit_power<HIT_POWER_EPSILON)
		return false;

	

	Fbox level_box = Level().ObjectSpace.GetBoundingVolume();
	if(!level_box.contains(bullet->pos))
		return false;

	return true;
}

void CBulletManager::OnRender	()
{
	for(BULLET_LIST_it it = m_BulletList.begin();
		it != m_BulletList.end(); it++)
	{
		SBullet* bullet = *it;
		RCache.dbg_DrawLINE(Fidentity,bullet->prev_pos,bullet->pos,D3DCOLOR_XRGB(0,255,0));
	}
}