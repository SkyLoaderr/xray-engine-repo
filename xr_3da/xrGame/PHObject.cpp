#include "stdafx.h"
#include "Physics.h"
#include "PHObject.h"

void CPHObject::Activate()
{
	m_ident=ph_world->AddObject(this);
}

void CPHObject::Deactivate()
{
	ph_world->RemoveObject(m_ident);
}