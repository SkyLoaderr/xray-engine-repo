// Spawn_Event.cpp: implementation of the CSpawn_Event class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Spawn_Event.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpawn_Event::CSpawn_Event()
{
	Commands.push_back	(Pair());
	Commands.back().Target.Count	= 2;
}

CSpawn_Event::~CSpawn_Event()
{

}

void CSpawn_Event::Save	(CFS_Base& FS)
{
	for (int cmd=0; cmd<Commands.size(); cmd++)
	{
		xrP_Template	tmpl;
		xrP_TOKEN::Item	item;
		CLASS_ID		cls;
		Pair& P			= Commands[cmd];
		tmpl.Type		= 0;
		tmpl.Limit		= 10;

		// 
		xrPWRITE_PROP	(FS,"Action",		xrPID_MARKER_TEMPLATE,	tmpl		);
		xrPWRITE_PROP	(FS,"Execute Once",	xrPID_BOOL,				P.bOnce		);
		xrPWRITE_PROP	(FS,"Target Class",	xrPID_CLSID,			P.Target	);
		cls = CLSID_OBJECT_ACTOR;	FS.write(&cls,sizeof(cls));
		cls = CLSID_AI_HEN;			FS.write(&cls,sizeof(cls));

		//
		xrPWRITE_PROP	(FS,"ENTER: type",	xrPID_TOKEN,			P.OnEnter.type);
		item.ID = typeNone;			strcpy(item.str,"none");		FS.write(&item,sizeof(item));
		item.ID = typeActivate;		strcpy(item.str,"activate");	FS.write(&item,sizeof(item));
		item.ID = typeDeactivate;	strcpy(item.str,"deactivate");	FS.write(&item,sizeof(item));
		item.ID = typeCustom;		strcpy(item.str,"custom");		FS.write(&item,sizeof(item));
		xrPWRITE_PROP	(FS,"ENTER: object",xrPID_OBJECT,			P.OnEnter.target);
		xrPWRITE_PROP	(FS,"ENTER: custom",xrPID_STRING,			P.OnEnter.custom);

		//
		xrPWRITE_PROP	(FS,"LEAVE: type",	xrPID_TOKEN,			P.OnLeave.type);
		item.ID = typeNone;			strcpy(item.str,"none");		FS.write(&item,sizeof(item));
		item.ID = typeActivate;		strcpy(item.str,"activate");	FS.write(&item,sizeof(item));
		item.ID = typeDeactivate;	strcpy(item.str,"deactivate");	FS.write(&item,sizeof(item));
		item.ID = typeCustom;		strcpy(item.str,"custom");		FS.write(&item,sizeof(item));
		xrPWRITE_PROP	(FS,"LEAVE: object",xrPID_OBJECT,			P.OnLeave.target);
		xrPWRITE_PROP	(FS,"LEAVE: custom",xrPID_STRING,			P.OnLeave.custom);
	}
}
