#include "stdafx.h"
#include "PHFracture.h"
#include "Physics.h"
#include "PHElement.h"
#include "PHShell.h"
#pragma warning(disable:4995)
#pragma warning(disable:4267)
#include "../ode/src/joint.h"
#pragma warning(default:4995)
#pragma warning(default:4267)

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
	SubFractureMass			(fracture);

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
	float density=element->getDensity();
	InitNewElement(new_element,shift_pivot,density);

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
	u16 leaved_geoms=begin_geom_num;
	u16 passed_geoms=end_geom-begin_geom_num;
	if(i_from==e) return;

	for(;i!=i_from;++i)//correct end geoms for fractures leaved in source
	{
		u16& cur_end_geom=i->m_end_geom_num;
		if(cur_end_geom>begin_geom_num) cur_end_geom=cur_end_geom-passed_geoms;
	}

	i++; // omit used fracture;

	for(;i!=e;i++)//itterate antil a fracture where geom num > end geom num
	{
		u16 &cur_end_geom	=i->m_end_geom_num;
		u16 &cur_geom		=i->m_start_geom_num;
		if(cur_geom>=end_geom) break;
		cur_end_geom=cur_end_geom-leaved_geoms;
		cur_geom=cur_geom-leaved_geoms;
	}
	FRACTURE_I i_to=i;
	for(;i!=e;++i)//correct data in the rest leaved fractures
	{
		u16 &cur_end_geom	=i->m_end_geom_num;
		u16 &cur_geom		=i->m_start_geom_num;
		cur_end_geom		=cur_end_geom-passed_geoms;
		cur_geom			=cur_geom-passed_geoms;
	}

	if(i_from + 1 != i_to)//insure it!!
	{
	
	CPHFracturesHolder* &dest_fract_holder=dest->m_fratures_holder;
	if(!dest_fract_holder) dest_fract_holder=xr_new<CPHFracturesHolder>();
	//pass fractures not including end fracture
	dest_fract_holder->m_fractures.insert(dest_fract_holder->m_fractures.end(),i_from+1,i_to);

	//u16 deb=u16(i_to-i_from-1);
	//deb++;deb--;
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
			float density = element->getDensity();
			new_elements.push_back(SplitFromEnd(element,i));
			element->ResetMass(density);
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
	for(int i=0;i<num;++i)
	{
		dJointID joint=dBodyGetJoint(body,i);

		if(dJointGetType(joint)==dJointTypeContact)
		{
			dJointSetFeedback(joint,ContactFeedBacks.add());
		}
		else
		{
			CPHJoint* ph_joint=(CPHJoint*)dJointGetData(joint);
			if(!(ph_joint&&ph_joint->JointDestroyInfo())) dJointSetFeedback(joint,ContactFeedBacks.add());
			//if(!dJointGetFeedback(joint))
			//{
			//	m_feedbacks.push_back(dJointFeedback());
			//	dJointSetFeedback(joint,&m_feedbacks.back());
			//}
		}
	}

}
bool CPHFracturesHolder::PhDataUpdate(CPHElement* element)
{
	FRACTURE_I i=m_fractures.begin(),e=m_fractures.end();
	for(;i!=e;++i)
	{
		m_has_breaks=i->Update(element)||m_has_breaks;
	}
	m_impacts.clear();
	return m_has_breaks;
	
}

