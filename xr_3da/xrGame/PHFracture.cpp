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

void CPHFracturesHolder::DistributeAdditionalMass(u16 geom_num,const dMass& m)
{
	FRACTURE_I f_i=m_fractures.begin(),f_e=m_fractures.end();
	for(;f_i!=f_e;f_i++)
	{
		R_ASSERT2(f_i->m_start_geom_num!=u16(-1),"fracture does not initialized!");

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
	dMass& second_mass=fracture->m_seccondM;
	dMass& first_mass=fracture->m_firstM;
	for(;f_i!=f_e;f_i++)
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

void CPHFracture::MassAddToFirst(const dMass& m)
{
	dMassAdd(&m_firstM,&m);
}

void CPHFracture::MassAddToSecond(const dMass& m)
{
	dMassAdd(&m_seccondM,&m);
}
void CPHFracture::MassSubFromFirst(const dMass& m)
{
	dMassSub(&m_firstM,&m);
}
void CPHFracture::MassSubFromSecond(const dMass& m)
{
	dMassSub(&m_seccondM,&m);
}
void CPHFracture::MassSetFirst(const dMass& m)
{
	m_firstM=m;
}
void CPHFracture::MassSetSecond(const dMass& m)
{
	m_seccondM=m;
}
void CPHFracture::MassUnsplitFromFirstToSecond(const dMass& m)
{
	dMassSub(&m_firstM,&m);
	dMassAdd(&m_seccondM,&m);
}
void CPHFracture::MassSetZerro()
{
	dMassSetZero(&m_firstM);
	dMassSetZero(&m_seccondM);
}