#include "stdafx.h"
#include "PHFracture.h"
#include "Physics.h"
#include "PHElement.h"
CPHFracturesHolder::CPHFracturesHolder()
{
	m_has_breaks=false;
}
CPHElement* CPHFracturesHolder::SplitFromEnd(CPHElement* element,u16 fracture)
{
	FRACTURE_I fract_i=m_fractures.begin()+fracture;
	u16 geom_num=fract_i->m_geom_num;
	CPHElement* new_element=dynamic_cast<CPHElement*>(P_create_Element());
	element->PassEndGeoms(geom_num,new_element);
	InitNewElement(new_element);
	m_fractures.erase(fract_i);
	//fracture now points to next fracture
	//if we have another fractures after this one pass them to the created element starting from fracture(next to argument fracture)
	//shift geoms in fractures by number of geoms lived in source element
	if(m_fractures.size()-fracture>0) 
	{	
		if(new_element->m_fratures_holder==NULL)//create fractures holder if it was not created before
		{
			new_element->m_fratures_holder=xr_new<CPHFracturesHolder>();
		}
		PassEndFractures(fracture,new_element->m_fratures_holder,geom_num+1);
	}
	return new_element;
}

void CPHFracturesHolder::PassEndFractures(u16 from,CPHFracturesHolder* dest,u16 shift_geoms)
{
	FRACTURE_I i_from=m_fractures.begin()+from,e=m_fractures.end();

	for(FRACTURE_I i=i_from;i!=e;i++)
	{
		i->m_geom_num=i->m_geom_num-shift_geoms;
	}


	dest->m_fractures.insert(dest->m_fractures.begin(),i_from,e);
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

}

bool CPHFracture::Update(PH_IMPACT_STORAGE& impacts,dBodyID body)
{
return false;
}