////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_Abstract.cpp
//	Created 	: 19.09.2002
//  Modified 	: 14.07.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "net_utils.h"
#include "xrServer_Objects_Abstract.h"
#include "xrMessages.h"

////////////////////////////////////////////////////////////////////////////
// CSE_Shape
////////////////////////////////////////////////////////////////////////////
CSE_Shape::CSE_Shape						()
{
}

CSE_Shape::~CSE_Shape						()
{
}

void CSE_Shape::cform_read					(NET_Packet	&tNetPacket)
{
	shapes.clear				();
	u8							count;
	tNetPacket.r_u8				(count);
	
	while (count) {
		shape_def				S;
		tNetPacket.r_u8			(S.type);
		switch (S.type) {
			case 0 :	
				tNetPacket.r	(&S.data.sphere,sizeof(S.data.sphere));	
				break;
			case 1 :	
				tNetPacket.r_matrix(S.data.box);
				break;
		}
		shapes.push_back		(S);
		--count;
	}
}

void CSE_Shape::cform_write					(NET_Packet	&tNetPacket)
{
	tNetPacket.w_u8				(u8(shapes.size()));
	for (u32 i=0; i<shapes.size(); ++i) {
		shape_def				&S = shapes[i];
		tNetPacket.w_u8			(S.type);
		switch (S.type) {
			case 0:	
				tNetPacket.w	(&S.data.sphere,sizeof(S.data.sphere));
				break;
			case 1:	
				tNetPacket.w_matrix	(S.data.box);
				break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
// CSE_Visual
////////////////////////////////////////////////////////////////////////////
CSE_Visual::CSE_Visual		   	(LPCSTR name)
{
	visual_name					= name;
    startup_animation			= "$editor";
}

CSE_Visual::~CSE_Visual			()
{
}

void CSE_Visual::set_visual	   	(LPCSTR name, bool load)
{
	string_path					tmp;
    strcpy						(tmp,name);
    if (strext(tmp))		 	*strext(tmp) = 0;
	visual_name					= tmp; 
}

void CSE_Visual::visual_read   	(NET_Packet	&tNetPacket)
{
	tNetPacket.r_stringZ		(visual_name);
}

void CSE_Visual::visual_write  	(NET_Packet	&tNetPacket)
{
	tNetPacket.w_stringZ		(visual_name);
}

void CSE_Visual::OnChangeVisual	(PropValue* sender)
{
	ISE_Abstract* abstract		= dynamic_cast<ISE_Abstract*>(this); VERIFY(abstract);
	abstract->set_editor_flag	(ISE_Abstract::flVisualChange);
}

void CSE_Visual::FillProp		(LPCSTR pref, PropItemVec &items)
{
	ISE_Abstract* abstract		= dynamic_cast<ISE_Abstract*>(this); VERIFY(abstract);
	ChooseValue *V 				= PHelper().CreateChoose(items, PrepareKey(pref,abstract->name(),"Model\\Visual"),	&visual_name,	smVisual);
	V->OnChangeEvent.bind		(this,&CSE_Visual::OnChangeVisual);
}

////////////////////////////////////////////////////////////////////////////
// CSE_Animated
////////////////////////////////////////////////////////////////////////////
CSE_Motion::CSE_Motion			(LPCSTR name)
{
	motion_name					= name;
}

CSE_Motion::~CSE_Motion			()
{
}

void CSE_Motion::set_motion		(LPCSTR name)
{
	motion_name					= name;
}

void CSE_Motion::motion_read	(NET_Packet	&tNetPacket)
{
	tNetPacket.r_stringZ		(motion_name);
}

void CSE_Motion::motion_write	(NET_Packet	&tNetPacket)
{
	tNetPacket.w_stringZ			(motion_name);
}

void CSE_Motion::OnChangeMotion	(PropValue* sender)
{
	ISE_Abstract* abstract		= dynamic_cast<ISE_Abstract*>(this); VERIFY(abstract);
	abstract->set_editor_flag	(ISE_Abstract::flMotionChange);
}

void CSE_Motion::FillProp(		LPCSTR pref, PropItemVec &items)
{
	ISE_Abstract* abstract		= dynamic_cast<ISE_Abstract*>(this); VERIFY(abstract);
	ChooseValue *V				= PHelper().CreateChoose(items, PrepareKey(pref,abstract->name(),"Motion"),&motion_name, smGameAnim);
	V->OnChangeEvent.bind		(this,&CSE_Motion::OnChangeMotion);
}
