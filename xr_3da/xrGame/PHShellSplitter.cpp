#include "stdafx.h"
#include "Physics.h"
#include "PHShell.h"
#include "PHShellSplitter.h"
#include "PHFracture.h"
#include "PHJointDestroyInfo.h"
CPHShellSplitterHolder::CPHShellSplitterHolder(CPHShell* shell)
{
	m_pShell=shell;
	m_has_breaks=false;
	bActive=false;
}

CPHShellSplitterHolder::~CPHShellSplitterHolder()
{
	Deactivate();
	m_splitters.clear();
}
//the simpliest case - a joint to be destroied 
shell_root CPHShellSplitterHolder::SplitJoint(u16 aspl)
{
//create new physics shell 

CPhysicsShell *new_shell=P_create_Shell();
CPHShell	  *new_shell_desc=dynamic_cast<CPHShell*>(new_shell);
InitNewShell(new_shell_desc);//this called before adding elements because existing elems do not need initialization
SPLITTER_I splitter=m_splitters.begin()+aspl;
u16 start_element=splitter->m_element;
u16 start_joint=splitter->m_joint;

u16 end_element=m_pShell->joints[start_joint]->JointDestroyInfo()->m_end_element;
u16 end_joint=m_pShell->joints[start_joint]->JointDestroyInfo()->m_end_joint;

m_pShell->PassEndElements(start_element,end_element,new_shell_desc);
m_pShell->PassEndJoints(start_joint+1,end_joint,new_shell_desc);
shell_root ret = mk_pair(new_shell,(m_pShell->joints[start_joint])->JointDestroyInfo()->BoneID());
m_pShell->DeleteJoint(start_joint);
//m_splitters.erase(splitter);
//aslp points to the next splitter after this was allready delleted
CShellSplitInfo split_inf;
split_inf.m_bone_id=m_pShell->joints[start_joint]->JointDestroyInfo()->BoneID();
split_inf.m_start_el_num=start_element;
split_inf.m_end_el_num=end_element;
split_inf.m_start_jt_num=start_joint;
split_inf.m_end_jt_num=end_joint;

PassEndSplitters(split_inf,new_shell_desc,1,0);

//start_element+1 the number of elements leaved in source shell
//start_joint+1 the number of joints leaved in source shell and the destroyed joint
return ret;
}

