#include "stdafx.h"
#include "physicobject.h"
#include "PhysicsShell.h"
#include "Physics.h"
#include "xrserver_objects_alife.h"
#include "level.h"

u32 CPhysicObject::remove_time=5000;

CPhysicObject::CPhysicObject(void) 
{
	m_type = epotBox;
	m_mass = 10.f;
	Init();
}

void CPhysicObject::RespawnInit()
{
	CKinematics*	K	=	PKinematics(Visual());
	K->LL_SetBoneRoot(0);
	K->LL_SetBonesVisible(0xffffffffffffffffL);
	Init();
	ClearUnsplited();
}
void CPhysicObject::Init()
{
	b_recalculate=false;
	m_unsplit_time = u32(-1);
	b_removing=false;
	m_startup_anim=NULL;
	//m_source=NULL;
}

void CPhysicObject::ClearUnsplited()
{
	SHELL_PAIR_I i=m_unsplited_shels.begin(),e=m_unsplited_shels.end();
	for(;i!=e;++i)
	{
		i->first->Deactivate();
		xr_delete(i->first);
	}
}
CPhysicObject::~CPhysicObject(void)
{
	ClearUnsplited();
}

BOOL CPhysicObject::net_Spawn(LPVOID DC)
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeObjectPhysic	*po	= dynamic_cast<CSE_ALifeObjectPhysic*>(e);
	R_ASSERT				(po);
	inherited::net_Spawn	(DC);

	m_flags					= po->flags;
//	m_flags.set				(flSpawnCopy,FALSE);
	m_type = EPOType(po->type);
	m_mass = po->mass;
	m_startup_anim=po->startup_animation;
	xr_delete(collidable.model);
	switch(m_type) {
		case epotBox:			collidable.model = xr_new<CCF_Rigid>(this);		break;
		case epotFixedChain:	
		case epotSkeleton:		collidable.model = xr_new<CCF_Skeleton>(this);	break;
		default: NODEFAULT; 
	}

	if(!po->flags.test(CSE_ALifeObjectPhysic::flSpawnCopy)) 
													CreateBody				(po);
	else
	{
		CPhysicObject* source=dynamic_cast<CPhysicObject*>(Level().Objects.net_Find(po->source_id));
		//m_source=dynamic_cast<CPhysicObject*>(Level().Objects.net_Find(po->source_id));
		R_ASSERT2(source,"no source");
		//setVisible(false);
		//setEnabled(false);
		source->UnsplitSingle(this);
	}

	CSkeletonAnimated* pSkeletonAnimated=NULL;
	switch(m_type) {
		case epotBox:				break;
		case epotFixedChain:
		case epotSkeleton:
			R_ASSERT(Visual()&&PKinematics(Visual()));
			pSkeletonAnimated=PSkeletonAnimated(Visual());
			if(pSkeletonAnimated)
			{
				R_ASSERT2(*m_startup_anim,"no startup animation");
				pSkeletonAnimated->PlayCycle(*m_startup_anim);
				pSkeletonAnimated->Calculate();
			}
			break;
		default: NODEFAULT; 
	}

if(!po->flags.test(CSE_ALifeObjectPhysic::flSpawnCopy))
{
	setVisible(true);
	setEnabled(true);
}

	return TRUE;
}

void CPhysicObject::net_Destroy()
{
	RespawnInit();
	inherited::net_Destroy();
	PKinematics(Visual())->Calculate();
}
void CPhysicObject::Load(LPCSTR section)
{
	inherited::Load(section);
	remove_time= pSettings->r_u32(section,"remove_time")*1000;
}
void CPhysicObject::UpdateCL	()
{
	inherited::UpdateCL		();
	if(m_pPhysicsShell)
	{
	

		if(m_type==epotBox) 
		{
			m_pPhysicsShell->Update();
			XFORM().set			(m_pPhysicsShell->mXFORM);
		}
		else
			m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
	}
	else
	{
		//if(m_source)
		//{
		//	m_source->UnsplitSingle(this);
		//	m_source=NULL;
		//}
	}

}

