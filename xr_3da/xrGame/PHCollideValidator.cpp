#include "stdafx.h"
#include "PHObject.h"
#include "phcollidevalidator.h"

CGID CPHCollideValidator::freeGroupID=0;
_flags<CLClassBits> CPHCollideValidator::onceFlags={CLClassBits(0)};	
_flags<CLClassBits> CPHCollideValidator::typeFlags={CLClassBits(0)};	
void CPHCollideValidator::Init()
{
	freeGroupID=0;
	typeFlags.set(cbNCStatic,TRUE);
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
bool CPHCollideValidator::IsGroupObject(const CPHObject& obj)
{
	return !!obj.collide_class_bits().test(cbNCGroupObject);
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

void CPHCollideValidator::SetStaticNotCollide(CPHObject& obj)
{
	obj.collide_class_bits().set(cbNone,FALSE);
	obj.collide_class_bits().set(cbNCStatic,TRUE);
}