#include "stdafx.h"
#include "xr_object_list.h"
#include "xr_tokens.h"
#include "std_classes.h"
#include "xr_creator.h"
#include "xr_ini.h"

#include "xr_object.h"
#include "net_utils.h"

class fNameEQ {
	char*	name;
public:
	fNameEQ(char *N) : name(N) {};
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
	R_ASSERT(objects.size()==0);
}

void CObjectList::Unload( ){
	for (DWORD i=0; i<objects.size(); i++) {
		CObject *pObject = objects[i];
		_DELETE	( pObject );
	}
	objects.clear();
}

CObject* CObjectList::LoadOne( CInifile *ini, const char* Name )
{
	CLASS_ID CLS		= ini->ReadCLSID(Name,"class");
	CObject* pObject	= (CObject*)NEW_INSTANCE(CLS);
	pObject->Load		(ini,Name);
	objects.push_back	(pObject);
/*
	if (pObject->dwMinUpdate)	sheduled.Register	(pObject);
	else						continues.push_back	(pObject);
*/
	return pObject;
}

VOID CObjectList::Load	( CInifile *ini, LPSTR section )
{
	R_ASSERT(pCreator);

	CInifile::Sect&	S		= ini->ReadSection("mobileobjects");
	for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
		if ((0==I->first) || (0==I->second))	continue;
		LPCSTR	Name		= I->second;
		LoadOne				(ini,Name);
	}
}

CObject*	CObjectList::FindObjectByName	( LPSTR name )
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

DWORD		CObjectList::GetObjectCID( CObject* What )
{
	OBJ_IT O=find(objects.begin(),objects.end(),What);
	VERIFY(O!=objects.end());
	return O-objects.begin();
}

VOID CObjectList::DestroyObject	( CObject *pObject )
{
	if (0==pObject)	return;
	objects.erase	( objects.begin()+GetObjectCID(pObject) );
	DEL_INSTANCE	( pObject );
}

VOID CObjectList::OnMove	( )
{
	// Sheduled
	Device.Statistic.UpdateSheduled.Begin	();
	sheduled.Update			( );
	Device.Statistic.UpdateSheduled.End		();

	// Clients
	Device.Statistic.UpdateClient.Begin		();
	/*
	float limit = pCreator->Environment.Current.Far; 
	limit*=2.f;
	*/
	for (OBJ_IT O=objects.begin(); O!=objects.end(); O++) 
	{
		(*O)->UpdateCL();
		/*
		CObject* P = *O;
		BOOL bRelevant = P->net_Ready && (Device.vCameraPosition.distance_to_sqr(P->Position())<limit);
		if (bRelevant) 
		*/
	}
	Device.Statistic.UpdateClient.End		();
}

VOID CObjectList::net_Export	(NET_Packet* Packet)
{
	for (OBJ_IT O=objects.begin(); O!=objects.end(); O++) 
	{
		CObject* P = *O;
		if (P->net_Relevant())	{
			Packet->w_u8	(u8(P->net_ID));
			P->net_Export	(Packet);
		}
	}
}

VOID CObjectList::net_Import	(NET_Packet* Packet)
{
	while (!Packet->r_eof())
	{
		CObject* P  = net_Find	(DWORD(Packet->r_u8()));
		if (P) P->net_Import	(Packet);
	}
}

CObject* CObjectList::net_Find	(DWORD ID)
{
	for (OBJ_IT O=objects.begin(); O!=objects.end(); O++) 
	{
		CObject* P = *O;
		if (P->net_ID == ID)	return P;
	}
	return 0;
}
