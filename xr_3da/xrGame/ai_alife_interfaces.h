////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life interfaces
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_ALIFE_INTERFACES
#define XRAY_AI_ALIFE_INTERFACES

class NET_Packet;

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

interface IPureServerObject : public IPureALifeLSObject {
public:
	virtual void					STATE_Write	(NET_Packet &tNetPacket)				= 0;
	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size)		= 0;
	virtual void					UPDATE_Write(NET_Packet &tNetPacket)				= 0;
	virtual void					UPDATE_Read	(NET_Packet &tNetPacket)				= 0;
	virtual void					Save		(IWriter	&tMemoryStream)
	{
		NET_Packet					l_tNetPacket;
		UPDATE_Write				(l_tNetPacket);
		tMemoryStream.w_u32			(l_tNetPacket.B.count);
		tMemoryStream.w				(l_tNetPacket.B.data,l_tNetPacket.B.count);
	}
	
	virtual void					Load		(IReader	&tFileStream)
	{
		NET_Packet					l_tNetPacket;
		l_tNetPacket.B.count		= tFileStream.r_u32();
		tFileStream.r				(l_tNetPacket.B.data,l_tNetPacket.B.count);
		UPDATE_Read					(l_tNetPacket);
	}
};
#endif