void CPHShellSplitterHolder::PassEndSplitters(const CShellSplitInfo& spl_inf,CPHShell* dest,u16 jt_add_shift,u16 el_add_shift)
{

	
	CPHShellSplitterHolder*	&dest_holder=dest->m_spliter_holder;
	if(!dest_holder)dest_holder=xr_new<CPHShellSplitterHolder>(dest);
	
	ELEMENT_STORAGE &source_elements=m_pShell->elements;
	ELEMENT_I i_elem=source_elements.begin(),e_elem=source_elements.begin()+spl_inf.m_start_el_num;
	u16 shift_e=spl_inf.m_end_el_num-spl_inf.m_start_el_num+1;
	u16 shift_j=spl_inf.m_end_jt_num-spl_inf.m_start_jt_num+1;

	for(;i_elem!=e_elem;i_elem++)	//until start elem in both joint or elem split fractures 
									//end elems have to be corrected 
									//if grater then end elem in moving diapason
	{
		FRACTURE_I f_i=(*i_elem)->FracturesHolder()->m_fractures.begin(),f_e=(*i_elem)->FracturesHolder()->m_fractures.end();
		for(;f_i!=f_e;f_i++)
		{
			u16	&end_el_num=f_i->m_end_el_num;
			if(end_el_num>spl_inf.m_end_el_num)		end_el_num=end_el_num-shift_e;
			u16	&end_jt_num=f_i->m_end_jt_num;
			if(end_jt_num>spl_inf.m_end_jt_num)		end_jt_num=end_jt_num-shift_j;
		}
	}

	//same for joints
	JOINT_STORAGE	&source_joints=m_pShell->joints;
	JOINT_I i_joint=source_joints.begin(), e_joint=source_joints.begin()+spl_inf.m_start_jt_num;
	
	for(;i_joint!=e_joint;i_joint++)	
	{
		u16 &end_element	=		(*i_joint)->JointDestroyInfo()->m_end_element;
		u16	&end_joint		=		(*i_joint)->JointDestroyInfo()->m_end_joint;
		if(end_element>spl_inf.m_end_el_num) end_element=end_element-shift_e;
		if(end_joint>spl_inf.m_end_jt_num)		end_joint=end_joint-shift_j;
	}



//now process diapason that tobe unsplited

	e_elem=source_elements.begin()+spl_inf.m_end_el_num+1;
	u16 passed_shift_e=spl_inf.m_start_el_num-el_add_shift;
	u16 passed_shift_j=spl_inf.m_end_jt_num-jt_add_shift;
	for(;i_elem!=e_elem;i_elem++)	

	{
		FRACTURE_I f_i=(*i_elem)->FracturesHolder()->m_fractures.begin(),f_e=(*i_elem)->FracturesHolder()->m_fractures.end();
		for(;f_i!=f_e;f_i++)
		{
			u16	&end_el_num=f_i->m_end_el_num;
			u16	&start_el_num=f_i->m_start_el_num;
				end_el_num=end_el_num-passed_shift_e;
				start_el_num=start_el_num-passed_shift_e;

			u16	&end_jt_num=f_i->m_end_jt_num;
			u16	&start_jt_num=f_i->m_start_jt_num;
				end_jt_num=end_jt_num-passed_shift_j;
				start_jt_num=start_jt_num-passed_shift_j;
		}
	}
	
	e_joint=source_joints.begin()+spl_inf.m_end_jt_num+1;
	for(;i_joint!=e_joint;i_joint++)	
	{
		u16 &end_element	=		(*i_joint)->JointDestroyInfo()->m_end_element;
		u16	&end_joint		=		(*i_joint)->JointDestroyInfo()->m_end_joint;
		if(end_element>spl_inf.m_end_el_num) end_element=end_element-passed_shift_e;
		if(end_joint>spl_inf.m_end_jt_num)		end_joint=end_joint-passed_shift_j;
	}

//the rest unconditionaly shift end & begin
	e_elem=source_elements.end();
	for(;i_elem!=e_elem;i_elem++)	

	{
		FRACTURE_I f_i=(*i_elem)->FracturesHolder()->m_fractures.begin(),f_e=(*i_elem)->FracturesHolder()->m_fractures.end();
		for(;f_i!=f_e;f_i++)
		{
			u16	&end_el_num=f_i->m_end_el_num;
			u16	&start_el_num=f_i->m_start_el_num;
			end_el_num=end_el_num-shift_e;
			start_el_num=start_el_num-shift_e;

			u16	&end_jt_num=f_i->m_end_jt_num;
			u16	&start_jt_num=f_i->m_start_jt_num;
			end_jt_num=end_jt_num-shift_j;
			start_jt_num=start_jt_num-shift_j;
		}
	}

	e_joint=source_joints.end();
	for(;i_joint!=e_joint;i_joint++)	
	{
		u16 &end_element	=		(*i_joint)->JointDestroyInfo()->m_end_element;
		u16	&end_joint		=		(*i_joint)->JointDestroyInfo()->m_end_joint;
		if(end_element>spl_inf.m_end_el_num) end_element=end_element-shift_e;
		if(end_joint>spl_inf.m_end_jt_num)	 end_joint=end_joint-shift_j;
	}
// at rest!! pass splitters it is very similar passing fractures
// correct data for splitters before passed and find start splitter to pass
	SPLITTER_I spl_e=m_splitters.end(),spl_i=m_splitters.begin();
	for(;spl_i!=spl_e;spl_i++)
	{
		u16	&elem	= spl_i->m_element;
		u16 &joint  = spl_i->m_joint;
		if(spl_i->m_type==CPHShellSplitter::splElement) 
		{
			if(elem>=spl_inf.m_start_el_num) break;//we at begining
		}
		else
		{
			if(joint>=spl_inf.m_start_jt_num) break;//we at begining
		}
		if(elem>spl_inf.m_end_el_num) elem=elem-shift_e;
		if(joint>spl_inf.m_end_jt_num) joint=joint-shift_j;
	}
SPLITTER_I i_from = spl_i;
//correct data for passing splitters and find last splitter to pass
	for(;spl_i!=spl_e;spl_i++)
	{
		u16	&elem	= spl_i->m_element;
		u16 &joint  = spl_i->m_joint;
		if(spl_i->m_type==CPHShellSplitter::splElement) 
		{
			if(elem>=spl_inf.m_end_el_num) break;//we after begining
		}
		else
		{
			if(joint>=spl_inf.m_end_jt_num) break;//we after begining
		}
		if(elem>spl_inf.m_end_el_num) elem=elem-spl_inf.m_start_el_num;
		if(joint>spl_inf.m_end_jt_num) joint=joint-spl_inf.m_start_jt_num;
	}
	SPLITTER_I i_to = spl_i;
	dest_holder->m_splitters.insert(dest_holder->m_splitters.end(),i_from,i_to);
	m_splitters.erase(i_from,i_to);
}

