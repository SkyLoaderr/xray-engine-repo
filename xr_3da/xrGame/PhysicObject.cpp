#include "stdafx.h"
#include "physicobject.h"
#include "PhysicsShell.h"
#include "Physics.h"
#include "xrserver_objects_alife.h"

CPhysicObject::CPhysicObject(void) {
	m_type = epotBox;
	m_mass = 10.f;
	b_recalculate=false;
}

CPhysicObject::~CPhysicObject(void)
{
	SHELL_PAIR_I i=m_unsplited_shels.begin(),e=m_unsplited_shels.end();
	for(;i!=e;++i)
	{
		i->first->Deactivate();
		xr_delete(i->first);
	}
}

BOOL CPhysicObject::net_Spawn(LPVOID DC)
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeObjectPhysic	*po	= dynamic_cast<CSE_ALifeObjectPhysic*>(e);
	R_ASSERT				(po);
	inherited::net_Spawn	(DC);

	m_type = EPOType(po->type);
	m_mass = po->mass;
	LPCSTR start_anim=*po->startup_animation;
	xr_delete(collidable.model);
	switch(m_type) {
		case epotBox:			collidable.model = xr_new<CCF_Rigid>(this);		break;
		case epotFixedChain:	
		case epotSkeleton:		collidable.model = xr_new<CCF_Skeleton>(this);	break;
		default: NODEFAULT; 
	}

	CreateBody				(po);

	CSkeletonAnimated* pSkeletonAnimated=NULL;
	switch(m_type) {
		case epotBox:				break;
		case epotFixedChain:
		case epotSkeleton:
			R_ASSERT(Visual()&&PKinematics(Visual()));
			pSkeletonAnimated=PSkeletonAnimated(Visual());
			if(pSkeletonAnimated)
			{
				pSkeletonAnimated->PlayCycle(start_anim);
				pSkeletonAnimated->Calculate();
			}
			break;
		default: NODEFAULT; 
	}


	setVisible(true);
	setEnabled(true);

	return TRUE;
}

void CPhysicObject::UpdateCL	()
{
	inherited::UpdateCL		();
	if(m_pPhysicsShell)
	{
		//PKinematics(Visual())->Calculate();
		if(m_type==epotBox) 
		{
			m_pPhysicsShell->Update();
			XFORM().set			(m_pPhysicsShell->mXFORM);
		}
		else
			m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
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


void CPhysicObject::CreateBody(CSE_ALifeObjectPhysic* po) {
	if(po->flags.test(CSE_ALifeObjectPhysic::flSpawnCopy))return;
	if(m_pPhysicsShell) return;
	m_pPhysicsShell		= P_create_Shell();
	switch(m_type) {
		case epotBox : {
			Fobb obb; Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); obb.m_rotate.identity();
			CPhysicsElement* E = P_create_Element(); R_ASSERT(E); E->add_Box(obb);
			m_pPhysicsShell->add_Element(E);
			m_pPhysicsShell->setMass(m_mass);
			if(!H_Parent())
				m_pPhysicsShell->Activate(XFORM(),0,XFORM(),!po->flags.test(CSE_ALifeObjectPhysic::flActive));
			m_pPhysicsShell->mDesired.identity();
			m_pPhysicsShell->fDesiredStrength = 0.f;
		} break;
		case epotFixedChain : 
		case epotFreeChain  :
			{
			m_pPhysicsShell->set_Kinematics(PKinematics(Visual()));
			AddElement(0,PKinematics(Visual())->LL_GetBoneRoot());
			m_pPhysicsShell->setMass1(m_mass);
		} break;
		case   epotSkeleton: CreateSkeleton(po); break;

		default : {
		} break;
	
	}
	CKinematics* pKinematics=PKinematics(Visual());
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
										Fvector p_in_object_space, float impulse, ALife::EHitType /**hit_type/**/)
{
	if(m_pPhysicsShell){
		switch(m_type) {
		case epotBox :
			inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
		break;
		case epotFixedChain :
		case epotFreeChain  :
		case epotSkeleton   :
			m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse,element);
		break;
		default : NODEFAULT;
		}
	}
}

