#include "stdafx.h"
#include "PHFracture.h"
#include "Physics.h"
#include "PHElement.h"
CPHFracturesHolder::CPHFracturesHolder()
{
	m_has_breaks=false;
}
CPHFracturesHolder::~CPHFracturesHolder()
{
	m_has_breaks=false;
	m_fractures.clear();
	m_impacts.clear();
	m_feedbacks.clear();
}
CPHElement* CPHFracturesHolder::SplitFromEnd(CPHElement* element,u16 fracture)
{
	FRACTURE_I fract_i=m_fractures.begin()+fracture;
	u16 geom_num=fract_i->m_geom_num;
	CPHElement* new_element=dynamic_cast<CPHElement*>(P_create_Element());
	InitNewElement(new_element);
	element->PassEndGeoms(geom_num,new_element);
	m_fractures.erase(fract_i);
	//fracture now points to next fracture
	//if we have another fractures after this one pass them to the created element starting from fracture(next to argument fracture)
	//shift geoms in fractures by number of geoms leaved in source element
	if(m_fractures.size()-fracture>0) 
	{	
		if(new_element->m_fratures_holder==NULL)//create fractures holder if it was not created before
		{
			new_element->m_fratures_holder=xr_new<CPHFracturesHolder>();
		}
		PassEndFractures(fracture,new_element,geom_num+1);
	}
	return new_element;
}

void CPHFracturesHolder::PassEndFractures(u16 from,CPHElement* dest,u16 shift_geoms)
{
	FRACTURE_I i_from=m_fractures.begin()+from,e=m_fractures.end();
	if(i_from==e) return;
	CPHFracturesHolder* &dest_fract_holder=dest->m_fratures_holder;
	if(!dest_fract_holder) dest_fract_holder=xr_new<CPHFracturesHolder>();
	for(FRACTURE_I i=i_from;i!=e;i++)
	{
		i->m_geom_num=i->m_geom_num-shift_geoms;
	}

	dest_fract_holder->m_fractures.insert(dest_fract_holder->m_fractures.begin(),i_from,e);
	m_fractures.erase(i_from,e);
}

void CPHFracturesHolder::SplitProcess(CPHElement* element,ELEMENT_STORAGE &new_elements)
{
	FRACTURE_RI i=m_fractures.rbegin(),e=m_fractures.rend();//reversed
	for(;i!=e;i++)
	{
		if(i->Breaked())
		{
			new_elements.push_back(SplitFromEnd(element,i->m_geom_num));
		}
	}
}

void CPHFracturesHolder::InitNewElement(CPHElement* element)
{
element->CreateSimulBase();
element->ReInitDynamics();
}

void CPHFracturesHolder::PhTune(dBodyID body)
{
	//iterate through all body's joints and set joints feedbacks where is not already set
	//contact feedbacks stored in global storage - ContactFeedBacks wich cleared on each step
	//breacable joints already has their feedbacks, 
	//feedbacks for rest noncontact joints stored in m_feedbacks in runtime in this function and
	//and killed by destructor

	//int dBodyGetNumJoints (dBodyID b);
	//dJointID dBodyGetJoint (dBodyID, int index);
	//dJointGetType
	//dJointTypeContact

	int num=dBodyGetNumJoints(body);
	for(int i=0;i<num;i++)
	{
		dJointID joint=dBodyGetJoint(body,i);

		if(dJointGetType(joint)==dJointTypeContact)
		{
			dJointSetFeedback(joint,ContactFeedBacks.add());
		}
		else
		{
			if(!dJointGetFeedback(joint))
			{
				m_feedbacks.push_back(dJointFeedback());
				dJointSetFeedback(joint,&m_feedbacks.back());
			}
		}
	}

}
bool CPHFracturesHolder::PhDataUpdate(dBodyID body)
{
	FRACTURE_I i=m_fractures.begin(),e=m_fractures.end();
	for(;i!=e;i++)
	{
		m_has_breaks=m_has_breaks||i->Update(m_impacts,body);
	}
	return m_has_breaks;
}

void CPHFracturesHolder::AddImpact(const Fvector& force,const Fvector& point)
{
	m_impacts.push_back(SPHImpact(force,point));
}
void CPHFracturesHolder::AddFracture(u16 geom_num,const Fvector& position,const Fvector& direction,const float& break_force,const float& break_torque)
{
	m_fractures.push_back(CPHFracture(geom_num,position,direction,break_force,break_torque));
}
CPHFracture::CPHFracture(u16 geom_num,const Fvector& position,const Fvector& direction,const float& break_force,const float& break_torque)
{
m_geom_num=geom_num;
m_position.set(position);
m_direction.set(direction);
m_break_force=break_force;
m_break_torque=break_torque;
m_breaked=false;
}
bool CPHFracture::Update(PH_IMPACT_STORAGE& impacts,dBodyID body)
{
	
	return false;
}