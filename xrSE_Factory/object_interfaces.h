////////////////////////////////////////////////////////////////////////////
//	Module 		: object_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife interfaces
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ALIFE_INTERFACES
#define XRAY_ALIFE_INTERFACES

class NET_Packet;

#ifdef XRSE_FACTORY_EXPORTS
#	define xr_interface __interface
#else
#	define xr_interface interface
#endif

xr_interface IPureDestroyableObject {
public:
	virtual void					destroy()											= 0;
};

xr_interface IPureLîadableObject {
public:
	virtual void					load(IReader	&tFileStream)						= 0;
};

xr_interface IPureSavableObject {
public:
	virtual void					save(IWriter	&tMemoryStream)						= 0;
};

xr_interface IPureSerializeObject : public IPureLîadableObject, public IPureSavableObject {
};

xr_interface IPureServerObject : public IPureSerializeObject {
public:
	virtual void					STATE_Write	(NET_Packet &tNetPacket)				= 0;
	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size)		= 0;
	virtual void					UPDATE_Write(NET_Packet &tNetPacket)				= 0;
	virtual void					UPDATE_Read	(NET_Packet &tNetPacket)				= 0;
};

xr_interface IPureSchedulableObject {
	virtual void					update		()										= 0;
};

#undef xr_interface

#endif