void CPHFracturesHolder::AddImpact(const Fvector& force,const Fvector& point,u16 id)
{
	m_impacts.push_back(SPHImpact(force,point,id));
}
u16 CPHFracturesHolder::AddFracture(const CPHFracture& fracture)
{
	m_fractures.push_back(fracture);
	return u16(m_fractures.size()-1);
}
CPHFracture& CPHFracturesHolder::Fracture(u16 num)
{
	R_ASSERT2(num<m_fractures.size(),"out of range!");
	return m_fractures[num];
}
void CPHFracturesHolder::DistributeAdditionalMass(u16 geom_num,const dMass& m)
{
	FRACTURE_I f_i=m_fractures.begin(),f_e=m_fractures.end();
	for(;f_i!=f_e;++f_i)
	{
		R_ASSERT2(u16(-1) != f_i->m_start_geom_num,"fracture does not initialized!");

			if(f_i->m_end_geom_num==u16(-1))f_i->MassAddToSecond(m)	;
			else							f_i->MassAddToFirst(m)	;
		

		
		//f_i->MassAddToFirst(m);
	}
}
void CPHFracturesHolder::SubFractureMass(u16 fracture_num)
{
	FRACTURE_I f_i=m_fractures.begin(),f_e=m_fractures.end();
	FRACTURE_I fracture=f_i+fracture_num;
	u16 start_geom=fracture->m_start_geom_num;
	u16	end_geom  =fracture->m_end_geom_num;
	dMass& second_mass=fracture->m_secondM;
	dMass& first_mass=fracture->m_firstM;
	for(;f_i!=f_e;++f_i)
	{
		if(f_i==fracture) continue;
		R_ASSERT2(start_geom!=f_i->m_start_geom_num,"Double fracture!!!");

		
		if(start_geom>f_i->m_start_geom_num)
		{
			
			if(end_geom<=f_i->m_end_geom_num)	f_i->MassSubFromSecond(second_mass);//tag fracture is in current
			else
			{
				R_ASSERT2(start_geom>=f_i->m_end_geom_num,"Odd fracture!!!");
				f_i->MassSubFromFirst(second_mass);//tag fracture is ouside current
			}
		}
		else
		{

			if(end_geom>=f_i->m_end_geom_num) f_i->MassSubFromFirst(first_mass);//current fracture is in tag
			else
			{
				R_ASSERT2(end_geom<=f_i->m_start_geom_num,"Odd fracture!!!");
				f_i->MassSubFromFirst(second_mass);//tag fracture is ouside current
			}
		}
	}
}

CPHFracture::CPHFracture()
{
//m_bone_id=bone_id;
//m_position.set(position);
//m_direction.set(direction);
//m_break_force=break_force;
//m_break_torque=break_torque;
m_start_geom_num=u16(-1);
m_end_geom_num	=u16(-1);
m_breaked=false;
}

//#define DBG_BREAK
bool CPHFracture::Update(CPHElement* element)
{

	////itterate through impacts & calculate 
	//dBodyID body=element->get_body();
	//CPHFracturesHolder* holder=element->FracturesHolder();
	//PH_IMPACT_STORAGE&	impacts=holder->Impacts();
	//int num=dBodyGetNumJoints(body);
	//for(int i=0;i<num;i++)
	//{
	//	dJointID joint=dBodyGetJoint(body,i);
	//	dJointFeedback* feedback=dJointGetFeedback(joint);
	//	//R_ASSERT(feedback,"Feedback was not set!!!");
	//
	//}

	Fvector break_force;//=1/(m1+m2)*(F1*m2-F2*m1)+r2xT2/(r2^2)-r1xT1/(r1^2)
	break_force.set(first_part_force);
	break_force.mul(m_secondM.mass);
	vtemp.set(second_part_force);
	vtemp.mul(m_firstM.mass);
	break_force.sub(vtemp);
	break_force.mul(1.f/body->mass.mass);
	
	//vtemp.crossproduct(second_in_bone,second_part_torque);
	//break_force.add(vtemp);
	//vtemp.crossproduct(first_in_bone,first_part_torque);
	//break_force.sub(vtemp);
#ifdef DBG_BREAK		
	float bfm_dbg=break_force.magnitude();
#endif
	if(m_break_force<break_force.magnitude())
	{
		m_breaked=true;
#ifndef DBG_BREAK		
		return m_breaked;
#endif
	}
#ifdef DBG_BREAK
Msg("bone_id %d break_torque - %f(max %f) break_force %f (max %f)",m_bone_id,btm_dbg,m_break_torque,bfm_dbg,m_break_force);
#endif
	return m_breaked;
}

void CPHFracture::SetMassParts(const dMass& first,const dMass& second)
{
	m_firstM=first;
	m_secondM=second;
}

void CPHFracture::MassAddToFirst(const dMass& m)
{
	dMassAdd(&m_firstM,&m);
}

void CPHFracture::MassAddToSecond(const dMass& m)
{
	dMassAdd(&m_secondM,&m);
}
void CPHFracture::MassSubFromFirst(const dMass& m)
{
	dMassSub(&m_firstM,&m);
}
void CPHFracture::MassSubFromSecond(const dMass& m)
{
	dMassSub(&m_secondM,&m);
}
void CPHFracture::MassSetFirst(const dMass& m)
{
	m_firstM=m;
}
void CPHFracture::MassSetSecond(const dMass& m)
{
	m_secondM=m;
}
void CPHFracture::MassUnsplitFromFirstToSecond(const dMass& m)
{
	dMassSub(&m_firstM,&m);
	dMassAdd(&m_secondM,&m);
}
void CPHFracture::MassSetZerro()
{
	dMassSetZero(&m_firstM);
	dMassSetZero(&m_secondM);
}