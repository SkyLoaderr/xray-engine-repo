#include "stdafx.h"
#include "telekinetic_object.h"
#include "../PhysicsShell.h"
#include "../PHInterpolation.h"
#include "../PHElement.h"

#define KEEP_IMPULSE_UPDATE 200

CTelekineticObject::CTelekineticObject()
{
		state	= TS_None;
		object	= 0;
}

CTelekineticObject::~CTelekineticObject()
{

}



bool CTelekineticObject::init(CGameObject *obj, float s, float h, u32 ttk) 
{
	if (!obj || !obj->m_pPhysicsShell) return false;
	
	state				= TS_Raise;
	object				= obj;
	
	target_height		= obj->Position().y + h;

	time_keep_started	= 0;
	time_keep_updated	= 0;
	time_to_keep		= ttk;

	strength			= s;

	object->m_pPhysicsShell->set_ApplyByGravity(FALSE);

	return true;
}

void CTelekineticObject::raise(float power) 
{
	power *= strength;
	
	Fvector dir;
	dir.set(0.f,1.0f,0.f);

	float elem_size = float(object->m_pPhysicsShell->Elements().size());
	dir.mul(elem_size*elem_size);

	(object->m_pPhysicsShell->Elements()[0])->applyGravityAccel(dir);
}

void CTelekineticObject::prepare_keep()
{
	time_keep_started	= Level().timeServer();
	state				= TS_Keep;

	time_keep_updated	= 0;
}

bool CTelekineticObject::time_keep_elapsed()
{
	if (time_keep_started + time_to_keep < Level().timeServer()) return true;
	return false;
}

void CTelekineticObject::keep()
{
	// ��������� ����� ���������� ����������
	//if (time_keep_updated + KEEP_IMPULSE_UPDATE > Level().timeServer()) return;

	// ��������� ������
	float cur_h		= object->Position().y;

	// ���������� dir � ������������ � ������� �������
	Fvector dir;
	if (cur_h > target_height+ 0.6f)			dir.set(0.f,-1.0f,0.f);
	else if (cur_h < target_height+ 0.6f)		dir.set(0.f,1.0f,0.f);
	else {
		dir.set(Random.randF(-1.0f,1.0f), Random.randF(-1.0f,1.0f), Random.randF(-1.0f,1.0f));
		dir.normalize_safe();
	}

	//float elem_size = float(object->m_pPhysicsShell->Elements().size());
	dir.mul(5.0f);

	(object->m_pPhysicsShell->Elements()[0])->applyGravityAccel(dir);

	// ���������� ����� ���������� ����������
	time_keep_updated = Level().timeServer();
}

void CTelekineticObject::release() 
{
	Fvector dir_inv;
	dir_inv.set(0.f,-1.0f,0.f);

	// �������� ����������
	object->m_pPhysicsShell->set_ApplyByGravity(TRUE);
	// ��������� ��������� ���� ��� ����, ����� ������ ����� ������
	object->m_pPhysicsShell->applyImpulse(dir_inv, 0.5f * object->m_pPhysicsShell->getMass());
}

void CTelekineticObject::fire(const Fvector &target)
{
	// ��������� �����������
	Fvector dir;
	dir.sub(target,object->Position());
	dir.normalize();

	// �������� ����������
	object->m_pPhysicsShell->set_ApplyByGravity(FALSE);
	// ��������� ������
	object->m_pPhysicsShell->applyImpulse(dir, 10.f * object->m_pPhysicsShell->getMass());
}

bool CTelekineticObject::check_height()
{
	return (object->Position().y > target_height);
}

void CTelekineticObject::enable()
{
	object->m_pPhysicsShell->Enable();
}