void CPhysicObject::AddElement(CPhysicsElement* root_e, int id)
{
	CKinematics* K		= PKinematics(Visual());

	CPhysicsElement* E	= P_create_Element();
	CBoneInstance& B	= K->LL_GetBoneInstance(u16(id));
	E->mXFORM.set		(K->LL_GetTransform(u16(id)));
	Fobb bb			= K->LL_GetBox(u16(id));


	if(bb.m_halfsize.magnitude()<0.05f)
	{
		bb.m_halfsize.add(0.05f);

	}
	E->add_Box			(bb);
	E->setMass			(10.f);
	E->set_ParentElement(root_e);
	B.set_callback		(m_pPhysicsShell->GetBonesCallback(),E);
	m_pPhysicsShell->add_Element	(E);
	if( !(m_type==epotFreeChain && root_e==0) )
	{		
		CPhysicsJoint* J= P_create_Joint(CPhysicsJoint::full_control,root_e,E);
		J->SetAnchorVsSecondElement	(0,0,0);
		J->SetAxisDirVsSecondElement	(1,0,0,0);
		J->SetAxisDirVsSecondElement	(0,1,0,2);
		J->SetLimits				(-M_PI/2,M_PI/2,0);
		J->SetLimits				(-M_PI/2,M_PI/2,1);
		J->SetLimits				(-M_PI/2,M_PI/2,2);
		m_pPhysicsShell->add_Joint	(J);	
	}

	CBoneData& BD		= K->LL_GetData(u16(id));
	for (vecBonesIt it=BD.children.begin(); BD.children.end() != it; ++it){
		AddElement		(E,(*it)->SelfID);
	}
}

static bool removable;//for RecursiveBonesCheck
void CPhysicObject::RecursiveBonesCheck(u16 id)
{
	if(!removable) return;
	CKinematics* K		= PKinematics(Visual());
	CBoneData& BD		= K->LL_GetData(u16(id));
	//////////////////////////////////////////
	Flags64 mask;
	mask.set(K->LL_GetBonesVisible());
	///////////////////////////////////////////
	if(
		mask.is(1ui64<<(u64)id)&& 
		!(BD.shape.flags.is(SBoneShape::sfRemoveAfterBreak))
	) {
		removable = false;
		return;
	}
	///////////////////////////////////////////////
	for (vecBonesIt it=BD.children.begin(); BD.children.end() != it; ++it){
		RecursiveBonesCheck		((*it)->SelfID);
	}
}
void CPhysicObject::CreateBody(CSE_ALifeObjectPhysic* po) {
	
	if(m_pPhysicsShell) return;
	CKinematics* pKinematics=PKinematics(Visual());
	switch(m_type) {
		case epotBox : {
			m_pPhysicsShell=P_build_SimpleShell(this,m_mass,!po->flags.test(CSE_ALifeObjectPhysic::flActive));
		} break;
		case epotFixedChain : 
		case epotFreeChain  :
			{	
			m_pPhysicsShell		= P_create_Shell();
			m_pPhysicsShell->set_Kinematics(pKinematics);
			AddElement(0,pKinematics->LL_GetBoneRoot());
			m_pPhysicsShell->setMass1(m_mass);
		} break;

		case   epotSkeleton: 
			{
			//pKinematics->LL_SetBoneRoot(0);
			CreateSkeleton(po);
		}break;

		default : {
		} break;
	
	}

	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
	if(pKinematics)
	{

		SAllDDOParams disable_params;
		disable_params.Load(pKinematics->LL_UserData());
		m_pPhysicsShell->set_DisableParams(disable_params);
	}
	//m_pPhysicsShell->SetAirResistance(0.002f, 0.3f);
}


void CPhysicObject::Hit(float P,Fvector &dir, CObject* who,s16 element,
										Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	if(m_pPhysicsShell){
		switch(m_type) {
		case epotBox :
			inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
		break;
		case epotFixedChain :
		case epotFreeChain  :
		case epotSkeleton   :
			m_pPhysicsShell->applyHit(p_in_object_space,dir,impulse,element,hit_type);
		break;
		default : NODEFAULT;
		}
	}
}


