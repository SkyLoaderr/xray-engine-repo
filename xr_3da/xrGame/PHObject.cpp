#include "stdafx.h"
#include "Physics.h"
#include "PHObject.h"

	CPHObject::CPHObject()
{
	b_activated=false;
}
void CPHObject::Activate()
{
	if(b_activated)return;
	m_ident=ph_world->AddObject(this);
	b_activated=true;
}

void CPHObject::Deactivate()
{
	if(!b_activated)return;
	ph_world->RemoveObject(m_ident);
	b_activated=false;
}