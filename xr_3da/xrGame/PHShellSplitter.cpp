#include "stdafx.h"
#include "Physics.h"
#include "PHShell.h"
#include "PHShellSplitter.h"
#include "PHFracture.h"
#include "PHJointDestroyInfo.h"
#include "Geometry.h"
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
	m_geom_root_map.clear();
}
//the simpliest case - a joint to be destroied 
shell_root CPHShellSplitterHolder::SplitJoint(u16 aspl)
{
	//create _new physics shell 

	CPhysicsShell *new_shell=P_create_Shell();
	CPHShell	  *new_shell_desc=dynamic_cast<CPHShell*>(new_shell);
	new_shell_desc->mXFORM.set(m_pShell->mXFORM);
	SPLITTER_I splitter=m_splitters.begin()+aspl;
	u16 start_element=splitter->m_element;
	u16 start_joint=splitter->m_joint;

	u16 end_element=m_pShell->joints[start_joint]->JointDestroyInfo()->m_end_element;
	u16 end_joint=m_pShell->joints[start_joint]->JointDestroyInfo()->m_end_joint;


	shell_root ret = mk_pair(new_shell,(m_pShell->joints[start_joint])->JointDestroyInfo()->BoneID());



	CShellSplitInfo split_inf;
	split_inf.m_bone_id=m_pShell->joints[start_joint]->JointDestroyInfo()->BoneID();
	split_inf.m_start_el_num=start_element;
	split_inf.m_end_el_num=end_element;
	split_inf.m_start_jt_num=start_joint;
	split_inf.m_end_jt_num=end_joint;
	
	m_splitters.erase(splitter);
	PassEndSplitters(split_inf,new_shell_desc,1,0);
	new_shell_desc->PresetActive();
	m_pShell->PassEndElements(start_element,end_element,new_shell_desc);
	m_pShell->PassEndJoints(start_joint+1,end_joint,new_shell_desc);
	new_shell_desc->PureActivate();
	//new_shell_desc->ObjectInRoot().identity();
	m_pShell->DeleteJoint(start_joint);


	return ret;
}

