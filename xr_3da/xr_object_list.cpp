#include "stdafx.h"
#include "xr_object_list.h"
#include "xr_tokens.h"
#include "std_classes.h"
#include "xr_creator.h"
#include "xr_ini.h"

#include "xr_object.h"
#include "net_utils.h"

class fNameEQ {
	LPCSTR	name;
public:
	fNameEQ(LPCSTR N) : name(N) {};
	IC bool operator() (CObject* O) { return strcmp(O->cName(),name)==0; }
};
class fClassEQ {
	CLASS_ID cls;
public:
	fClassEQ(CLASS_ID C) : cls(C) {};
	IC bool operator() (CObject* O) { return cls==O->SUB_CLS_ID; }
};

CObjectList::CObjectList	( )
{
}

CObjectList::~CObjectList	( )
{
	R_ASSERT(objects.empty()		);
	R_ASSERT(destroy_queue.empty()	);
	R_ASSERT(map_NETID.empty()		);
	R_ASSERT(map_POOL.empty()		);
}

CObject*	CObjectList::FindObjectByName	( LPCSTR name )
{
	OBJ_IT O=find_if(objects.begin(),objects.end(),fNameEQ(name));
	if (O!=objects.end())	return *O;
	else					return NULL;
}

CObject*	CObjectList::FindObjectByCLS_ID	( CLASS_ID cls )
{
	OBJ_IT O=find_if(objects.begin(),objects.end(),fClassEQ(cls));
	if (O!=objects.end())	return *O;
	else					return NULL;
}

void CObjectList::SingleUpdate	(CObject* O)
{
	if (Device.dwFrame != O->dwFrame_UpdateCL)
	{
		if (O->H_Parent())		SingleUpdate(O->H_Parent());
		O->dwFrame_UpdateCL		= Device.dwFrame;
		O->UpdateCL				();
		if (O->getDestroy() && !O->shedule_Locked)				destroy_queue.push_back(O);
		if (O->H_Parent() && O->H_Parent()->getDestroy() && !O->shedule_Locked)	
		{
			// Push to destroy-queue if it isn't here already
			if (find(destroy_queue.begin(),destroy_queue.end(),O)==destroy_queue.end())
				destroy_queue.push_back	(O);
		}
	}
}

void CObjectList::Update		()
{
	// Clients
	Device.Statistic.UpdateClient.Begin		();
	for (OBJ_IT O=objects.begin(); O!=objects.end(); O++) 
		SingleUpdate(*O);
	Device.Statistic.UpdateClient.End		();

	// Destroy
	if (!destroy_queue.empty()) 
	{
		// Info
		for (OBJ_IT oit=objects.begin(); oit!=objects.end(); oit++)
		{
			for (int it = destroy_queue.size()-1; it>=0; it--)
				(*oit)->net_Relcase	(destroy_queue[it]);
		}

		// Destroy
		for (int it = destroy_queue.size()-1; it>=0; it--)
		{
			CObject*		O	= destroy_queue[it];
			O->net_Destroy	();
			Destroy			(O);
		}
		destroy_queue.clear	();
	}
}

void CObjectList::net_Register	(CObject* O)
{
	R_ASSERT		(O);
	map_NETID.insert(make_pair(O->ID(),O));
	Msg				("-------------------------------- Register: %s",O->cName());
}

void CObjectList::net_Unregister(CObject* O)
{
	Msg				("-------------------------------- Unregster: %s",O->cName());
	map<u32,CObject*>::iterator	it = map_NETID.find(O->ID());
	if ((it!=map_NETID.end()) && (it->second == O))	map_NETID.erase(it);
}

void CObjectList::net_Export	(NET_Packet* _Packet)
{
	NET_Packet& Packet	= *_Packet;
	u32			position;
	for (OBJ_IT O=objects.begin(); O!=objects.end(); O++) 
	{
		CObject* P = *O;
		if (P->net_Relevant() && !P->getDestroy())	
		{
			Packet.w_u16			(u16(P->ID())	);
			Packet.w_chunk_open8	(position);
			P->net_Export			(Packet);
			Packet.w_chunk_close8	(position);
		}
	}
}

void CObjectList::net_Import		(NET_Packet* Packet)
{
	while (!Packet->r_eof())
	{
		u16 ID;		Packet->r_u16	(ID);
		u8  size;	Packet->r_u8	(size);
		CObject* P  = net_Find		(u32(ID));
		if (P)	P->net_Import		(*Packet);
		else	Packet->r_advance	(size);
	}
}

CObject* CObjectList::net_Find			(u32 ID)
{
	map<u32,CObject*>::iterator	it = map_NETID.find(ID);
	return (it==map_NETID.end())?0:it->second;
}

void	CObjectList::SLS_Save			(IWriter&	fs		)
{
	for (u32 i=0; i<objects.size(); i++) 
	{
		CObject *O		= objects[i];
		fs.open_chunk	(i);
		fs.w_u16		(u16(O->ID()));
		O->SLS_Save		(fs);
		fs.close_chunk	();
	}
}

void	CObjectList::SLS_Load			(IReader&	fs		)
{
	u32 ID	= 0; 
	while (fs.find_chunk(ID)) 
	{
		u16 net_ID		= fs.r_u16	();
		CObject* O		= net_Find	(net_ID);
		if (0==O)		Msg("! SLS_Load: Invalid ID found. (Object not spawned up to this moment?)");
		else			O->SLS_Load(fs);
		ID ++;
	}
}
