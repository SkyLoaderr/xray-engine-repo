////////////////////////////////////////////////////////////////////////////
//	Module 		: object_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife interfaces
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef XRAY_COMPATIBLE
	class NET_Packet;
#endif

class IPureDestroyableObject {
public:
	virtual void					destroy()											= 0;
};

template <typename _storage_type>
class IPureLîadableObject {
public:
	virtual void					load(_storage_type	&storage)						= 0;
};

template <typename _storage_type>
class IPureSavableObject {
public:
	virtual void					save(_storage_type	&storage)						= 0;
};

template <typename _storage_type_load, typename _storage_type_save>
class IPureSerializeObject : public IPureLîadableObject<_storage_type_load>, public IPureSavableObject<_storage_type_save> {
public:
};

#ifdef XRAY_COMPATIBLE
class IPureServerObject : public IPureSerializeObject<IReader,IWriter> {
public:
	virtual void					STATE_Write	(NET_Packet &tNetPacket)				= 0;
	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size)		= 0;
	virtual void					UPDATE_Write(NET_Packet &tNetPacket)				= 0;
	virtual void					UPDATE_Read	(NET_Packet &tNetPacket)				= 0;
};
#endif

class IPureSchedulableObject {
public:
	virtual void					update		()										= 0;
};
