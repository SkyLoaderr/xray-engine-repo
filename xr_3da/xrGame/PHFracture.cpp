#include "stdafx.h"
#include "PHFracture.h"
#include "Physics.h"
#include "PHElement.h"
#include "PHShell.h"
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
element_fracture CPHFracturesHolder::SplitFromEnd(CPHElement* element,u16 fracture)
{
	FRACTURE_I fract_i		=m_fractures.begin()+fracture;
	u16 geom_num			=fract_i->m_start_geom_num;
	u16 end_geom_num		=fract_i->m_end_geom_num;

	CPHElement* new_element	=dynamic_cast<CPHElement*>(P_create_Element());
	new_element->m_SelfID=fract_i->m_bone_id;
	new_element->mXFORM.set(element->mXFORM);
	element->PassEndGeoms(geom_num,end_geom_num,new_element);
	/////////////////////////////////////////////
	CKinematics* pKinematics= element->m_shell->PKinematics();
	const CBoneInstance& new_bi=pKinematics->LL_GetBoneInstance(new_element->m_SelfID);
	const CBoneInstance& old_bi=pKinematics->LL_GetBoneInstance(element->m_SelfID);
	Fmatrix shift_pivot;
	shift_pivot.set(new_bi.mTransform);
	shift_pivot.invert();
	shift_pivot.mulB(old_bi.mTransform);
	/////////////////////////////////////////////
	InitNewElement(new_element,shift_pivot,element->getDensity());
	

	element_fracture ret	=mk_pair(new_element,(CShellSplitInfo)(*fract_i));

	if(m_fractures.size()-fracture>0) 
	{	
		if(new_element->m_fratures_holder==NULL)//create fractures holder if it was not created before
		{
			new_element->m_fratures_holder=xr_new<CPHFracturesHolder>();
		}
		PassEndFractures(fracture,new_element);
	}
	return ret;
}

void CPHFracturesHolder::PassEndFractures(u16 from,CPHElement* dest)
{
	FRACTURE_I i=m_fractures.begin(),i_from=m_fractures.begin()+from,e=m_fractures.end();
	u16 end_geom=i_from->m_end_geom_num;
	u16 begin_geom_num=i_from->m_start_geom_num;
	u16 leaved_geoms=begin_geom_num+1;
	u16 passed_geoms=end_geom-begin_geom_num;
	if(i_from==e) return;

	for(;i!=i_from;i++)//correct end geoms for fractures leaved in source
	{
		u16& cur_end_geom=i->m_end_geom_num;
		if(cur_end_geom>begin_geom_num) cur_end_geom=cur_end_geom-passed_geoms;
	}

	i++; // omit used fracture;

	for(;i!=e;i++)//itterate antil a fracture where geom num > end geom num
	{
		u16 &cur_end_geom	=i->m_end_geom_num;
		u16 &cur_geom		=i->m_end_geom_num;
		if(cur_geom>end_geom) break;
		cur_end_geom=cur_end_geom-leaved_geoms;
		cur_geom=cur_geom-leaved_geoms;
	}
	FRACTURE_I i_to=i;
	for(;i!=e;i++)//correct data in the rest leaved fractures
	{
		u16 &cur_end_geom	=i->m_end_geom_num;
		u16 &cur_geom		=i->m_end_geom_num;
		cur_end_geom		=cur_end_geom-passed_geoms;
		cur_geom			=cur_end_geom-passed_geoms;
	}

	if(i_from+1!=i_to)//insure it!!
	{
	
	CPHFracturesHolder* &dest_fract_holder=dest->m_fratures_holder;
	if(!dest_fract_holder) dest_fract_holder=xr_new<CPHFracturesHolder>();
	//pass fractures not including end fracture
	dest_fract_holder->m_fractures.insert(dest_fract_holder->m_fractures.end(),i_from+1,i_to);
	//erase passed fracture allong whith used fracture

	}
	m_fractures.erase(i_from,i_to);//erase along whith used fracture
}
void CPHFracturesHolder::SplitProcess(CPHElement* element,ELEMENT_PAIR_VECTOR &new_elements)
{
	//FRACTURE_RI i=m_fractures.rbegin(),e=m_fractures.rend();//reversed
	u16 i=u16(m_fractures.size()-1);

	for(;i!=u16(-1);i--)
	{
		if(m_fractures[i].Breaked())
		{
			new_elements.push_back(SplitFromEnd(element,i));
		}
	}
}

void CPHFracturesHolder::InitNewElement(CPHElement* element,const Fmatrix &shift_pivot,float density)
{
element->CreateSimulBase();
element->ReInitDynamics(shift_pivot,density);
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
		m_has_breaks=i->Update(m_impacts,body)||m_has_breaks;
	}
	return m_has_breaks;
}

void CPHFracturesHolder::AddImpact(const Fvector& force,const Fvector& point)
{
	m_impacts.push_back(SPHImpact(force,point));
}
CPHFracture& CPHFracturesHolder::AddFracture(const CPHFracture& fracture)
{
	m_fractures.push_back(fracture);
	return m_fractures.back();
}
CPHFracture::CPHFracture()
{
//m_bone_id=bone_id;
//m_position.set(position);
//m_direction.set(direction);
//m_break_force=break_force;
//m_break_torque=break_torque;
m_breaked=false;
}
bool CPHFracture::Update(PH_IMPACT_STORAGE& impacts,dBodyID body)
{
	//itterate through impacts & calculate 
	m_breaked=true;
	return m_breaked;
}

void CPHFracture::SetMassParts(const dMass& first,const dMass& second)
{
	m_firstM=first;
	m_seccondM=second;
}