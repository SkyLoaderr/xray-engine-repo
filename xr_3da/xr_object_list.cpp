#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "xr_object_list.h"
#include "std_classes.h"

#include "xr_object.h"
#include "net_utils.h"

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
}

CObject*	CObjectList::FindObjectByName	( ref_str name )
{
	for (xr_vector<CObject*>::iterator I=objects.begin(); I!=objects.end(); I++)
		if ((*I)->cName().equal(name))	return (*I);
	return	NULL;
}
CObject*	CObjectList::FindObjectByName	( LPCSTR name )
{
	return	FindObjectByName(ref_str(name));
}

CObject*	CObjectList::FindObjectByCLS_ID	( CLASS_ID cls )
{
	xr_vector<CObject*>::iterator O	= std::find_if(objects.begin(),objects.end(),fClassEQ(cls));
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
		VERIFY3					(O->dbg_update_cl == Device.dwFrame, "Broken sequence of calls to 'UpdateCL'",*O->cName());
		if (O->getDestroy() && !O->shedule.b_locked)	destroy_queue.push_back(O);
		else if (O->H_Parent() && (O->H_Parent()->getDestroy() || O->H_Root()->getDestroy()) && !O->shedule.b_locked)	{
			// Push to destroy-queue if it isn't here already
			Msg	("! ERROR: incorrect destroy sequence for object[%d:%s], section[%s]",O->ID(),*O->cName(),*O->cNameSect());
			if (std::find(destroy_queue.begin(),destroy_queue.end(),O)==destroy_queue.end())
				destroy_queue.push_back	(O);
		}
	}
}

void CObjectList::Update		()
{
	// Clients
	Device.Statistic.UpdateClient.Begin		();
	for (xr_vector<CObject*>::iterator O=objects.begin(); O!=objects.end(); O++) 
		SingleUpdate(*O);
	Device.Statistic.UpdateClient.End		();

	// Destroy
	if (!destroy_queue.empty()) 
	{
		// Info
		for (xr_vector<CObject*>::iterator oit=objects.begin(); oit!=objects.end(); oit++)
		{
			for (int it = destroy_queue.size()-1; it>=0; it--)
				(*oit)->net_Relcase	(destroy_queue[it]);
		}

		// Destroy
		for (int it = destroy_queue.size()-1; it>=0; it--)
		{
			CObject*		O	= destroy_queue[it];
			O->net_Destroy	( );
			Destroy			(O);
		}
		destroy_queue.clear	();
	}
}

void CObjectList::net_Register	(CObject* O)
{
	R_ASSERT		(O);
	map_NETID.insert(mk_pair(O->ID(),O));
	//Msg			("-------------------------------- Register: %s",O->cName());
}

void CObjectList::net_Unregister(CObject* O)
{
	xr_map<u32,CObject*>::iterator	it = map_NETID.find(O->ID());
	if ((it!=map_NETID.end()) && (it->second == O))	{
		// Msg			("-------------------------------- Unregster: %s",O->cName());
		map_NETID.erase(it);
	}
}

void CObjectList::net_Export	(NET_Packet* _Packet)
{
	NET_Packet& Packet	= *_Packet;
	u32			position;
	for (xr_vector<CObject*>::iterator O=objects.begin(); O!=objects.end(); O++) 
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
		if (P)		P->net_Import	(*Packet);
		else		Packet->r_advance(size);
	}
}

CObject* CObjectList::net_Find			(u32 ID)
{
	xr_map<u32,CObject*>::iterator	it = map_NETID.find(ID);
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

void CObjectList::Load		()
{
	R_ASSERT			(map_NETID.empty() && objects.empty() && destroy_queue.empty());
}

void CObjectList::Unload	( )
{
	// Destroy objects
	while (objects.size())
	{
		CObject*	O	= objects.back();
		O->net_Destroy	(   );
		Destroy			( O );
	}
	objects.clear();
}

CObject*	CObjectList::Create				( LPCSTR	name	)
{
	CObject*	O			= g_pGamePersistent->ObjectPool.create(name);
	objects.push_back		(O);
	return					O;
}

void		CObjectList::Destroy			( CObject*	O		)
{
	if (0==O)								return;
	net_Unregister							(O);
	xr_vector<CObject*>::iterator  it		=	std::find	(objects.begin(),objects.end(),O);
	if (it!=objects.end())	objects.erase	(it);
	g_pGamePersistent->ObjectPool.destroy	(O);
}
