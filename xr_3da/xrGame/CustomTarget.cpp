#include "stdafx.h"
#include "customtarget.h"

CCustomTarget::CCustomTarget(void)
{
}

CCustomTarget::~CCustomTarget(void)
{
}

void CCustomTarget::Load(LPCSTR section)
{
	inherited::Load(section);
}

BOOL CCustomTarget::net_Spawn			(LPVOID DC)
{
	return inherited::net_Spawn(DC);
}
void CCustomTarget::net_Destroy			()
{
	inherited::net_Destroy();
}
void CCustomTarget::net_Export			(NET_Packet& P)		// export to server
{
	inherited::net_Export(P);
}
void CCustomTarget::net_Import			(NET_Packet& P)		// import from server
{
	inherited::net_Import(P);
}
void CCustomTarget::Update				(DWORD dt)
{
	inherited::Update(dt);
}
void CCustomTarget::UpdateCL			()
{
	inherited::UpdateCL();
}
/*
void CCustomTarget::OnEvent				()
{
	inherited::OnVisible();
}
*/
void CCustomTarget::OnH_A_Chield		()
{
	inherited::OnH_A_Chield		();
	setVisible					(false);
	setEnabled					(false);

	if (Local()) 
	{
		NET_Packet		P;

		switch (GAME)
		{
		case GAME_ASSAULT:
			{
				u_EventGen		(P,GEG_ASSAULT_ACOMPLISHED,ID());
				u_EventSend		(P);
			}
		}
	}
}
void CCustomTarget::OnH_B_Independent	()
{
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
}
