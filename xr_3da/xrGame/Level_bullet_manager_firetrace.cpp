// Level_Bullet_Manager.cpp:	для обеспечения полета пули по траектории
//								все пули и осколки передаются сюда
//								(для просчета столкновений и их визуализации)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Level_Bullet_Manager.h"
#include "entity.h"
#include "gamemtllib.h"
#include "level.h"
#include "xrmessages.h"

//константы shoot_factor, определяющие 
//поведение пули при столкновении с объектом
#define RICOCHET_THRESHOLD		0.1
#define STUCK_THRESHOLD			0.4

//расстояния не пролетев которого пуля не трогает того кто ее пустил
#define PARENT_IGNORE_DIST 3.f

//callback функция 
//	result.O;		// 0-static else CObject*
//	result.range;	// range from start to element 
//	result.element;	// if (O) "num tri" else "num bone"
//	params;			// user defined abstract data
//	Device.Statistic.TEST0.End();
//return TRUE-продолжить трассировку / FALSE-закончить трассировку
BOOL __stdcall CBulletManager::firetrace_callback(Collide::rq_result& result, LPVOID params)
{
	SBullet* bullet = (SBullet*)params;

	//вычислить точку попадания
	Fvector end_point;
	end_point.mad(bullet->pos, bullet->dir, result.range);

	u16 hit_material_idx = GAMEMTL_NONE_IDX;

	//динамический объект
	if(result.O)
	{
		//получить косточку и ее материал
		CKinematics* V = 0;
		if (0!=(V=PKinematics(result.O->Visual())))
		{
			CBoneData& B = V->LL_GetData((u16)result.element);
			hit_material_idx = B.game_mtl_idx;
			Level().BulletManager().DynamicObjectHit(bullet, end_point, result, hit_material_idx);
		}
		/*else
		{
			hit_material_idx = GAMEMTL_NONE_IDX;
			Level().BulletManager().DynamicObjectHit(bullet, end_point, result, hit_material_idx);
		}*/
	}
	//статический объект
	else
	{
		//получить треугольник и узнать его материал
		CDB::TRI* T			= Level().ObjectSpace.GetStaticTris()+result.element;
		hit_material_idx	= T->material;
		Level().BulletManager().StaticObjectHit(bullet, end_point, result, hit_material_idx);
	}

	//проверить достаточно ли силы хита, чтобы двигаться дальше
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


void CBulletManager::FireShotmark (const SBullet* bullet, const Fvector& vDir, const Fvector &vEnd, Collide::rq_result& R, u16 target_material, Fvector& vNormal)
{
	SGameMtlPair* mtl_pair	= GMLib.GetMaterialPair(bullet->bullet_material_idx, target_material);

	Fvector particle_dir;

	if (R.O) 
	{
		if (R.O->CLS_ID==CLSID_ENTITY)
		{
			//тут добавить отметки крови на живой сущности
		}
		particle_dir = vDir;
		particle_dir.invert();

		//вернуть нормаль по которой играли партиклы и ставили валмарки

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
	} 
	else 
	{
		//вычислить нормаль к пораженной поверхности
		Fvector N;
		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
		particle_dir = N;

		//вернуть нормаль по которой играли партиклы и ставили валмарки
		vNormal = particle_dir;


		ref_shader* pWallmarkShader = (!mtl_pair || mtl_pair->CollideMarks.empty())?
NULL:&mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];;

		if (pWallmarkShader)
		{
			//добавить отметку на материале
			::Render->add_Wallmark	(*pWallmarkShader, vEnd,
				bullet->wallmark_size, pTri, pVerts);
		}
	}		

	ref_sound* pSound = (!mtl_pair || mtl_pair->CollideSounds.empty())?
NULL:&mtl_pair->CollideSounds[::Random.randI(0,mtl_pair->CollideSounds.size())];

	//проиграть звук
	if(pSound)
	{
		CObject* O = Level().Objects.net_Find(bullet->parent_id );
		pSound->play_at_pos_unlimited(O, vEnd, false);
	}

	LPCSTR ps_name = (!mtl_pair || mtl_pair->CollideParticles.empty())?
NULL:*mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];

	if(ps_name)
	{
		//отыграть партиклы попадания в материал
		CParticlesObject* ps = xr_new<CParticlesObject>(ps_name);

		Fmatrix pos;
		pos.k.normalize(particle_dir);
		Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);
		pos.c.set(vEnd);

		ps->UpdateParent(pos,zero_vel);
		Level().ps_needtoplay.push_back(ps);
	}
}

void CBulletManager::StaticObjectHit(SBullet* bullet, const Fvector& end_point, Collide::rq_result& R, u16 target_material)
{
	Fvector hit_normal;
	FireShotmark(bullet, bullet->dir, end_point, R, target_material, hit_normal);
	ObjectHit(bullet, end_point, R, target_material, hit_normal);
}


