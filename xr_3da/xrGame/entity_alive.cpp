#include "stdafx.h"
#include "entity_alive.h"
#include "inventoryowner.h"
#include "inventory.h"
#include "physicsshell.h"
#include "gamemtllib.h"
#include "phmovementcontrol.h"

#define SMALL_ENTITY_RADIUS		0.6f


SHADER_VECTOR CEntityAlive::m_BloodMarksVector;
float CEntityAlive::m_fBloodMarkSizeMin = 0.f;
float CEntityAlive::m_fBloodMarkSizeMax = 0.f;
float CEntityAlive::m_fBloodMarkDistance = 0.f;
float CEntityAlive::m_fBloodMarkDispersion = 0.f;
float CEntityAlive::m_fNominalHit = 0.f;

/////////////////////////////////////////////
// CEntityAlive
/////////////////////////////////////////////
CEntityAlive::CEntityAlive()
{
	Init				();
}

CEntityAlive::~CEntityAlive()
{
	xr_delete				(m_PhysicMovementControl);
}

void CEntityAlive::Init			()
{
	m_PhysicMovementControl = xr_new<CPHMovementControl>();
}

void CEntityAlive::Load		(LPCSTR section)
{
	CEntity::Load			(section);
	CEntityCondition::Load	(section);
	m_fFood					= 100*pSettings->r_float	(section,"ph_mass");


	//bloody wallmarks
	if(m_BloodMarksVector.empty()) 
		LoadBloodyWallmarks ("bloody_marks");

	//blood_wallmark.create("effects\\wallmark", "wm\\wm_blood_1");
/*	string128	tmp;
	int cnt		=_GetItemCount(buf);	R_ASSERT(cnt<=GAMEMTL_SUBITEM_COUNT);
	ref_shader	s;
	for (int k=0; k<cnt; ++k)
	{
		s.create				("effects\\wallmark",_GetItem(buf,k,tmp));
		BloodMarks.push_back	(s);
	}*/

}

void CEntityAlive::LoadBloodyWallmarks (LPCSTR section)
{
	string128	tmp;
	LPCSTR wallmarks_name = pSettings->r_string(section, "wallmarks"); 
	
	int cnt		=_GetItemCount(wallmarks_name);
	
	ref_shader	s;
	for (int k=0; k<cnt; ++k)
	{
		s.create ("effects\\wallmark",_GetItem(wallmarks_name,k,tmp));
		m_BloodMarksVector.push_back	(s);
	}

	m_fBloodMarkSizeMin = pSettings->r_float(section, "min_size"); 
	m_fBloodMarkSizeMax = pSettings->r_float(section, "max_size"); 
	m_fBloodMarkDistance = pSettings->r_float(section, "dist"); 
	m_fBloodMarkDispersion = pSettings->r_float(section, "dispersion"); 
	m_fNominalHit = pSettings->r_float(section, "nominal_hit"); 
}

void CEntityAlive::reinit			()
{
	CEntity::reinit			();
	CEntityCondition::reinit();

	m_fAccuracy				= 25.f;
	m_fIntelligence			= 25.f;
}

void CEntityAlive::reload		(LPCSTR section)
{
	CEntity::reload			(section);
//	CEntityCondition::reload(section);

	m_fFood					= 100*pSettings->r_float	(section,"ph_mass");
}

void CEntityAlive::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);

	//condition update with the game time pass
	UpdateCondition();

	//убить сущность
	if(Local() && !g_Alive() && !AlreadyDie())
	{
		if(GetWhoHitLastTime()) {
//			Msg			("%6d : KillEntity from CEntityAlive (using who hit last time) for object %s",Level().timeServer(),*cName());
			KillEntity(GetWhoHitLastTime());
		}
		else {
//			Msg			("%6d : KillEntity from CEntityAlive for object %s",Level().timeServer(),*cName());
			KillEntity(this);
		}
	}
}

BOOL CEntityAlive::net_Spawn	(LPVOID DC)
{
	inherited::net_Spawn	(DC);

	//m_PhysicMovementControl->SetPosition	(Position());
	//m_PhysicMovementControl->SetVelocity	(0,0,0);
	return					TRUE;
}

void CEntityAlive::net_Destroy	()
{
	CInventoryOwner	*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(this);
	if (l_tpInventoryOwner) {
		l_tpInventoryOwner->inventory().ClearAll();
		l_tpInventoryOwner->m_trade_storage->ClearAll();
	}

	inherited::net_Destroy		();
}

