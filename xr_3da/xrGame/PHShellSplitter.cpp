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
m_pShell->PassEndElements(start_element,new_shell_desc,0);
m_pShell->PassEndJoints(start_joint+1,new_shell_desc);
shell_root ret = mk_pair(new_shell,(m_pShell->joints[start_joint])->JointDestroyInfo()->BoneID());
m_pShell->DeleteJoint(start_joint);
m_splitters.erase(splitter);
//aslp points to the next splitter after this was allready delleted
PassEndSplitters(aspl,new_shell_desc,0,start_element+1,start_joint+1);

//start_element+1 the number of elements leaved in source shell
//start_joint+1 the number of joints leaved in source shell and the destroyed joint
return ret;
}

void CPHShellSplitterHolder::PassEndSplitters(u16 from,CPHShell* dest,u16 position,u16 shift_elements,u16 shift_joints)
{
	SPLITTER_I i_from=m_splitters.begin()+from,e=m_splitters.end();
	CPHShellSplitterHolder*	&dest_holder=dest->m_spliter_holder;
	if(!dest_holder)dest_holder=xr_new<CPHShellSplitterHolder>(dest);

	for(SPLITTER_I i=i_from;i!=e;i++)
	{
		i->m_element	= i->m_element	- shift_elements;
		i->m_joint		= i->m_joint	- shift_joints;
	}
	dest_holder->m_splitters.insert(dest_holder->m_splitters.begin()+position,i_from,e);
	m_splitters.erase(i_from,e);
}

static ELEMENT_PAIR_VECTOR new_elements;
void CPHShellSplitterHolder::SplitElement(u16 aspl,PHSHELL_PAIR_VECTOR &out_shels)
{

	new_elements.clear();
	const CPHShellSplitter& splitter=m_splitters[aspl];
	CPHElement* element=m_pShell->elements[splitter.m_element];
	CPhysicsShell *new_shell_last=P_create_Shell();
	CPHShell	  *new_shell_last_desc=dynamic_cast<CPHShell*>(new_shell_last);
	new_shell_last->mXFORM.set(m_pShell->mXFORM);

	element->SplitProcess(new_elements);
	ELEMENT_PAIR_RI i=new_elements.rbegin(),e=new_elements.rend();
	m_pShell->joints[splitter.m_joint]->ReattachFirstElement(i->first);
	//the last new shell will have all splitted old elements end joints and one new element reattached to old joint

	m_pShell->add_Element(i->first);
	m_pShell->PassEndElements(splitter.m_element+1,new_shell_last_desc,1);
	
	InitNewShell(new_shell_last_desc);//this cretes space for the shell and add elements to it,place elements to attach joints.....

	m_pShell->PassEndJoints(splitter.m_joint,new_shell_last_desc);
	m_splitters.erase(m_splitters.begin()+aspl);
	//now aspl points to the next splitter
	if((i->first)->FracturesHolder())//if this element can be splitted add a splitter for it
	{
		new_shell_last_desc->m_spliter_holder->m_splitters.push_back(CPHShellSplitter(CPHShellSplitter::splElement,0,0));//
		//pass splitters taking into account that one was olready added
		PassEndSplitters(aspl,new_shell_last_desc,1,splitter.m_element,splitter.m_joint);
	}
	else
	{
		PassEndSplitters(aspl,new_shell_last_desc,0,splitter.m_element,splitter.m_joint);
	}

	//splitter.m_element the num of els leaved in old shell minus one new element added
	//start_joint the number of joints leaved in source shell (no destroied joints)
	out_shels.push_back(mk_pair(new_shell_last,i->second));
	i++;

		//create shells containing one element
		for(;i!=e;i++)
		{
			CPhysicsShell *new_shell=P_create_Shell();
			CPHShell	  *new_shell_desc=dynamic_cast<CPHShell*>(new_shell);
			InitNewShell(new_shell_desc);
			new_shell->add_Element(i->first);
			if((i->first)->FracturesHolder())//if this element can be splitted add a splitter for it
			{
				new_shell_desc->m_spliter_holder->m_splitters.push_back(CPHShellSplitter(CPHShellSplitter::splElement,0,0));//
			}
			out_shels.push_back(mk_pair(new_shell,i->second));
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
	m_type=type;
	m_element=element;
	m_joint=joint;
}