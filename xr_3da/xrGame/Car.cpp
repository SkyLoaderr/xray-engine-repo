#include "stdafx.h"
#include "car.h"
#include "..\\camerabase.h"

CCar::CCar(void)
{
}

CCar::~CCar(void)
{
}

void	CCar::cam_Update			(float dt)
{
	Fvector							P,Da;
	clCenter						(P);
	Da.set							(0,0,0);

	camera->Update					(P,Da);
	Level().Cameras.Update			(camera);
}

// Core events
void	CCar::Load					( CInifile* ini, const char *section )
{
	inherited::Load					(ini,section);
}

BOOL	CCar::Spawn					( BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags )
{
	return inherited::Spawn			(bLocal,server_id,o_pos,o_angle,P,flags);
}

void	CCar::Update				( DWORD T )
{
	inherited::Update				(T);

	float dt						= float(T)/1000.f;

	if (IsMyCamera())				cam_Update(dt);
}

void	CCar::OnVisible				( )
{
	inherited::OnVisible			();
}

void	CCar::net_Export			(NET_Packet* P)
{
}

void	CCar::net_Import			(NET_Packet* P)
{
}

void	CCar::OnMouseMove			(int x, int y)
{
}

void	CCar::OnKeyboardPress		(int dik)
{
}

void	CCar::OnKeyboardRelease		(int dik)
{
}

void	CCar::OnKeyboardHold		(int dik)
{
}

void	CCar::OnHUDDraw				(CCustomHUD* hud)
{
}
