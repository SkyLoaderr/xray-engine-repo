//////////////////////////////////////////////////////////////////////
// ShootingObject.cpp:  интерфейс дл€ семейства стрел€ющих объектов 
//						(оружие и осколочные гранаты) 	
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ShootingObject.h"

#include "ParticlesObject.h"
#include "WeaponAmmo.h"

#include "actor.h"

#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f

u16 CShootingObject::bullet_material_idx = GAMEMTL_NONE_IDX;

CShootingObject::CShootingObject(void)
{
	m_fCurrentHitPower = 0.0f;
	m_fCurrentHitImpulse = 0.0f;
	m_fCurrentFireDist = 0.0f;
	m_fCurrentWallmarkSize = WALLMARK_SIZE;
	m_pCurrentCartridge = NULL;
	m_vCurrentShootDir = Fvector().set(0,0,0);
	m_vCurrentShootPos = Fvector().set(0,0,0);
	m_vEndPoint = Fvector().set(0,0,0);
	m_iCurrentParentID = 0xFFFF;
	m_fCurrentHitType = ALife::eHitTypeFireWound;
}
CShootingObject::~CShootingObject(void)
{
}



void CShootingObject::FireShotmark (const Fvector& vDir, const Fvector &vEnd, Collide::rq_result& R, u16 target_material) 
{
	SGameMtlPair* mtl_pair	= GMLib.GetMaterialPair(bullet_material_idx, target_material);
	
	Fvector particle_dir;

	if (R.O) 
	{
		if (R.O->CLS_ID==CLSID_ENTITY)
		{
			//тут добавить отметки крови
		}
		particle_dir = vDir;
		particle_dir.invert();
	} 
	else 
	{
		//вычислить нормаль к пораженной поверхности
        Fvector N;
		Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
		particle_dir = N;

		ref_shader* pWallmarkShader = (!mtl_pair || mtl_pair->CollideMarks.empty())?
					 NULL:
					 &mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];;
		
		if (pWallmarkShader)
		{
			//добавить отметку на материале
			::Render->add_Wallmark	(*pWallmarkShader, vEnd,
				m_fCurrentWallmarkSize, pTri, pVerts);
		}
	}		

	ref_sound* pSound = (!mtl_pair || mtl_pair->CollideSounds.empty())?
				NULL:
				&mtl_pair->CollideSounds[::Random.randI(0,mtl_pair->CollideSounds.size())];
	//проиграть звук
	if(pSound)
	{
		pSound->play_at_pos_unlimited(this, vEnd, false);
	}

	LPCSTR ps_name = (!mtl_pair || mtl_pair->CollideParticles.empty())?
			NULL:
			*mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];
	if(ps_name)
	{

		//отыграть партиклы попадани€ в материал
		CParticlesObject* ps = xr_new<CParticlesObject>(ps_name, this->Sector());

		Fmatrix pos;
		pos.k.normalize(particle_dir);
		Fvector::generate_orthonormal_basis(pos.k, pos.i, pos.j);
		pos.c.set(vEnd);

		ps->UpdateParent(pos,zero_vel);
		Level().ps_needtoplay.push_back(ps);

	}
}


//callback функци€ 
//	result.O;		// 0-static else CObject*
//	result.range;	// range from start to element 
//	result.element;	// if (O) "num tri" else "num bone"
//	params;			// user defined abstract data
//	Device.Statistic.TEST0.End();
//return TRUE-продолжить трассировку / FALSE-закончить трассировку
BOOL __stdcall CShootingObject::firetrace_callback(Collide::rq_result& result, LPVOID params)
{

	CShootingObject* pThisWeapon = (CShootingObject*)params;
	//вычислить точку попадани€
	pThisWeapon->m_vEndPoint.mad(pThisWeapon->m_vCurrentShootPos,
		pThisWeapon->m_vCurrentShootDir,result.range);

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
			pThisWeapon->DynamicObjectHit(result, hit_material_idx);
		}
		else
		{
			hit_material_idx = 0*GAMEMTL_NONE_IDX;
			pThisWeapon->DynamicObjectHit(result, hit_material_idx);
		}
	}
	//статический объект
	else
	{
		//получить треугольник и узнать его материал
		CDB::TRI* T			= Level().ObjectSpace.GetStaticTris()+result.element;
		hit_material_idx	= T->material;
		pThisWeapon->StaticObjectHit(result, hit_material_idx);
	}

	//проверить достаточно ли силы хита, чтобы двигатьс€ дальше
	if(pThisWeapon->m_fCurrentHitPower>HIT_POWER_EPSILON)
		return TRUE;
	else
		return FALSE;
}

