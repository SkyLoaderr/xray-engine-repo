#pragma once

#include "..\\xr_object.h"

#define DEFINE_EMPTY_FUNCTION(a,b) a b(){return a(0);}

class CGameObject : public CObject {
};

class CEntityCondition {
};

class CInventoryItem : virtual public CGameObject {
public:
	DEFINE_EMPTY_FUNCTION(int,Cost);
	DEFINE_EMPTY_FUNCTION(int,GetCondition);
};

class CEntity : virtual public CGameObject, public CEntityCondition {
public:
	u32 m_dwDeathTime;
	DEFINE_EMPTY_FUNCTION(int,g_Armor);
	DEFINE_EMPTY_FUNCTION(int,g_MaxHealth);
	DEFINE_EMPTY_FUNCTION(int,g_Accuracy);
	DEFINE_EMPTY_FUNCTION(int,g_Alive);
	DEFINE_EMPTY_FUNCTION(int,g_Team);
	DEFINE_EMPTY_FUNCTION(int,g_Squad);
	DEFINE_EMPTY_FUNCTION(int,g_Group);
	DEFINE_EMPTY_FUNCTION(int,KillEntity);
};

class CEntityAlive : public CEntity {
};

class CCustomMonster : public CEntityAlive {
};

class CAI_Stalker : public CCustomMonster {
};

class CScriptMonster : virtual public CGameObject {
};

class CScriptZone {
};

class CParticlesObject {
};

class CCameraManager {
};