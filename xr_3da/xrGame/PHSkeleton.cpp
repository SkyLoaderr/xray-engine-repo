#include "stdafx.h"
#include "PHSkeleton.h"
#include "PhysicsShellHolder.h"
#include "xrServer_Objects_ALife.h"
#include "Level.h"
#include "PHDefs.h"
#include "PhysicsShell.h"
#include "PHSynchronize.h"
#define F_MAX         3.402823466e+38F
u32 CPHSkeleton::remove_time=5000;

CPHSkeleton::CPHSkeleton()
{
		Init();
}

CPHSkeleton::~CPHSkeleton()
{
		ClearUnsplited();
}

void CPHSkeleton::RespawnInit()
{
	CKinematics*	K	=	PKinematics(PPhysicsShellHolder()->Visual());
	if(K)
	{
		K->LL_SetBoneRoot(0);
		K->LL_SetBonesVisible(0xffffffffffffffffL);
	}
	Init();
	ClearUnsplited();
}

void CPHSkeleton::Init()
{
	m_unsplit_time = u32(-1);
	b_removing=false;
	m_startup_anim=NULL;
}

bool CPHSkeleton::Spawn(CSE_Abstract *D)
{
	
	CSE_PHSkeleton *po	= dynamic_cast<CSE_PHSkeleton*>(D);
	R_ASSERT				(po);

	m_flags					= po->_flags;
	CSE_Visual				*visual = dynamic_cast<CSE_Visual*>(D);
	R_ASSERT				(visual);
	m_startup_anim			= visual->startup_animation;
	


	if(po->_flags.test(CSE_PHSkeleton::flSpawnCopy))
	{
		CPHSkeleton* source=dynamic_cast<CPHSkeleton*>(Level().Objects.net_Find(po->source_id));
		R_ASSERT2(source,"no source");
		source->UnsplitSingle(this);
		m_flags.set				(CSE_PHSkeleton::flSpawnCopy,FALSE);
		po->_flags.set				(CSE_PHSkeleton::flSpawnCopy,FALSE);
		return true;
	}
	else 
	{
		CPhysicsShellHolder* obj=PPhysicsShellHolder();

		if (obj->Visual())
		{
			CKinematics* K= PKinematics(obj->Visual());
			if(K)
			{
				K->LL_SetBoneRoot(po->saved_bones.root_bone);
				K->LL_SetBonesVisible(po->saved_bones.bones_mask);
			}
		}
		SpawnInitPhysics(D);
		RestoreNetState(po);
	}
	return false;
}

void CPHSkeleton::Load(LPCSTR section)
{
		remove_time= pSettings->r_u32(section,"remove_time")*1000;
}

void CPHSkeleton::Update(u32 dt)
{
	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	CPhysicsShell* pPhysicsShell=obj->PPhysicsShell();
	if ( pPhysicsShell && pPhysicsShell->isFractured()) //!ai().get_alife() &&
	{
		PHSplit();
	}

	if(b_removing&&(Device.dwTimeGlobal-m_unsplit_time)*phTimefactor>remove_time&&m_unsplited_shels.empty()) 
	{
		NET_Packet			P;
		obj->u_EventGen			(P,GE_DESTROY,obj->ID());
		Msg					("ge_destroy: [%d] - %s",obj->ID(),*(obj->cName()));
		if (obj->Local())	obj->u_EventSend			(P);
		b_removing=false;
	}

}
void CPHSkeleton::SaveNetState(NET_Packet& P)
{

	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	CPhysicsShell* pPhysicsShell=obj->PPhysicsShell();
	CKinematics* K	=PKinematics(obj->Visual());
	if(pPhysicsShell&&pPhysicsShell->bActive)			m_flags.set(CSE_PHSkeleton::flActive,pPhysicsShell->isEnabled());

	P.w_u8 (m_flags.get());
	if(K)
	{
		P.w_u64(K->LL_GetBonesVisible());
		P.w_u16(K->LL_GetBoneRoot());
	}
	else
	{
		P.w_u64(u64(-1));
		P.w_u16(0);
	}
	/////////////////////////////
	Fvector min,max;

	min.set(F_MAX,F_MAX,F_MAX);
	max.set(-F_MAX,-F_MAX,-F_MAX);
	/////////////////////////////////////

	u16 bones_number=obj->PHGetSyncItemsNumber();
	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		obj->PHGetSyncItem(i)->get_State(state);
		Fvector& p=state.position;
		if(p.x<min.x)min.x=p.x;
		if(p.y<min.y)min.y=p.y;
		if(p.z<min.z)min.z=p.z;

		if(p.x>max.x)max.x=p.x;
		if(p.y>max.y)max.y=p.y;
		if(p.z>max.z)max.z=p.z;
	}

	///min.sub(2.f*EPS_S);
	//max.add(2.f*EPS_S);
	P.w_vec3(min);
	P.w_vec3(max);

	P.w_u16(bones_number);

	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		obj->PHGetSyncItem(i)->get_State(state);
		state.net_Save(P,min,max);
	}
}

