////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife interfaces
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ALIFE_INTERFACES
#define XRAY_ALIFE_INTERFACES

class NET_Packet;

interface IPureALifeLObject {
public:
	IC virtual						~IPureALifeLObject()								= 0;
	virtual void					load(IReader	&tFileStream)						= 0;
};

interface IPureALifeSObject {
public:
	IC virtual						~IPureALifeSObject()								= 0;
	virtual void					save(IWriter	&tMemoryStream)						= 0;
};

interface IPureALifeLSObject : public IPureALifeLObject, public IPureALifeSObject {
};

interface IPureServerObject : public IPureALifeLSObject {
public:
	virtual void					STATE_Write	(NET_Packet &tNetPacket)				= 0;
	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size)		= 0;
	virtual void					UPDATE_Write(NET_Packet &tNetPacket)				= 0;
	virtual void					UPDATE_Read	(NET_Packet &tNetPacket)				= 0;
};

interface IPureSchedulableObject {
	IC virtual						~IPureSchedulableObject()							= 0;
	virtual void					update		()										= 0;
};

IC IPureALifeLObject::~IPureALifeLObject()
{
}

IC IPureALifeSObject::~IPureALifeSObject()
{
}

IC IPureSchedulableObject::~IPureSchedulableObject()
{
}

#endif