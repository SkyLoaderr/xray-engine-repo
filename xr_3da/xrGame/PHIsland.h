#ifndef	PH_ISLAND_H
#define PH_ISLAND_H

#pragma warning(disable:4995)
#pragma warning(disable:4267)
#include <../ode/src/objects.h>
#include <../ode/src/joint.h>
#pragma warning(default:4995)
#pragma warning(default:4267)

#include "PhysicsCommon.h"
class CPHIsland: public dxWorld
{
bool			b_active				;
dxBody			*m_first_body			;
dxJoint			*m_first_joint			;
dxJoint			**m_joints_tail			;
dxBody			**m_bodies_tail			;
CPHIsland		*m_self_active			;
int				m_nj					;
int				m_nb					;

public:
IC	dWorldID		DWorld()
{
	return (dWorldID)this;
}
IC	dWorldID		DActiveWorld()
{
	return (dWorldID)DActiveIsland();
}
IC CPHIsland* DActiveIsland()
{
	GoActive();
	return	m_self_active;
}
IC	void GoActive()
{
	while (!m_self_active->b_active) m_self_active=m_self_active->m_self_active;
}
IC	void			Merge(CPHIsland* island)
{
	//VERIFY2(b_active&&island->b_active,"no active island");
	CPHIsland* first_island=DActiveIsland();
	CPHIsland* second_island=island->DActiveIsland();
	if(first_island==second_island)return;

	*(second_island->m_joints_tail)=first_island->firstjoint;
	first_island->firstjoint=second_island->firstjoint;
	if(0==first_island->nj&&0!=second_island->nj)
	{
		first_island->m_joints_tail=second_island->m_joints_tail;
	}

	*(second_island->m_bodies_tail)=first_island->firstbody;
	first_island->firstbody=second_island->firstbody;

	first_island->nj+=second_island->nj;
	first_island->nb+=second_island->nb;
	VERIFY(!(*(first_island->m_bodies_tail)));
	VERIFY(!(*(first_island->m_joints_tail)));
	VERIFY(!((!(first_island->nj))&&(first_island->firstjoint)));
	second_island->m_self_active=first_island;
	second_island->b_active=false;
}
IC	void		Unmerge()
{
	firstjoint=m_first_joint;
	firstbody=m_first_body;
	if(!m_nj)
	{
		m_joints_tail=&firstjoint;
		*m_joints_tail=0;
	}
	else
	{
		firstjoint->tome=(dObject**)&firstjoint;
	}
	*m_joints_tail=0;
	*m_bodies_tail=0;
	b_active=true;
	m_self_active=this;
	nj=m_nj;
	nb=m_nb;

}
IC	void			Init()
{
	b_active=true;
	m_nj=nj=0;
	m_nb=nb=0;
	m_first_joint=firstjoint=0;
	m_first_body=firstbody=0;
	m_joints_tail=&firstjoint;
	m_bodies_tail=&firstbody;
	m_self_active=this;
}
IC	void			AddBody(dxBody* body)
{
	VERIFY2(m_nj==nj&&m_nb==nb&&b_active,"can not remove/add during processing phase");
	dWorldAddBody(DWorld(),body);
	m_first_body=body;
	if(m_nb==0)
	{
		m_bodies_tail=(dxBody**)&body->next;
	}
	m_nb++;
}
IC void				RemoveBody(dxBody* body)
{
	VERIFY2(m_nj==nj&&m_nb==nb&&b_active,"can not remove/add during processing phase");
	if(m_first_body==body)m_first_body=(dxBody*)body->next;
	if(m_bodies_tail==(dxBody**)(&(body->next)))
	{
		m_bodies_tail=(dxBody**)body->tome;
	}
	dWorldRemoveBody((dxWorld*)this,body);
	m_nb--;
}
IC	void			AddJoint(dxJoint* joint)
{
	VERIFY2(m_nj==nj&&m_nb==nb&&b_active,"can not remove/add during processing phase");
	dWorldAddJoint(DWorld(),joint);
	m_first_joint=joint;
	if(!m_nj)
	{
		VERIFY(joint->next==0);
		m_joints_tail=(dxJoint**)(&(joint->next));

	}
	m_nj++;
}

IC void				ConnectJoint(dxJoint* joint)
{
	if(!nj)
	{
		m_joints_tail=(dxJoint**)(&(joint->next));
		VERIFY(!firstjoint);
	}
	dWorldAddJoint(DWorld(),joint);
	VERIFY(!(*(m_joints_tail)));
}

IC void				DisconnectJoint(dxJoint* joint)
{
		dWorldRemoveJoint(DWorld(),joint);
}

IC				ConnectBody(dxBody* body)
{
		dWorldAddBody(DWorld(),body);
}
IC				DisconnectBody(dxBody* body)
{
		dWorldRemoveBody(DWorld(),body);
}
IC void			RemoveJoint(dxJoint* joint)
{
	VERIFY2(m_nj==nj&&m_nb==nb&&b_active,"can not remove/add during processing phase");
	if(m_first_joint==joint)
		m_first_joint=(dxJoint*)joint->next;
	if(m_joints_tail==(dxJoint**)(&(joint->next)))
	{
		m_joints_tail=(dxJoint**)joint->tome;
	}
	dWorldRemoveJoint(DWorld(),joint);
	VERIFY(!*(m_joints_tail));
	m_nj--;
}

void			Step(dReal step);
protected:
private:
};
#endif