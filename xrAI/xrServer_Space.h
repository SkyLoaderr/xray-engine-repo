////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects space
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_SpaceH
#define xrServer_SpaceH

#ifdef _EDITOR
#define SERVER_OBJECT_EDITOR_METHODS	virtual void FillProp(LPCSTR pref, PropItemVec& values);
#else
#define SERVER_OBJECT_EDITOR_METHODS 
#endif

#define SERVER_OBJECT_DECLARE_BEGIN(__A,__B)	class __A : public __B	{ typedef __B inherited; public:
#define SERVER_OBJECT_DECLARE_BEGIN2(__A,__B,__C) class __A : public __B, public __C	{ typedef __B inherited1; typedef __C inherited2; public:
#define SERVER_OBJECT_DECLARE_BEGIN3(__A,__B,__C,__D) class __A : public __B, public __C, public __D	{ typedef __B inherited1; typedef __C inherited2; typedef __D inherited3; public:

#define	SERVER_OBJECT_DECLARE_END \
public:\
	virtual void 						UPDATE_Read		(NET_Packet& P); \
	virtual void 						UPDATE_Write	(NET_Packet& P); \
	virtual void 						STATE_Read		(NET_Packet& P, u16 size); \
	virtual void 						STATE_Write		(NET_Packet& P); \
	SERVER_OBJECT_EDITOR_METHODS \
};

struct	SRotation
{
	float  yaw, pitch;
	SRotation() { yaw=pitch=0; }
};

enum EPOType {
	epotBox,
	epotFixedChain,
    epotFreeChain,
    epotSkeleton
};

DEFINE_VECTOR	(u32,						DWORD_VECTOR,			DWORD_IT);
DEFINE_VECTOR	(bool,						BOOL_VECTOR,			BOOL_IT);

template <class T> void	F_entity_Destroy	(T*& P)
{
	xr_delete(P);
};

#endif