void CPHSkeleton::LoadNetState(NET_Packet& P)
{
	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	CKinematics* K=PKinematics(obj->Visual());
	P.r_u8 (m_flags.flags);
	if(K)
	{
		K->LL_SetBonesVisible(P.r_u64());
		K->LL_SetBoneRoot(P.r_u16());
	}

	u16 bones_number=P.r_u16();
	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		state.net_Load(P);
		obj->PHGetSyncItem(i)->set_State(state);
	}
}
void CPHSkeleton::RestoreNetState(CSE_PHSkeleton* po)
{
	if(!po->_flags.test(CSE_PHSkeleton::flSavedData))return;
	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	PHNETSTATE_VECTOR& saved_bones=po->saved_bones.bones;
	PHNETSTATE_I i=saved_bones.begin(),e=saved_bones.end();
	for(u16 bone=0;e!=i;i++,bone++)
	{
		R_ASSERT(bone<obj->PHGetSyncItemsNumber());
		obj->PHGetSyncItem(bone)->set_State(*i);
	}
	saved_bones.clear();
	po->_flags.set(CSE_PHSkeleton::flSavedData,FALSE);
	m_flags.set(CSE_PHSkeleton::flSavedData,FALSE);
}


void CPHSkeleton::ClearUnsplited()
{
	SHELL_PAIR_I i=m_unsplited_shels.begin(),e=m_unsplited_shels.end();
	for(;i!=e;++i)
	{
		i->first->Deactivate();
		xr_delete(i->first);
	}
	m_unsplited_shels.clear();
}

