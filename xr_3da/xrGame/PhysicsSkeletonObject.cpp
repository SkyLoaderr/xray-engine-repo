#include "stdafx.h"
#include "physicsskeletonobject.h"
#include "PhysicsShell.h"
#include "phsynchronize.h"
#include "xrserver_objects_alife.h"
#include "level.h"

#define F_MAX         3.402823466e+38F
u32 CPhysicsSkeletonObject::remove_time=5000;

CPhysicsSkeletonObject::CPhysicsSkeletonObject()
{
	Init();
}

CPhysicsSkeletonObject::~CPhysicsSkeletonObject()
{
	ClearUnsplited();
}

void CPhysicsSkeletonObject::RespawnInit()
{
	CKinematics*	K	=	PKinematics(Visual());
	if(K)
	{
		K->LL_SetBoneRoot(0);
		K->LL_SetBonesVisible(0xffffffffffffffffL);
	}
	Init();
	ClearUnsplited();
}

void CPhysicsSkeletonObject::Init()
{
	m_unsplit_time = u32(-1);
	b_removing=false;
	m_startup_anim=NULL;
}

void CPhysicsSkeletonObject::SaveNetState(NET_Packet& P)
{

	CKinematics* K	=PKinematics(Visual());
	if(m_pPhysicsShell&&m_pPhysicsShell->bActive)
		m_flags.set(CSE_ALifePHSkeletonObject::flActive,m_pPhysicsShell->isEnabled());

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

	u16 bones_number=PHGetSyncItemsNumber();
	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);
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
		PHGetSyncItem(i)->get_State(state);
		state.net_Save(P,min,max);
	}
}

void CPhysicsSkeletonObject::LoadNetState(NET_Packet& P)
{

	CKinematics* K=PKinematics(Visual());
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
		PHGetSyncItem(i)->set_State(state);
	}
}
void CPhysicsSkeletonObject::RestoreNetState(PHNETSTATE_VECTOR& saved_bones)
{
	PHNETSTATE_I i=saved_bones.begin(),e=saved_bones.end();
	for(u16 bone=0;e!=i;i++,bone++)
	{
		R_ASSERT(bone<PHGetSyncItemsNumber());
		PHGetSyncItem(bone)->set_State(*i);
	}
	saved_bones.clear();
	m_flags.set(CSE_ALifeObjectPhysic::flSavedData,FALSE);
}


void CPhysicsSkeletonObject::ClearUnsplited()
{
	SHELL_PAIR_I i=m_unsplited_shels.begin(),e=m_unsplited_shels.end();
	for(;i!=e;++i)
	{
		i->first->Deactivate();
		xr_delete(i->first);
	}
}





BOOL CPhysicsSkeletonObject::net_Spawn(LPVOID DC)
{
	CSE_Abstract			  *e	= (CSE_Abstract*)(DC);
	CSE_ALifePHSkeletonObject *po	= dynamic_cast<CSE_ALifePHSkeletonObject*>(e);
	R_ASSERT				(po);
	inherited::net_Spawn	(DC);

	m_flags					= po->flags;
	m_startup_anim			= po->startup_animation;

	if(po->flags.test(CSE_ALifeObjectPhysic::flSpawnCopy))
	{
		CPhysicsSkeletonObject* source=dynamic_cast<CPhysicsSkeletonObject*>(Level().Objects.net_Find(po->source_id));
		R_ASSERT2(source,"no source");
		source->UnsplitSingle(this);
	}
	

 

	m_flags.set				(CSE_ALifePHSkeletonObject::flSpawnCopy,FALSE);

	return TRUE;
}

void CPhysicsSkeletonObject::net_Destroy()
{
	RespawnInit();
	inherited::net_Destroy();
	CKinematics* K=PKinematics(Visual());
	if(K)	K->Calculate();
}

void CPhysicsSkeletonObject::Load(LPCSTR section)
{
	inherited::Load(section);
	remove_time= pSettings->r_u32(section,"remove_time")*1000;
}

void CPhysicsSkeletonObject::CreatePhysicsShell(CSE_Abstract* po)
{

}


void CPhysicsSkeletonObject::shedule_Update(u32 dt)
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

void CPhysicsSkeletonObject::net_Save(NET_Packet &P)
{
	inherited::net_Save(P);
	SaveNetState	   (P);
}

void CPhysicsSkeletonObject::SpawnCopy()
{
	if(Local()) {
		CSE_Abstract*				D	= F_entity_Create(*cNameSect());
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
void CPhysicsSkeletonObject::PHSplit()
{


	u16 spawned=u16(m_unsplited_shels.size());
	m_pPhysicsShell->SplitProcess(m_unsplited_shels);
	u16 i=u16(m_unsplited_shels.size())-spawned;
	//	Msg("%o,spawned,%d",this,i);
	for(;i;--i) SpawnCopy();


}


void __stdcall PushOutCallback2(bool& do_colide,dContact& c);

void CPhysicsSkeletonObject::UnsplitSingle(CPhysicsSkeletonObject* O)
{
	//Msg("%o,received has %d,",this,m_unsplited_shels.size());
	//if (0==m_unsplited_shels.size())	return;	//. hack
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
	if(!newPhysicsShell->isEnabled())O->processing_deactivate();
	newPhysicsShell->set_PhysicsRefObject(O);
	newPhysicsShell->set_PushOut(5000,PushOutCallback2);
	m_unsplited_shels.erase(m_unsplited_shels.begin());
	O->setVisible(true);
	O->setEnabled(true);
	newKinematics->Calculate();


	O->CopySpawnInit		();
	CopySpawnInit			();


}

void CPhysicsSkeletonObject::CopySpawnInit()
{
	if(ReadyForRemove()) SetAutoRemove();
}


void CPhysicsSkeletonObject::SetAutoRemove()
{
	b_removing=true;
	m_unsplit_time=Device.dwTimeGlobal;
	m_flags.set(CSE_ALifePHSkeletonObject::flNotSave);
}


static bool removable;//for RecursiveBonesCheck
void CPhysicsSkeletonObject::RecursiveBonesCheck(u16 id)
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
bool CPhysicsSkeletonObject::ReadyForRemove()
{
	removable=true;
	RecursiveBonesCheck(PKinematics(Visual())->LL_GetBoneRoot());
	return removable;
}
void CPhysicsSkeletonObject::InitServerObject(CSE_Abstract * D)
{
	

	CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT					(l_tpALifeDynamicObject);
	CSE_ALifePHSkeletonObject		*l_tpALifePhysicObject = dynamic_cast<CSE_ALifePHSkeletonObject*>(D);
	R_ASSERT					(l_tpALifePhysicObject);

	l_tpALifePhysicObject->m_tGraphID	= game_vertex_id();
	l_tpALifeDynamicObject->m_tNodeID	= level_vertex_id();
	l_tpALifePhysicObject->set_visual	(*cNameVisual());



	l_tpALifePhysicObject->flags.set	(CSE_ALifePHSkeletonObject::flSpawnCopy,1);
	l_tpALifePhysicObject->source_id	= u16(ID());
	l_tpALifePhysicObject->startup_animation=m_startup_anim;
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
}