void CPhysicObject::CreateSkeleton(CSE_ALifeObjectPhysic* po)
{
	if (!Visual()) return;
	LPCSTR	fixed_bones=*po->fixed_bones;
	m_pPhysicsShell=P_build_Shell(this,!po->flags.test(CSE_ALifeObjectPhysic::flActive),fixed_bones);
}

void CPhysicObject::net_Export(NET_Packet& P)
{
	inherited::net_Export			(P);
	R_ASSERT						(Local());
//	P.w_u8							(m_flags.get());
	//m_pPhysicsShell->net_Export(P);
}

void CPhysicObject::net_Import(NET_Packet& P)
{
	inherited::net_Import(P);
	//m_pPhysicsShell->net_Import(P);
	R_ASSERT						(Remote());
//	m_flags.set						(P.r_u8());
}


void CPhysicObject::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	if ( m_pPhysicsShell && m_pPhysicsShell->isFractured()) //!ai().get_alife() &&
	{
		PHSplit();
	}



	
	if(b_removing&&(Device.dwTimeGlobal-m_unsplit_time)*phTimefactor>remove_time) 
	{
		NET_Packet			P;
		u_EventGen			(P,GE_DESTROY,ID());
		Msg					("ge_destroy: [%d] - %s",ID(),*cName());
		if (Local()) u_EventSend			(P);
		b_removing=false;
	}


}

void CPhysicObject::SpawnCopy()
{
	if(Local()) {
		CSE_Abstract*				D	= F_entity_Create(*cNameSect());
		R_ASSERT					(D);
		CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
		R_ASSERT					(l_tpALifeDynamicObject);
		CSE_ALifeObjectPhysic		*l_tpALifePhysicObject = dynamic_cast<CSE_ALifeObjectPhysic*>(D);
		R_ASSERT					(l_tpALifePhysicObject);
		
		l_tpALifePhysicObject->m_tGraphID	= game_vertex_id();
		l_tpALifeDynamicObject->m_tNodeID	= level_vertex_id();
		l_tpALifePhysicObject->set_visual	(*cNameVisual());
		l_tpALifePhysicObject->type			= u32(m_type);
		//char mask=0;
		//mask&= (1>>1);
		l_tpALifePhysicObject->flags.set	(CSE_ALifeObjectPhysic::flSpawnCopy,1);
		l_tpALifePhysicObject->source_id	= u16(ID());
		l_tpALifePhysicObject->startup_animation=m_startup_anim;
		//l_tpALifePhysicObject->flags.set(mask);
		// Fill

		strcpy				(D->s_name,*cNameSect());
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	0xffff;//u16(ID());//
		D->ID_Phantom		=	0xffff;
		D->o_Position		=	Position();
		XFORM()				.getHPB(D->o_Angle);
		D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
}
PHSHELL_PAIR_VECTOR new_shells;
void CPhysicObject::PHSplit()
{
	//if(m_unsplited_shels.empty())
	{
		u16 spawned=u16(m_unsplited_shels.size());
		m_pPhysicsShell->SplitProcess(m_unsplited_shels);
		u16 i=u16(m_unsplited_shels.size())-spawned;
		//	Msg("%o,spawned,%d",this,i);
		for(;i;--i) SpawnCopy();
	}
	//else
	//{
	//	new_shells.clear();
	//	m_pPhysicsShell->SplitProcess(new_shells);
	//	m_unsplited_shels.insert(m_unsplited_shels.begin(),new_shells.begin(),new_shells.end());
	//	u16 i=u16(new_shells.size());
	//	//	Msg("%o,spawned,%d",this,i);
	//	for(;i;--i) SpawnCopy();
	//}
}

void CPhysicObject::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);
	//u16 id;
	//switch (type)
	//{
	//case GE_OWNERSHIP_TAKE:
	//	{
	//		P.r_u16		(id);
	//		CGameObject* O =dynamic_cast<CGameObject*>(Level().Objects.net_Find	(id));
	//		//O->H_SetParent(this);
	//		UnsplitSingle( dynamic_cast<CPhysicObject*>(O) );
	//		break;
	//	}
	//case GE_OWNERSHIP_REJECT:
	//	{
	//		P.r_u16		(id);
	//		CGameObject* O =dynamic_cast<CGameObject*>(Level().Objects.net_Find	(id));
	//		//O->H_SetParent(NULL);
	//		break;
	//	}
	//}
}
void __stdcall PushOutCallback2(bool& do_colide,dContact& c);