static BONE_P_MAP bone_map=BONE_P_MAP();
void CPhysicObject::CreateSkeleton(CSE_ALifeObjectPhysic* po)
{
	if (!Visual()) return;

	CKinematics* pKinematics=PKinematics(Visual());
	m_pPhysicsShell		= P_create_Shell();

	LPCSTR	fixed_bones=*po->fixed_bones;
	if(fixed_bones)
	{
		bone_map					.clear();

		int count =					_GetItemCount(fixed_bones);
		for (int i=0 ;i<count; ++i) 
		{
			string64					fixed_bone							;
			_GetItem					(fixed_bones,i,fixed_bone)			;
			u16 fixed_bone_id=pKinematics->LL_BoneID(fixed_bone)			;
			R_ASSERT2(BI_NONE!=fixed_bone_id,"wrong fixed bone")			;
			bone_map.insert(mk_pair(fixed_bone_id,physicsBone()))			;
		}
		
		

		m_pPhysicsShell->build_FromKinematics(pKinematics,&bone_map);
		


		//m_pPhysicsShell->add_Joint(P_create_Joint(CPhysicsJoint::enumType::full_control,0,fixed_element));
	}
	else
		m_pPhysicsShell->build_FromKinematics(pKinematics);

	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->Activate(true,!po->flags.test(CSE_ALifeObjectPhysic::flActive));//,
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	m_pPhysicsShell->SetAirResistance();//0.0014f,1.5f

	BONE_P_PAIR_IT i=bone_map.begin(),e=bone_map.end();
	for(;i!=e;i++)
	{
		CPhysicsElement* fixed_element=i->second.element;
		R_ASSERT2(fixed_element,"fixed bone has no physics");
		FixBody(fixed_element->get_body());
	}
}

void CPhysicObject::net_Export(NET_Packet& P)
{
	R_ASSERT						(Local());
	//m_pPhysicsShell->net_Export(P);
}

void CPhysicObject::net_Import(NET_Packet& P)
{
	//m_pPhysicsShell->net_Import(P);
	R_ASSERT						(Remote());
}


void CPhysicObject::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	if(m_pPhysicsShell && m_pPhysicsShell->isFractured()) 
	{
		PHSplit();
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
		
		l_tpALifeDynamicObject->m_tNodeID	= level_vertex_id();
		l_tpALifePhysicObject->set_visual(*cNameVisual());
		l_tpALifePhysicObject->type=u32(m_type);
		//char mask=0;
		//mask&= (1>>1);
		l_tpALifePhysicObject->flags.set(CSE_ALifeObjectPhysic::flSpawnCopy,1);
		//l_tpALifePhysicObject->flags.set(mask);
		// Fill

		strcpy				(D->s_name,*cNameSect());
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	u16(ID());
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
	u16 id;
	switch (type)
	{
	case GE_OWNERSHIP_TAKE:
		{
			P.r_u16		(id);
			UnsplitSingle( dynamic_cast<CGameObject*>(Level().Objects.net_Find	(id)));
		}
	}
}
void __stdcall PushOutCallback2(bool& do_colide,dContact& c);

void CPhysicObject::UnsplitSingle(CGameObject* O)
{
	//Msg("%o,received has %d,",this,m_unsplited_shels.size());
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
	newPhysicsShell->set_PushOut(5000,PushOutCallback2);
	m_unsplited_shels.erase(m_unsplited_shels.begin());
	newKinematics->Calculate();
	NET_Packet P;
	O->u_EventGen(P, GE_OWNERSHIP_REJECT,ID());
	P.w_u16(u16(O->ID()));
	O->u_EventSend(P);
}

BOOL CPhysicObject::UsedAI_Locations()
{
	return					(FALSE);
}

void CPhysicObject::OnH_A_Independent()
{
	PKinematics(Visual())->Calculate();
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
