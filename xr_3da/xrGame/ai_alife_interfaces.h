////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life interfaces
////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace ALife;

class IPureALifeLObject {
public:
	virtual void					Load(CStream	&tFileStream)	= 0;
};

class IPureALifeSObject {
public:
	virtual void					Save(CFS_Memory	&tMemoryStream) = 0;
};

class IPureALifeIObject {
public:
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints) = 0;
};

class IPureALifeLSObject : public IPureALifeLObject, public IPureALifeSObject {
};

class IPureALifeLSIObject : public IPureALifeLSObject, public IPureALifeIObject {
};