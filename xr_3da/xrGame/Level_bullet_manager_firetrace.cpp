// Level_Bullet_Manager.cpp:	для обеспечения полета пули по траектории
//								все пули и осколки передаются сюда
//								(для просчета столкновений и их визуализации)
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

//константы shoot_factor, определяющие 
//поведение пули при столкновении с объектом
#define RICOCHET_THRESHOLD		0.1
#define STUCK_THRESHOLD			0.4

//расстояния не пролетев которого пуля не трогает того кто ее пустил
#define PARENT_IGNORE_DIST		3.f

//test callback функция 
//  object - object for testing
//return TRUE-тестировать объект / FALSE-пропустить объект
BOOL __stdcall CBulletManager::test_callback(CObject* object, LPVOID params)
{
	SBullet* bullet = (SBullet*)params;
	if( (object->ID() == bullet->parent_id)		&&  
		(bullet->fly_dist<PARENT_IGNORE_DIST)	&&
		(!bullet->flags.is(SBullet::RICOCHET_FLAG)))		return FALSE;

	// whine sounds
	if (object){
		CEntity*	entity			= smart_cast<CEntity*>(object);
		if (entity&&entity->g_Alive()&&(entity->ID()!=bullet->parent_id)){
			ICollisionForm*	cform	= entity->collidable.model;
			ECollisionFormType tp	= cform->Type();
			if ((tp==cftObject)&&(smart_cast<CAI_Stalker*>(entity)||smart_cast<CActor*>(entity))){
				Fsphere S			= cform->getSphere();
				entity->XFORM().transform_tiny	(S.P)	;
				float dist			= 1000.f;
				if (Fsphere::rpNone!=S.intersect(bullet->pos, bullet->dir, dist)){
					Fvector			pt;
					pt.mad			(bullet->pos, bullet->dir, dist);
					CObject* initiator	= Level().Objects.net_Find	(bullet->parent_id);
					Level().BulletManager().PlayWhineSound			(bullet,initiator,pt);
				}
			}
		}
	}
	
	return TRUE;
}
//callback функция 
//	result.O;		// 0-static else CObject*
//	result.range;	// range from start to element 
//	result.element;	// if (O) "num tri" else "num bone"
//	params;			// user defined abstract data
//	Device.Statistic.TEST0.End();
//return TRUE-продолжить трассировку / FALSE-закончить трассировку
BOOL __stdcall CBulletManager::firetrace_callback(collide::rq_result& result, LPVOID params)
{
	SBullet* bullet = (SBullet*)params;

	//вычислить точку попадания
	Fvector end_point;
	end_point.mad(bullet->pos, bullet->dir, result.range);

	u16 hit_material_idx = GAMEMTL_NONE_IDX;

	//динамический объект
	if(result.O){
		//получить косточку и ее материал
		CKinematics* V = 0;
		//если мы попали по родителю на первых же
		//кадре, то игнорировать это, так как это он
		//и стрелял
		VERIFY( !(result.O->ID() == bullet->parent_id &&  bullet->fly_dist<PARENT_IGNORE_DIST) );
		if (0!=(V=smart_cast<CKinematics*>(result.O->Visual()))){
			CBoneData& B = V->LL_GetData((u16)result.element);
			hit_material_idx = B.game_mtl_idx;
			Level().BulletManager().DynamicObjectHit(bullet, end_point, result, hit_material_idx);
		}
	} else {
		//статический объект
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


void CBulletManager::FireShotmark (SBullet* bullet, const Fvector& vDir, const Fvector &vEnd, collide::rq_result& R, u16 target_material, Fvector& vNormal, bool ShowMark)
{
	SGameMtlPair* mtl_pair	= GMLib.GetMaterialPair(bullet->bullet_material_idx, target_material);
	Fvector particle_dir;

	if (R.O)
	{
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

		//на текущем актере отметок не ставим
		if(Level().CurrentEntity() && Level().CurrentEntity()->ID() == R.O->ID()) return;

		ref_shader* pWallmarkShader = (!mtl_pair || mtl_pair->CollideMarks.empty())?
						NULL:&mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];;

		if (pWallmarkShader && ShowMark)
		{
			//добавить отметку на материале
			Fvector p;
			p.mad(bullet->pos,bullet->dir,R.range-0.01f);
			::Render->add_SkeletonWallmark	(&R.O->renderable.xform, 
							PKinematics(R.O->Visual()), *pWallmarkShader,
							p, bullet->dir, bullet->wallmark_size);
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

		if (pWallmarkShader && ShowMark)
		{
			//добавить отметку на материале
			::Render->add_StaticWallmark	(*pWallmarkShader, vEnd,
				bullet->wallmark_size, pTri, pVerts);
		}
	}		

	ref_sound* pSound = (!mtl_pair || mtl_pair->CollideSounds.empty())?
						NULL:&mtl_pair->CollideSounds[::Random.randI(0,mtl_pair->CollideSounds.size())];

	//проиграть звук
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

	if( (ps_name && ShowMark) || (bullet->flags.test(SBullet::EXPLOSIVE_FLAG)&&bStatic) )
	{
		Fmatrix pos;
		pos.k.normalize(particle_dir);
		Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);
		pos.c.set(vEnd);
		if(ps_name && ShowMark){
			//отыграть партиклы попадания в материал
			CParticlesObject* ps = xr_new<CParticlesObject>(ps_name,TRUE);

			ps->UpdateParent(pos,zero_vel);
			GamePersistent().ps_needtoplay.push_back(ps);
		}

		if(bullet->flags.test(SBullet::EXPLOSIVE_FLAG)&&bStatic)
			PlayExplodePS(pos);
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
	//только для динамических объектов
	VERIFY(R.O);

	bool NeedShootmark = true;
	
	if (R.O->CLS_ID == CLSID_OBJECT_ACTOR)
	{
		game_PlayerState* ps = Game().GetPlayerByGameID(R.O->ID());
		if (ps && ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE))
		{
			NeedShootmark = false;
		};
	}
	
	//визуальное обозначение попадание на объекте
	Fvector hit_normal;
	FireShotmark(bullet, bullet->dir, end_point, R, target_material, hit_normal, NeedShootmark);
	
	Fvector original_dir = bullet->dir;
	float power, impulse;
	std::pair<float,float> hit_result = ObjectHit(bullet, end_point, R, target_material, hit_normal);
	power = hit_result.first;
	impulse = hit_result.second;

	// object-space
	//вычислить координаты попадания
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

	//отправить хит пораженному объекту
//	if(!OnServer())
	if (bullet->m_bSendHit)
	{
		//-------------------------------------------------
		NET_Packet		P;
		bool AddStatistic = false;
		if (GameID() != GAME_SINGLE && bullet->m_bSendHit && R.O->CLS_ID == CLSID_OBJECT_ACTOR
			&& Game().m_WeaponUsageStatistic.CollectData())
		{
			CActor* pActor = smart_cast<CActor*>(R.O);
			if (pActor && pActor->g_Alive())
			{
				Game().m_WeaponUsageStatistic.OnBullet_Hit(bullet, R.O->ID(), (s16)R.element, end_point);
				AddStatistic = true;
			};
		};
		
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

extern void random_dir(Fvector& tgt_dir, const Fvector& src_dir, float dispersion);

std::pair<float, float>  CBulletManager::ObjectHit	(SBullet* bullet, const Fvector& end_point, 
									collide::rq_result& R, u16 target_material, 
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

	float impulse	= 0.f;

#ifdef DEBUG
	Fvector dbg_bullet_pos;
	dbg_bullet_pos.mad(bullet->pos,bullet->dir,R.range);
	int bullet_state		= 0;
#endif
	//рикошет
	Fvector			new_dir;
	new_dir.reflect	(bullet->dir,hit_normal);
	Fvector			tgt_dir;
	random_dir		(tgt_dir, new_dir, deg2rad(10.f));

	float ricoshet_factor	= bullet->dir.dotproduct(tgt_dir);

	float f			= Random.randF	(-1.f,1.f);
//	if(shoot_factor<RICOCHET_THRESHOLD &&  )
	if (((f+shoot_factor)<ricoshet_factor) && bullet->flags.test(SBullet::RICOCHET_ENABLED_FLAG)){
		//уменьшение скорости полета в зависимости 
		//от угла падения пули (чем прямее угол, тем больше потеря)
		float scale = 1.f -_abs(bullet->dir.dotproduct(hit_normal))*m_fCollisionEnergyMin;
		clamp(scale, 0.f, m_fCollisionEnergyMax);

		//вычисление рикошета, делается немного фейком,
		//т.к. пуля остается в точке столкновения
		//и сразу выходит из RayQuery()
		bullet->dir.set	(tgt_dir);
		bullet->prev_pos= bullet->pos;
		bullet->pos		= end_point;
		bullet->flags.set(SBullet::RICOCHET_FLAG, 1);

		//уменьшить скорость в зависимости от простреливаемости
		bullet->speed *= material_pierce*scale;
		//сколько энергии в процентах потеряла пуля при столкновении
		float energy_lost = 1.f - bullet->speed/old_speed;
		//импульс переданный объекту равен прямопропорционален потерянной энергии
		impulse = bullet->hit_impulse*speed_factor*bullet->impulse_k*energy_lost;

		#ifdef DEBUG
		bullet_state = 0;
		#endif		
	} else if(shoot_factor <  STUCK_THRESHOLD) {
		//застрявание пули в материале
		bullet->speed  = 0.f;
		//передаем весь импульс целиком
		impulse = bullet->hit_impulse*bullet->impulse_k*speed_factor;
		#ifdef DEBUG
		bullet_state = 1;
		#endif		
	} else {
		//пробивание материала
		//уменьшить скорость пропорцианально потраченому импульсу
		//float speed_lost = fis_zero(bullet->hit_impulse) ?	1.f : 		1.f - impulse/bullet->hit_impulse;
		//clamp (speed_lost, 0.f , 1.f);
		//float speed_lost = shoot_factor;
		
		bullet->speed *= shoot_factor;
		energy_lost = 1.f - bullet->speed/old_speed;
		impulse = bullet->hit_impulse*bullet->impulse_k*speed_factor*energy_lost;
		
		bullet->pos.mad(bullet->pos,bullet->dir,EPS);//fake
		//ввести коэффициент случайности при простреливании
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
