#include "stdafx.h"
#include "HUDmanager.h"

void	CLevel::SLS_Save				(LPCSTR name)		// Game Save
{
	if (0==Server)		{
		HUD()->outMessage(D3DCOLOR_RGBA(0xff,0,0,0xff),"KERNEL", "Can't save game on pure client");
		return;
	}

	// 1. Create stream
	CFS_Memory			fs;

	// 2. Description
	fs.open_chunk		(fsSLS_Description);
	fs.WstringZ			(net_SessionName());
	fs.close_chunk		();

	// 3. Server state
	fs.open_chunk		(fsSLS_ServerState);
	Server->Save		(fs);
	fs.close_chunk		();

	// 4. Units Specific
	fs.open_chunk		(fsSLS_UnitsSpecific);
	Objects.SLS_Save    (fs);
	fs.close_chunk		();

	// Save it to file
	fs.SaveTo			(name,0);
}