void CPHSkeleton::SpawnCopy()
{
	if(PPhysicsShellHolder()->Local()) {
		CSE_Abstract*				D	= F_entity_Create("ph_skeleton_object");//*cNameSect()
		R_ASSERT					(D);
		InitServerObject			(D);
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
}
PHSHELL_PAIR_VECTOR new_shells;
void CPHSkeleton::PHSplit()
{


	u16 spawned=u16(m_unsplited_shels.size());
	PPhysicsShellHolder()->PPhysicsShell()->SplitProcess(m_unsplited_shels);
	u16 i=u16(m_unsplited_shels.size())-spawned;
	//	Msg("%o,spawned,%d",this,i);
	for(;i;--i) SpawnCopy();


}


void __stdcall PushOutCallback2(bool& do_colide,dContact& c);

void CPHSkeleton::UnsplitSingle(CPHSkeleton* SO)
{
	//Msg("%o,received has %d,",this,m_unsplited_shels.size());
	if (0==m_unsplited_shels.size())	return;	//. hack
	CPhysicsShellHolder* obj = PPhysicsShellHolder();
	CPhysicsShellHolder* O =SO->PPhysicsShellHolder();
	VERIFY2(m_unsplited_shels.size(),"NO_SHELLS !!");
	VERIFY2(!O->m_pPhysicsShell,"this has shell already!!!");
	CPhysicsShell* newPhysicsShell=m_unsplited_shels.front().first;
	O->m_pPhysicsShell=newPhysicsShell;
	VERIFY(_valid(newPhysicsShell->mXFORM));
	CKinematics *newKinematics=PKinematics(O->Visual());
	CKinematics *pKinematics  =PKinematics(obj->Visual());

	Flags64 mask0,mask1;
	u16 split_bone=m_unsplited_shels.front().second;
	mask1.assign(pKinematics->LL_GetBonesVisible());//source bones mask
	pKinematics->LL_SetBoneVisible(split_bone,FALSE,TRUE);
	mask0.assign(pKinematics->LL_GetBonesVisible());//first part mask
	VERIFY2(mask0.flags,"mask0 -Zero");
	mask0.invert();
	mask1.and(mask0.flags);//second part mask


	newKinematics->LL_SetBoneRoot		(split_bone);
	VERIFY2(mask1.flags,"mask1 -Zero");
	newKinematics->LL_SetBonesVisible	(mask1.flags);

	newKinematics->CalculateBones_Invalidate	();
	newKinematics->CalculateBones				();

	newPhysicsShell->set_Kinematics(newKinematics);
	VERIFY(_valid(newPhysicsShell->mXFORM));
	newPhysicsShell->ResetCallbacks(split_bone,mask1);
	VERIFY(_valid(newPhysicsShell->mXFORM));

	newPhysicsShell->ObjectInRoot().identity();

	if(!newPhysicsShell->isEnabled())O->processing_deactivate();
	newPhysicsShell->set_PhysicsRefObject(O);
	newPhysicsShell->set_PushOut(5000,PushOutCallback2);
	m_unsplited_shels.erase(m_unsplited_shels.begin());
	O->setVisible(true);
	O->setEnabled(true);


	SO->CopySpawnInit		();
	CopySpawnInit			();


}

void CPHSkeleton::CopySpawnInit()
{
	if(ReadyForRemove()) SetAutoRemove();
}


void CPHSkeleton::SetAutoRemove()
{
	b_removing=true;
	m_unsplit_time=Device.dwTimeGlobal;
	m_flags.set(CSE_PHSkeleton::flNotSave,TRUE);
}


static bool removable;//for RecursiveBonesCheck
void CPHSkeleton::RecursiveBonesCheck(u16 id)
{
	if(!removable) return;
	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	CKinematics* K		= PKinematics(obj->Visual());
	CBoneData& BD		= K->LL_GetData(u16(id));
	//////////////////////////////////////////
	Flags64 mask;
	mask.assign(K->LL_GetBonesVisible());
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
bool CPHSkeleton::ReadyForRemove()
{
	removable=true;
	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	RecursiveBonesCheck(PKinematics(obj->Visual())->LL_GetBoneRoot());
	return removable;
}
void CPHSkeleton::InitServerObject(CSE_Abstract * D)
{

	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT					(l_tpALifeDynamicObject);
	CSE_ALifePHSkeletonObject		*l_tpALifePhysicObject = dynamic_cast<CSE_ALifePHSkeletonObject*>(D);
	R_ASSERT					(l_tpALifePhysicObject);

	l_tpALifePhysicObject->m_tGraphID	=obj->game_vertex_id();
	l_tpALifeDynamicObject->m_tNodeID	= obj->level_vertex_id();
	l_tpALifePhysicObject->set_visual	(*obj->cNameVisual());



	l_tpALifePhysicObject->_flags.set	(CSE_PHSkeleton::flSpawnCopy,1);
	l_tpALifePhysicObject->source_id	= u16(obj->ID());
	l_tpALifePhysicObject->startup_animation=m_startup_anim;
	strcpy				(D->s_name,"ph_skeleton_object");//*cNameSect()
	strcpy				(D->s_name_replace,"");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	0xffff;//u16(ID());//
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	obj->Position();
	obj->XFORM()				.getHPB(D->o_Angle);
	D->s_flags.assign	(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
}