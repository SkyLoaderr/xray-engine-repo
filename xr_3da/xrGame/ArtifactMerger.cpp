///////////////////////////////////////////////////////////////
// ArtifactMerger.cpp
// ArtifactMerger - ���������� ��� ��������� ����� ���������� 
// �� ���������
///////////////////////////////////////////////////////////////

#include "stdafx.h"


#include "ArtifactMerger.h"
#include "PhysicsShell.h"
#include "ai_script_processor.h"

#include "level.h"


//���������� ��������� �� �������, �������
//��������� ������� ��������� ���������� �� 
//�������
static luabind::functor<void> g_ArtifactMergeFunctor;



CArtifactMerger::CArtifactMerger(void) 
{
	m_ArtifactList.clear();
}

CArtifactMerger::~CArtifactMerger(void) 
{
	m_ArtifactList.clear();
}


void CArtifactMerger::SetArtifactMergeFunctor(const luabind::functor<void>& artifactMergeFunctor)
{
	g_ArtifactMergeFunctor = artifactMergeFunctor;
}


BOOL CArtifactMerger::net_Spawn(LPVOID DC) 
{
	BOOL res = inherited::net_Spawn(DC);

//	CSkeletonAnimated* V = PSkeletonAnimated(Visual());
//	if(V) V->PlayCycle("idle");
	CSkeletonRigid* V = PSkeletonRigid(Visual());
	R_ASSERT(V);


	if (m_pPhysicsShell==0) 
	{
		// Physics (Box)
		Fobb obb; 
		Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); 
		obb.m_rotate.identity();
		
		// Physics (Elements)
		CPhysicsElement* E = P_create_Element(); 
		R_ASSERT(E); 
		E->add_Box(obb);
		// Physics (Shell)
		m_pPhysicsShell = P_create_Shell(); 
		R_ASSERT(m_pPhysicsShell);
		m_pPhysicsShell->add_Element(E);
		m_pPhysicsShell->setDensity(2000.f);
		
		CSE_Abstract *l_pE = (CSE_Abstract*)DC;
		if(l_pE->ID_Parent==0xffff) m_pPhysicsShell->Activate(XFORM(),0,XFORM());
		m_pPhysicsShell->mDesired.identity();
		m_pPhysicsShell->fDesiredStrength = 0.f;
	}

	setVisible(true);
	setEnabled(true);

	return res;
}

void CArtifactMerger::Load(LPCSTR section) 
{
	//��������� ��� ��������� ���������� ���������� ��� ���������
	//�����

	inherited::Load(section);
}

void CArtifactMerger::net_Destroy() 
{
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	
	inherited::net_Destroy();
}

void CArtifactMerger::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CArtifactMerger::UpdateCL() 
{
	inherited::UpdateCL();

	if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update	();
		XFORM().set				(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}
}


void CArtifactMerger::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();

	setVisible					(false);
	setEnabled					(false);
	if(m_pPhysicsShell)			m_pPhysicsShell->Deactivate();
}

void CArtifactMerger::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();

	setVisible(true);
	setEnabled(true);
	
	CObject* E = dynamic_cast<CObject*>(H_Parent()); 
	R_ASSERT(E);
	
	XFORM().set(E->XFORM());
	Position().set(XFORM().c);

	if(m_pPhysicsShell) 
	{
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		Fvector l_fw; l_fw.set(trans.k);
		Fvector l_up; l_up.set(XFORM().j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(XFORM()); l_p1.c.add(l_up); l_up.mul(1.2f); 
		l_p2.set(XFORM()); l_p2.c.add(l_up); l_fw.mul(3.f); l_p2.c.add(l_fw);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		XFORM().set(l_p1);
		Position().set(XFORM().c);
	}
}

void CArtifactMerger::renderable_Render() 
{
	if(getVisible() && !H_Parent()) 
	{
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}
////////////////////////////////////////////
//������� ��� ������ �� ������� ����������
////////////////////////////////////////////

void CArtifactMerger::AddArtifact(CArtifact* pArtifact)
{
	m_ArtifactList.push_back(pArtifact);
}
void CArtifactMerger::RemoveArtifact(CArtifact* pArtifact)
{
	m_ArtifactList.remove(pArtifact);
}
void CArtifactMerger::RemoveAllArtifacts()
{
	m_ArtifactList.clear();
}

//��������� �������� ������� ��� ����
//����������, ��� ��������� � ������
//(����� ������������� ���������)
bool CArtifactMerger::PerformMerge()
{
	R_ASSERT2(g_ArtifactMergeFunctor.is_valid(), "The function that perform artifact merge doesn't set yet");
	
	m_ArtifactDeletedList.clear();
	m_ArtifactNewList.clear();
	
	g_ArtifactMergeFunctor(this);
	return false;
}

void CArtifactMerger::SpawnArtifact(const char* af_section)
{
	CSE_Abstract*		D	= F_entity_Create(af_section);
	R_ASSERT			(D);
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = 
								 dynamic_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT			(l_tpALifeDynamicObject);
	l_tpALifeDynamicObject->m_tNodeID = this->AI_NodeID;
		
	// Fill
	strcpy				(D->s_name, af_section);
	strcpy				(D->s_name_replace,"");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	u16(this->H_Parent()->ID());
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	Position();
	D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
	// Send
	NET_Packet			P;
	D->Spawn_Write		(P,TRUE);
	Level().Send		(P,net_flags(TRUE));
	// Destroy
	F_entity_Destroy	(D);
}




/*
		// ������� �����, ���� � ������ ������ ���� 5 ������
		CSE_Abstract*		D	= F_entity_Create(cNameSect());
		R_ASSERT			(D);
		CSE_ALifeDynamicObject				*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
		R_ASSERT							(l_tpALifeDynamicObject);
		l_tpALifeDynamicObject->m_tNodeID	= AI_NodeID;
		// Fill
		strcpy				(D->s_name,cNameSect());
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	(u16)ID();
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
*/

/*
	if(Local()) for(u32 i = 0; i < 1; i++) {
		// Create
		//CSE_Abstract*		D	= F_entity_Create("detector_simple");
		//CSE_Abstract*		D	= F_entity_Create("grenade_f1");
		CSE_Abstract*		D	= F_entity_Create("bolt");
		R_ASSERT			(D);
		CSE_ALifeDynamicObject				*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
		R_ASSERT							(l_tpALifeDynamicObject);
		l_tpALifeDynamicObject->m_tNodeID	= E->m_tNodeID;
		
		
		// Fill
		strcpy				(D->s_name,"bolt");
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	E->ID;
		D->ID_Phantom		=	0xffff;
		D->o_Position		=	Position();
		D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);

*/