static ELEMENT_PAIR_VECTOR new_elements;
shell_root CPHShellSplitterHolder::ElementSingleSplit(const element_fracture &split_elem)
{

	//const CPHShellSplitter& splitter=m_splitters[aspl];
	//CPHElement* element=m_pShell->elements[splitter.m_element];
	CPhysicsShell *new_shell_last=P_create_Shell();
	CPHShell	  *new_shell_last_desc=dynamic_cast<CPHShell*>(new_shell_last);
	new_shell_last->mXFORM.set(m_pShell->mXFORM);
	
	
	m_pShell->joints[split_elem.second.m_start_jt_num]->ReattachFirstElement(split_elem.first);
	//the last new shell will have all splitted old elements end joints and one new element reattached to old joint

	m_pShell->add_Element(split_elem.first);
	m_pShell->PassEndElements(split_elem.second.m_start_el_num,split_elem.second.m_end_el_num,new_shell_last_desc);

	InitNewShell(new_shell_last_desc);//this cretes space for the shell and add elements to it,place elements to attach joints.....

	m_pShell->PassEndJoints(split_elem.second.m_end_jt_num,split_elem.second.m_end_jt_num,new_shell_last_desc);
	//m_splitters.erase(m_splitters.begin()+aspl);
	//now aspl points to the next splitter
	if((split_elem.first)->FracturesHolder())//if this element can be splitted add a splitter for it
			new_shell_last_desc->m_spliter_holder->m_splitters.push_back(CPHShellSplitter(CPHShellSplitter::splElement,0,0));//

	//pass splitters taking into account that one element was olready added
	PassEndSplitters(split_elem.second,new_shell_last_desc,1,0);


	return mk_pair(new_shell_last,split_elem.second.m_bone_id);

}
void CPHShellSplitterHolder::SplitElement(u16 aspl,PHSHELL_PAIR_VECTOR &out_shels)
{

	new_elements.clear();

	
	m_pShell->elements[(m_splitters.begin()+aspl)->m_element]->SplitProcess(new_elements);


	ELEMENT_PAIR_RI i=new_elements.rbegin(),e=new_elements.rend();

		for(;i!=e;i++)
		{
			out_shels.push_back(ElementSingleSplit(*i));
		}
}

void CPHShellSplitterHolder::SplitProcess(PHSHELL_PAIR_VECTOR &out_shels)
{
	//any split process must start from the end of the elment storage
	//this based on that all childs in the bone hierarchy was added after their parrent
	u16 i=u16(m_splitters.size()-1);
	for(;i!=0;i--)
	{
		switch(m_splitters[i].m_type) 
		{
		case CPHShellSplitter::splJoint:
				 out_shels.push_back(SplitJoint(i));
			break;
		case CPHShellSplitter::splElement:
				SplitElement(i,out_shels);
			break;
		default: NODEFAULT;
		}
	}
}
void CPHShellSplitterHolder::InitNewShell(CPHShell* shell)
{
shell->PresetActive();
}

void CPHShellSplitterHolder::PhTune(dReal step)
{
	SPLITTER_I i=m_splitters.begin(),e=m_splitters.end();
	for(;i!=e;i++)
	{
		switch(i->m_type) 
		{
			case CPHShellSplitter::splElement:
				{
				CPHElement* element=m_pShell->elements[i->m_element];
				element->FracturesHolder()->PhTune(element->get_body());
				break;
				}
			case CPHShellSplitter::splJoint:
				{
				break;
				}
		default: NODEFAULT;
		}
	}
}
void CPHShellSplitterHolder::PhDataUpdate(dReal step)
{
	SPLITTER_I i=m_splitters.begin(),e=m_splitters.end();
	for(;i!=e;i++)
	{
		switch(i->m_type) 
		{
		case CPHShellSplitter::splElement:
			{
				CPHElement* element=m_pShell->elements[i->m_element];
				m_has_breaks=m_has_breaks||(element->FracturesHolder()->PhDataUpdate(element->get_body()));
				break;
			}
		case CPHShellSplitter::splJoint:
			{
				m_has_breaks=m_has_breaks||m_pShell->joints[i->m_joint]->JointDestroyInfo()->Update();
				break;
			}
		default: NODEFAULT;
		}
	}
}
void CPHShellSplitterHolder::Activate()
{
	if(bActive) return;
	CPHObject::Activate();
	bActive=true;
}

void CPHShellSplitterHolder::Deactivate()
{
	if(!bActive)return;
	CPHObject::Deactivate();
	bActive=false;
}
void CPHShellSplitterHolder::AddSplitter(CPHShellSplitter::EType type,u16 element,u16 joint)
{
	m_splitters.push_back(CPHShellSplitter(type,element,joint));
}
CPHShellSplitter::CPHShellSplitter(CPHShellSplitter::EType type,u16 element,u16 joint)
{
	m_breaked=false;
	m_type=type;
	m_element=element;
	m_joint=joint;
}

CPHShellSplitter::CPHShellSplitter()
{
	m_breaked=false;
}

