////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life interfaces
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_ALIFE_INTERFACES
#define XRAY_AI_ALIFE_INTERFACES

#include "ai_alife_space.h"
#include "net_utils.h"
using namespace ALife;

interface IPureALifeLObject {
public:
	virtual void					Load(CStream	&tFileStream)	= 0;
};

interface IPureALifeSObject {
public:
	virtual void					Save(CFS_Memory	&tMemoryStream) = 0;
};

interface IPureALifeIObject {
public:
	virtual void					Init(_SPAWN_ID	tSpawnID, SPAWN_P_VECTOR &tpSpawnPoints) = 0;
};

class IPureALifeLSObject : public IPureALifeLObject, public IPureALifeSObject {
};

interface IPureALifeLSIObject : public IPureALifeLSObject, public IPureALifeIObject {
};

interface IPureServerObject {
public:
	virtual void					STATE_Write	(NET_Packet &tNetPacket)				= 0;
	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size)		= 0;
	virtual void					UPDATE_Write(NET_Packet &tNetPacket)				= 0;
	virtual void					UPDATE_Read	(NET_Packet &tNetPacket)				= 0;
};

interface IPureServerInitObject : public IPureServerObject {
public:
	virtual void					Init		(LPCSTR caSection)						= 0;
};
#endif