void CBulletManager::DynamicObjectHit (SBullet* bullet, const Fvector& end_point, Collide::rq_result& R, u16 target_material)
{
	//только для динамических объектов
	VERIFY(R.O);

	//если мы попали по родителю на первых же
	//кадре, то игнорировать это, так как это он
	//и стрелял
	if(R.O->ID() == bullet->parent_id &&  bullet->fly_dist<PARENT_IGNORE_DIST)
		return;

	//визуальное обозначение попадание на объекте
	Fvector hit_normal;
	FireShotmark(bullet, bullet->dir, end_point, R, target_material, hit_normal);

	Fvector original_dir = bullet->dir;
	float power, impulse;
	std::pair<float,float> hit_result = ObjectHit(bullet, end_point, R, target_material, hit_normal);
	power = hit_result.first;
	impulse = hit_result.second;


	CEntity* E = smart_cast<CEntity*>(R.O);
	//учитываем попадание в разные части 
	if(E) power *= E->HitScale(R.element);

	// object-space
	//вычислить координаты попадания
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

	//отправить хит пораженному объекту
	if(OnServer())
	{
		NET_Packet		P;
		CGameObject::u_EventGen	(P,GE_HIT,R.O->ID());
		P.w_u16			(bullet->parent_id);
		P.w_u16			(bullet->weapon_id);
		P.w_dir			(original_dir);
		P.w_float		(power);
		P.w_s16			((s16)R.element);
		P.w_vec3		(position_in_bone_space);
		P.w_float		(impulse);
		P.w_u16			(u16(bullet->hit_type));
		CGameObject::u_EventSend (P);
	}
}


std::pair<float, float>  CBulletManager::ObjectHit	(SBullet* bullet, const Fvector& end_point, 
									Collide::rq_result& R, u16 target_material, 
									const Fvector& hit_normal)
{
	float old_speed, energy_lost;

	old_speed = bullet->speed;

	//коэффициент уменьшение силы с падением скорости
	float speed_factor = bullet->speed/bullet->max_speed;
	//получить силу хита выстрела с учетом патрона
	float power = bullet->hit_power*bullet->hit_k*speed_factor;

	SGameMtl* mtl = GMLib.GetMaterialByIdx(target_material);

	//shoot_factor: простреливаемость материала (1 - полностью простреливаемый)
	float shoot_factor = mtl->fShootFactor * bullet->pierce_k;
	clamp(shoot_factor, 0.f, 1.f);
	//material_pierce: 0 - полностью простреливаемый
	float material_pierce = 1.f - shoot_factor;
	clamp(material_pierce, 0.f, 1.f);

	float impulse;

	//рикошет
	if(shoot_factor<RICOCHET_THRESHOLD && 
		bullet->flags.test(SBullet::RICOCHET_ENABLED_FLAG))
	{
		//уменьшение скорости полета в зависимости 
		//от угла падения пули (чем прямее угол, тем больше потеря)
		float scale = 1.f -_abs(bullet->dir.dotproduct(hit_normal))*m_fCollisionEnergyMin;
		clamp(scale, 0.f, m_fCollisionEnergyMax);

		//вычисление рикошета, делается немного фейком,
		//т.к. пуля остается в точке столкновения
		//и сразу выходит из RayQuery()
		Fvector rand_normal;
		rand_normal.random_dir(hit_normal, PI_DIV_4, Random);
		rand_normal.add(hit_normal);
		rand_normal.normalize();

		bullet->dir.mad(rand_normal,-2*bullet->dir.dotproduct(hit_normal));

		bullet->prev_pos = bullet->pos;
		bullet->pos = end_point;
		bullet->flags.set(SBullet::RICOCHET_FLAG, 1);


		//уменьшить скорость в зависимости от простреливаемости
		bullet->speed *= material_pierce*scale;
		//сколько энергии в процентах потеряла пуля при столкновении
		float energy_lost = 1.f - bullet->speed/old_speed;
		//импульс переданный объекту равен прямопропорционален потерянной энергии
		impulse = bullet->hit_impulse*speed_factor*bullet->impulse_k*energy_lost;
	}
	//застрявание пули в материале
	else if(shoot_factor <  STUCK_THRESHOLD)
	{
		bullet->speed  = 0.f;
		//передаем весь импульс целиком
		impulse = bullet->hit_impulse*bullet->impulse_k*speed_factor;
	}
	//пробивание материала
	else
	{
		//уменьшить скорость пропорцианально потраченому импульсу
		//float speed_lost = fis_zero(bullet->hit_impulse) ?	1.f : 		1.f - impulse/bullet->hit_impulse;
		//clamp (speed_lost, 0.f , 1.f);
		//float speed_lost = shoot_factor;
		
		bullet->speed *= shoot_factor;
		energy_lost = 1.f - bullet->speed/old_speed;
		impulse = bullet->hit_impulse*bullet->impulse_k*speed_factor*energy_lost;

		//ввести коэффициент случайности при простреливании
		Fvector rand_normal;
		rand_normal.random_dir(bullet->dir, PI_DIV_6*energy_lost, Random);
		bullet->dir.set(rand_normal);
	}


	return std::make_pair(power, impulse);
}