void CEntityAlive::HitImpulse	(float /**amount/**/, Fvector& /**vWorldDir/**/, Fvector& /**vLocalDir/**/)
{
	//	float Q					= 2*float(amount)/m_PhysicMovementControl->GetMass();
	//	m_PhysicMovementControl->vExternalImpulse.mad	(vWorldDir,Q);
}

void CEntityAlive::Hit(float P, Fvector &dir,CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type)
{
	//изменить состояние, перед тем как родительский класс обработае хит
	ConditionHit(who, P, hit_type, element);

	//добавить кровь на стены
	BloodyWallmarks (P, dir, element, position_in_object_space);

	inherited::Hit(P,dir,who,element,position_in_object_space,impulse, hit_type);
}

void CEntityAlive::BuyItem(LPCSTR buf)
{
	NET_Packet P;
	u_EventGen	(P,GE_BUY,ID());
	P.w_string	(buf);
	u_EventSend	(P);
}

//вывзывает при подсчете хита
float CEntityAlive::CalcCondition(float /**hit/**/)
{	
	UpdateCondition();

	//dont call inherited::CalcCondition it will be meaningless
	return GetHealthLost()*100.f;
}

///////////////////////////////////////////////////////////////////////
u16	CEntityAlive::PHGetSyncItemsNumber()
{
	if(m_PhysicMovementControl->CharacterExist()) return 1;
	else										  return 0;
}
CPHSynchronize* CEntityAlive::PHGetSyncItem	(u16/*item*/)
{
	if(m_PhysicMovementControl->CharacterExist()) return m_PhysicMovementControl->GetSyncItem();
	else										 return 0;
}
void CEntityAlive::PHUnFreeze()
{
	if(m_PhysicMovementControl->CharacterExist()) m_PhysicMovementControl->UnFreeze();
}
void CEntityAlive::PHFreeze()
{
	if(m_PhysicMovementControl->CharacterExist()) m_PhysicMovementControl->Freeze();
}
//////////////////////////////////////////////////////////////////////

//добавление кровавых отметок на стенах, после получения хита
void CEntityAlive::BloodyWallmarks (float P, Fvector &dir, s16 element, 
									Fvector position_in_object_space)
{
	//вычислить координаты попадания
	CKinematics* V = PKinematics(Visual());
		
	Fvector start_pos = position_in_object_space;
	if(V)
	{
		Fmatrix& m_bone = (V->LL_GetBoneInstance(u16(element))).mTransform;
		m_bone.transform_tiny(start_pos);
	}
	XFORM().transform_tiny(start_pos);

	float small_entity = 1.f;
	if(Radius()<SMALL_ENTITY_RADIUS) small_entity = 0.5;
	
	setEnabled(false);


	//рисуем кровь по алгоритму:
	//по направлению полета пули максимальный по размеру валмарк
	//потом несколько маленьких с большим разбросом
	int blood_marks_num = iFloor(0.5f + P/m_fNominalHit);
	clamp(blood_marks_num, 1, 6);
	//for(int i=0; i<blood_marks_num; i++)
	{
		float disp;
		float main_mark;
	//	if(i==0) 
		{
			//для основной отметки ставим совсем небольшую дисперсию
			disp = 0.0f;
			main_mark = 1.f;
		}
	/*	else
		{
			disp = m_fBloodMarkDispersion;
			main_mark = 0.3f;
		}*/

		Fvector rnd_dir;
		rnd_dir.random_dir(dir, disp, Random);
		dir = rnd_dir;

		Collide::rq_result result;
		BOOL reach_wall = Level().ObjectSpace.RayPick(start_pos, dir, m_fBloodMarkDistance, 
			Collide::rqtBoth, result);

		//если кровь долетела до статического объекта
		if(reach_wall && !result.O)
		{
			//вычислить нормаль к пораженной поверхности
			CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+result.element;
			Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();

			//вычислить точку попадания
			Fvector end_point;
			end_point.set(0,0,0);
			end_point.mad(start_pos, dir, result.range);


			ref_shader* pWallmarkShader = m_BloodMarksVector.empty()?NULL:
			&m_BloodMarksVector[::Random.randI(0,m_BloodMarksVector.size())];

			if (pWallmarkShader)
			{
				float wallmark_size = m_fBloodMarkSizeMax;
				wallmark_size *= (P/m_fNominalHit);
				wallmark_size *= small_entity;
				wallmark_size *= main_mark;
				clamp(wallmark_size, m_fBloodMarkSizeMin, m_fBloodMarkSizeMax);
			
				//добавить отметку на материале
				::Render->add_Wallmark(*pWallmarkShader, end_point,
					wallmark_size, pTri, pVerts);
			}
		}

	}


	setEnabled(true);
}