void CShootingObject::DynamicObjectHit (Collide::rq_result& R, u16 target_material)
{
	//только дл€ динамических объектов
	R_ASSERT(R.O);

	SGameMtl* mtl = GMLib.GetMaterialByIdx(target_material);
	float shoot_factor = mtl->fShootFactor;
	float pierce = (m_pCurrentCartridge?m_pCurrentCartridge->m_kPierce:1.f);

	//получить силу хита выстрела с учетом патрона
	float power = m_fCurrentHitPower * pierce *
		(m_pCurrentCartridge?m_pCurrentCartridge->m_kHit:1.f);

	//коэффициент уменьшение силы с рассто€нием
	float scale = 1-(R.range/(m_fCurrentFireDist*
		(m_pCurrentCartridge?m_pCurrentCartridge->m_kDist:1.f)));
	clamp(scale,0.f,1.f);
	//!!! почему мы берем квадратный корень
	scale = _sqrt(scale);
	power *= scale;

	//сила хита физического импульса
	//вычисл€етс€ с учетом пробиваемости материалов
	float material_pierce = 1.f - shoot_factor * pierce;
	clamp(material_pierce, 0.f, 1.f);
	float impulse = m_fCurrentHitImpulse*
		material_pierce*
		(m_pCurrentCartridge?m_pCurrentCartridge->m_kImpulse:1.f)
		*scale;

	VERIFY(impulse>=0);

	CEntity* E = dynamic_cast<CEntity*>(R.O);
	//учитываем попадание в разные части 
	if(E) power *= E->HitScale(R.element);

	// object-space
	//вычислить координаты попадани€
	Fvector p_in_object_space,position_in_bone_space;
	Fmatrix m_inv;
	m_inv.invert(R.O->XFORM());
	m_inv.transform_tiny(p_in_object_space, m_vEndPoint);

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
		u_EventGen		(P,GE_HIT,R.O->ID());
		P.w_u16			(u16(m_iCurrentParentID));
		P.w_u16			(ID());
		P.w_dir			(m_vCurrentShootDir);
		P.w_float		(power);
		P.w_s16			((s16)R.element);
		P.w_vec3		(position_in_bone_space);
		P.w_float		(impulse);
		P.w_u16			(u16(m_fCurrentHitType));
		u_EventSend		(P);
	}

	//визуальное обозначение попадание на объекте
	FireShotmark(m_vCurrentShootDir, m_vEndPoint, R, target_material);



	//уменьшить хит и импульс перед тем как передать его дальше 
	m_fCurrentHitPower *= scale;
	m_fCurrentHitImpulse *= scale;
		
	m_fCurrentHitPower *= (shoot_factor*pierce);
	m_fCurrentHitImpulse *= material_pierce;
}

void CShootingObject::StaticObjectHit(Collide::rq_result& R, u16 target_material)
{
	FireShotmark(m_vCurrentShootDir, m_vEndPoint, R, target_material);

	SGameMtl* mtl = GMLib.GetMaterialByIdx(target_material);

	float shoot_factor = mtl->fShootFactor;
	float pierce = (m_pCurrentCartridge?m_pCurrentCartridge->m_kPierce:1.f);
	float material_pierce = 1.f - shoot_factor * pierce;
	clamp(material_pierce, 0.f, 1.f);
	
	m_fCurrentHitPower*= mtl->fShootFactor;
	m_fCurrentHitPower *= (shoot_factor*pierce);
	m_fCurrentHitImpulse *= material_pierce;
}