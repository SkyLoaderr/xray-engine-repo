#include "stdafx.h"
#include "PHObject.h"
#include "phcollidevalidator.h"

CGID CPHCollideValidator::freeGroupID=0;
_flags<CLClassBits> CPHCollideValidator::onceFlags={CLClassBits(0)};	
_flags<CLClassBits> CPHCollideValidator::typeFlags={CLClassBits(-1)};	
void CPHCollideValidator::Init()
{
	freeGroupID=0;
}
CGID CPHCollideValidator::RegisterGroup()
{
	++freeGroupID;
	return freeGroupID-1;
}

void CPHCollideValidator::InitObject(CPHObject& obj)
{
	obj.collide_class_bits().assign(0);
	obj.collide_class_bits().set(cbNone,TRUE);
	obj.collide_bits()=0;
}
void CPHCollideValidator::RegisterObjToGroup(CGID group,CPHObject& obj)
{
	R_ASSERT(group<freeGroupID);
	obj.collide_bits()=group;
	obj.collide_class_bits().set(cbNCGroupObject,TRUE);
	obj.collide_class_bits().set(cbNone,FALSE);
}

void CPHCollideValidator::RegisterObjToLastGroup(CPHObject& obj)
{
	RegisterObjToGroup(LastGroupRegistred(),obj);
}

CGID CPHCollideValidator::LastGroupRegistred()
{
	return freeGroupID-1;
}

void CPHCollideValidator::RestoreGroupObject(const CPHObject& obj)
{
}

