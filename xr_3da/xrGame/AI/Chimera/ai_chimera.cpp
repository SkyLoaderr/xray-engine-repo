#include "stdafx.h"
#include "ai_chimera.h"
#include "../../CharacterPhysicsSupport.h"
#include "../../phmovementcontrol.h"
#include "../../xrserver_objects_alife_monsters.h"


CAI_Chimera::CAI_Chimera() 
{
	Init							();

	m_pPhysics_support				= xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etBitting,this);
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);

}

CAI_Chimera::~CAI_Chimera()
{
	xr_delete						(m_pPhysics_support);
}


void CAI_Chimera::Init()
{
	inherited::Init();

}

void CAI_Chimera::reinit()
{
	inherited::reinit();
	m_pPhysics_support->in_NetSpawn	();

	cur_anim	= 0;
}

void CAI_Chimera::Load(LPCSTR section)
{
	inherited::Load(section);

	m_pPhysics_support->in_Load		(section);
}

void CAI_Chimera::reload(LPCSTR	section)
{
	inherited::reload(section);
}


BOOL CAI_Chimera::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterAbstract		*tpMonster = dynamic_cast<CSE_ALifeMonsterAbstract*>(e);
	R_ASSERT						(tpMonster);


	return TRUE;
}

void CAI_Chimera::UpdateCL()
{
	inherited::UpdateCL();
	
	m_pPhysics_support->in_UpdateCL();
}

void CAI_Chimera::net_Destroy()
{
	inherited::net_Destroy	();
	m_pPhysics_support->in_NetDestroy();
}

void CAI_Chimera::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	m_pPhysics_support->in_shedule_Update(dt);
}

void CAI_Chimera::Think()
{
	
}

void CAI_Chimera::Die()
{
	inherited::Die();
}


static void __stdcall AnimEndCallback(CBlend* B)
{
	CAI_Chimera *pC = (CAI_Chimera*)B->CallbackParam;
	pC->cur_anim = 0;
}


void CAI_Chimera::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	if (cur_anim == 0) {
		cur_anim = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_idle_0");
		PSkeletonAnimated(Visual())->PlayCycle(cur_anim,TRUE,AnimEndCallback,this);
	}
}

BOOL CAI_Chimera::feel_vision_isRelevant(CObject* O)
{
	if (CLSID_ENTITY!=O->CLS_ID) return FALSE;

	CEntityAlive* E = dynamic_cast<CEntityAlive*> (O);
	if (!E) return FALSE;
	if (E->g_Team() == g_Team() && E->g_Alive()) return FALSE;

	return TRUE;
}