void CPHShellSplitterHolder::PassEndSplitters(const CShellSplitInfo& spl_inf,CPHShell* dest,u16 jt_add_shift,u16 el_add_shift)
{


	CPHShellSplitterHolder*	&dest_holder=dest->m_spliter_holder;
	if(!dest_holder)dest_holder=xr_new<CPHShellSplitterHolder>(dest);

	ELEMENT_STORAGE &source_elements=m_pShell->elements;
	ELEMENT_STORAGE &dest_elements=dest->elements;
	ELEMENT_I i_elem=source_elements.begin(),e_elem=source_elements.begin()+spl_inf.m_start_el_num;
	u16 shift_e=spl_inf.m_end_el_num-spl_inf.m_start_el_num;
	u16 shift_j=spl_inf.m_end_jt_num-spl_inf.m_start_jt_num;

	for(;i_elem!=e_elem;i_elem++)	//until start elem in both joint or elem split fractures 
		//end elems have to be corrected 
		//if grater then end elem in moving diapason
	{
		CPHFracturesHolder	*fracturesHolder=(*i_elem)->FracturesHolder();
		if(!fracturesHolder) continue;
		FRACTURE_I f_i=fracturesHolder->m_fractures.begin(),f_e=fracturesHolder->m_fractures.end();
		for(;f_i!=f_e;f_i++)
		{
		
				u16	&end_el_num=f_i->m_end_el_num;
				u16 &start_el_num=f_i->m_start_el_num;
				if(end_el_num>=spl_inf.m_end_el_num)		end_el_num=end_el_num-shift_e;
				if(start_el_num>=spl_inf.m_end_el_num)		start_el_num=start_el_num-shift_e;
				
				u16	&end_jt_num=f_i->m_end_jt_num;
				u16 &start_jt_num=f_i->m_start_jt_num;
				if(end_jt_num>=spl_inf.m_end_jt_num)		end_jt_num=end_jt_num-shift_j;
				if(start_jt_num>=spl_inf.m_end_jt_num)		start_jt_num=start_jt_num-shift_j;
		}
	}

	//same for joints
	JOINT_STORAGE	&source_joints=m_pShell->joints;
	JOINT_I i_joint=source_joints.begin(),e_joint;
	if(spl_inf.m_start_jt_num!=u16(-1))
	{
		e_joint=source_joints.begin()+spl_inf.m_start_jt_num;
		for(;i_joint!=e_joint;i_joint++)	
		{
			CPHJointDestroyInfo* jointDestroyInfo=(*i_joint)->JointDestroyInfo();
			if(!jointDestroyInfo) continue;
			u16 &end_element	=		jointDestroyInfo->m_end_element;
			if(end_element>=spl_inf.m_end_el_num) end_element=end_element-shift_e;
			u16	&end_joint		=		jointDestroyInfo->m_end_joint;
			if(end_joint>=spl_inf.m_end_jt_num)		end_joint=end_joint-shift_j;
		}
	}


	//now process diapason that tobe unsplited

	e_elem=source_elements.begin()+spl_inf.m_end_el_num;
	u16 passed_shift_e=spl_inf.m_start_el_num-u16(dest_elements.size());
	u16 passed_shift_j = u16(-1) & (spl_inf.m_start_jt_num + jt_add_shift);
	for(;i_elem!=e_elem;i_elem++)	

	{
		CPHFracturesHolder	*fracturesHolder=(*i_elem)->FracturesHolder();
		if(!fracturesHolder) continue;
		FRACTURE_I f_i=fracturesHolder->m_fractures.begin(),f_e=fracturesHolder->m_fractures.end();
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
//////correct data in fractures for elements allready added to dest with fractures from source///////
ELEMENT_I i_dest_elem=dest_elements.begin(),e_dest_elem=dest_elements.end();
for(;i_dest_elem!=e_dest_elem;++i_dest_elem)
{
	CPHFracturesHolder	*fracturesHolder=(*i_dest_elem)->FracturesHolder();
	if(!fracturesHolder) continue;
	FRACTURE_I f_i=fracturesHolder->m_fractures.begin(),f_e=fracturesHolder->m_fractures.end();
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
/////////////////////////////////////////////////////////////////////////////////////////////////////

if(spl_inf.m_end_jt_num!=u16(-1))
{
	e_joint=source_joints.begin()+spl_inf.m_end_jt_num;
	for(;i_joint!=e_joint;i_joint++)	
	{
		CPHJointDestroyInfo* jointDestroyInfo=(*i_joint)->JointDestroyInfo();
		if(!jointDestroyInfo) continue;
		u16 &end_element	=		jointDestroyInfo->m_end_element;
		u16	&end_joint		=		jointDestroyInfo->m_end_joint;
		end_element=end_element-passed_shift_e;
		end_joint=end_joint-passed_shift_j;
	}
}
	//the rest unconditionaly shift end & begin
	e_elem=source_elements.end();
	for(;i_elem!=e_elem;i_elem++)	

	{
		CPHFracturesHolder	*fracturesHolder=(*i_elem)->FracturesHolder();
		if(!fracturesHolder) continue;
		FRACTURE_I f_i=fracturesHolder->m_fractures.begin(),f_e=fracturesHolder->m_fractures.end();
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
		CPHJointDestroyInfo* jointDestroyInfo=(*i_joint)->JointDestroyInfo();
		if(!jointDestroyInfo) continue;
		u16 &end_element	=		jointDestroyInfo->m_end_element;
		u16	&end_joint		=		jointDestroyInfo->m_end_joint;
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
			if( elem!=u16(-1) && elem>=spl_inf.m_start_el_num) break;//we at begining
		}
		else
		{
			if( joint!=u16(-1) && joint>=spl_inf.m_start_jt_num) break;//we at begining
		}
		if(elem!=u16(-1) && elem>spl_inf.m_end_el_num) elem=elem-shift_e;
		if(joint!=u16(-1) && joint>spl_inf.m_end_jt_num) joint=joint-shift_j;
	}
	SPLITTER_I i_from = spl_i;
	//correct data for passing splitters and find last splitter to pass
	for(;spl_i!=spl_e;spl_i++)
	{
		u16	&elem	= spl_i->m_element;
		u16 &joint  = spl_i->m_joint;
		if(spl_i->m_type==CPHShellSplitter::splElement) 
		{
			if(elem!=u16(-1) && elem>=spl_inf.m_end_el_num) break;//we after begining
		}
		else
		{
			if(joint!=u16(-1) && joint>=spl_inf.m_end_jt_num) break;//we after begining
		}
		if(elem!=u16(-1) ) elem=elem-passed_shift_e;
		if(joint!=u16(-1)) joint=joint-passed_shift_j;
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
	const u16& start_joint=split_elem.second.m_start_jt_num;
	const u16& end_joint=split_elem.second.m_end_jt_num;
	//it is not right for multiple joints attached to the unsplited part becource all these need to be reattached
	if(start_joint!=end_joint)
		m_pShell->joints[split_elem.second.m_start_jt_num]->ReattachFirstElement(split_elem.first);

	//the last new shell will have all splitted old elements end joints and one new element reattached to old joint
	//m_splitters.erase(m_splitters.begin()+aspl);
	//now aspl points to the next splitter
	if((split_elem.first)->FracturesHolder())//if this element can be splitted add a splitter for it
		new_shell_last_desc->AddSplitter(CPHShellSplitter::splElement,0,0);//
	
	new_shell_last_desc->add_Element(split_elem.first);
	//pass splitters taking into account that one element was olready added
	PassEndSplitters(split_elem.second,new_shell_last_desc,1,0);


	new_shell_last_desc->PresetActive();
	m_pShell->PassEndElements(split_elem.second.m_start_el_num,split_elem.second.m_end_el_num,new_shell_last_desc);

	//InitNewShell(new_shell_last_desc);//this cretes space for the shell and add elements to it,place elements to attach joints.....

	m_pShell->PassEndJoints(split_elem.second.m_end_jt_num,split_elem.second.m_end_jt_num,new_shell_last_desc);



	new_shell_last_desc->AfterSetActive();
	R_ASSERT2(split_elem.second.m_bone_id<64,"strange root");
	return mk_pair(new_shell_last,split_elem.second.m_bone_id);

}
void CPHShellSplitterHolder::SplitElement(u16 aspl,PHSHELL_PAIR_VECTOR &out_shels)
{
 
	new_elements.clear();
	SPLITTER_I spl_i=(m_splitters.begin()+aspl);
	CPHElement* E= m_pShell->elements[spl_i->m_element];
	E->SplitProcess(new_elements);


	ELEMENT_PAIR_I i=new_elements.begin(),e=new_elements.end();

	for(;i!=e;i++)
	{
		out_shels.push_back(ElementSingleSplit(*i));
	}

	if(!E->FracturesHolder()) m_splitters.erase(spl_i);//delete splitter if the element no longer have fractures
	else					  spl_i->m_breaked=false;//it is no longer breaked

}

void CPHShellSplitterHolder::SplitProcess(PHSHELL_PAIR_VECTOR &out_shels)
{
	//any split process must start from the end of the elment storage
	//this based on that all childs in the bone hierarchy was added after their parrent

	u16 i=u16(m_splitters.size()-1);
	for(;i!=u16(-1);i--)
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
	m_has_breaks=false;
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
				dBodyID body=element->get_body();
				if(!dBodyIsEnabled(body)) return;
				m_has_breaks=(element->FracturesHolder()->PhDataUpdate(element))||m_has_breaks;
				break;
			}
		case CPHShellSplitter::splJoint:
			{
				m_has_breaks=m_pShell->joints[i->m_joint]->JointDestroyInfo()->Update()||m_has_breaks;
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

void CPHShellSplitterHolder::AddToGeomMap(const id_geom& id_rootgeom)
{
	m_geom_root_map.insert(id_rootgeom);
}

u16 CPHShellSplitterHolder::FindRootGeom(u16 bone_id)
{
	GEOM_MAP_I iter=m_geom_root_map.find(bone_id);
	if(iter==m_geom_root_map.end()) return u16(-1);

	return iter->second->element_position();

}

CPHShellSplitter::CPHShellSplitter()
{
	m_breaked=false;
}

