#include "stdafx.h"
#include "holder_custom.h"

bool CHolderCustom::attach_Actor(CActor* actor)
{
	m_owner	= actor;
	return true;
}

void CHolderCustom::detach_Actor()
{
	m_owner	= 0;
}
