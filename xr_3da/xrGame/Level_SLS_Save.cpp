#include "stdafx.h"
#include "HUDmanager.h"
#include "..\\xrLevel.h"

void	CLevel::net_Save				(LPCSTR name)		// Game Save
{
	if (0==Server)		{
		HUD().outMessage(D3DCOLOR_RGBA(0xff,0,0,0xff),"KERNEL", "Can't save game on pure client");
		return;
	}

	// 1. Create stream
	CMemoryWriter			fs;

	// 2. Description
	fs.open_chunk		(fsSLS_Description);
	fs.w_stringZ		(net_SessionName());
	fs.close_chunk		();

	// 3. Server state
	fs.open_chunk		(fsSLS_ServerState);
	Server->SLS_Save	(fs);
	fs.close_chunk		();

	// 4. Units Specific
	fs.open_chunk		(fsSLS_UnitsSpecific);
	Objects.SLS_Save    (fs);
	fs.close_chunk		();

	// Save it to file
	fs.save_to			(name);
}
