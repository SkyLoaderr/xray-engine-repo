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
	virtual void					Load(IReader	&tFileStream)	= 0;
};

interface IPureALifeSObject {
public:
	virtual void					Save(IWriter	&tMemoryStream) = 0;
};

interface IPureALifeLSObject : public IPureALifeLObject, public IPureALifeSObject {
};

interface IPureServerObject {
public:
	virtual void					STATE_Write	(NET_Packet &tNetPacket)				= 0;
	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size)		= 0;
	virtual void					UPDATE_Write(NET_Packet &tNetPacket)				= 0;
	virtual void					UPDATE_Read	(NET_Packet &tNetPacket)				= 0;
};
#endif