void CPhysicObject::UnsplitSingle(CPhysicObject* O)
{
	//Msg("%o,received has %d,",this,m_unsplited_shels.size());
	if (0==m_unsplited_shels.size())	return;	//. hack
	VERIFY2(m_unsplited_shels.size(),"NO_SHELLS !!");
	VERIFY2(!O->m_pPhysicsShell,"this has shell already!!!");
	CPhysicsShell* newPhysicsShell=m_unsplited_shels.front().first;
	O->m_pPhysicsShell=newPhysicsShell;
	VERIFY(_valid(newPhysicsShell->mXFORM));
	CKinematics *newKinematics=PKinematics(O->Visual());
	CKinematics *pKinematics  =PKinematics(Visual());

	Flags64 mask0,mask1;
	u16 split_bone=m_unsplited_shels.front().second;
	mask1.set(pKinematics->LL_GetBonesVisible());//source bones mask
	pKinematics->LL_SetBoneVisible(split_bone,FALSE,TRUE);
	mask0.set(pKinematics->LL_GetBonesVisible());//first part mask
	VERIFY2(mask0.flags,"mask0 -Zero");
	mask0.invert();
	mask1.and(mask0.flags);//second part mask
	newPhysicsShell->set_Kinematics(newKinematics);
	VERIFY(_valid(newPhysicsShell->mXFORM));
	newPhysicsShell->ResetCallbacks(split_bone,mask1);
	VERIFY(_valid(newPhysicsShell->mXFORM));

	newPhysicsShell->ObjectInRoot().identity();

	newKinematics->LL_SetBoneRoot		(split_bone);
	VERIFY2(mask1.flags,"mask1 -Zero");
	newKinematics->LL_SetBonesVisible	(mask1.flags);
		
	newPhysicsShell->set_PhysicsRefObject(O);
	//newPhysicsShell->set_PushOut(5000,PushOutCallback2);
	m_unsplited_shels.erase(m_unsplited_shels.begin());
	O->setVisible(true);
	O->setEnabled(true);
	newKinematics->Calculate();

	//NET_Packet				P;
	//u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
	//P.w_u16					(u16(O->ID()));
	//u_EventSend				(P);

	O->CopySpawnInit		();
	CopySpawnInit			();

	
}

BOOL CPhysicObject::UsedAI_Locations()
{
	return					(FALSE);
}

void CPhysicObject::OnH_A_Independent()
{
	inherited::OnH_A_Independent();
	//PKinematics(Visual())->Calculate();
	//CopySpawnInit();
}

void CPhysicObject::CopySpawnInit()
{
	if(ReadyForRemove()) SetAutoRemove();
}

bool CPhysicObject::ReadyForRemove()
{
	removable=true;
	RecursiveBonesCheck(PKinematics(Visual())->LL_GetBoneRoot());
	return removable;
}
void CPhysicObject::SetAutoRemove()
{
	b_removing=true;
	m_unsplit_time=Device.dwTimeGlobal;

}
//////////////////////////////////////////////////////////////////////////
/*
DEFINE_MAP_PRED	(LPCSTR,	CPhysicsJoint*,	JOINT_P_MAP,	JOINT_P_PAIR_IT,	pred_str);

JOINT_P_MAP			*l_tpJointMap = xr_new<JOINT_P_MAP>();

l_tpJointMap->insert(mk_pair(bone_name,joint*));
JOINT_P_PAIR_IT		I = l_tpJointMap->find(bone_name);
if (l_tpJointMap->end()!=I){
	//bone_name is found and is an pair_iterator
	(*I).second
}

JOINT_P_PAIR_IT		I = l_tpJointMap->begin();
JOINT_P_PAIR_IT		E = l_tpJointMap->end();
for ( ; I != E; ++I) {
	(*I).second->joint_method();
	Msg("%s",(*I).first);
}

*/

//////////////////////////////////////////////////////////////////////////
