// CDemoRecord.cpp: implementation of the CDemoRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"
#include "x_ray.h"

#include "gamefont.h"
#include "fDemoRecord.h"
#include "xr_ioconsole.h"
#include "xr_input.h"
#include "xr_object.h"
#include "render.h"
#include "CustomHUD.h"

CDemoRecord * xrDemoRecord = 0;

#define		g_fSpeed		5.0f
#define		g_fAngularSpeed 1.0f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDemoRecord::CDemoRecord(const char *name,float life_time) : CEffector(cefDemo,life_time,FALSE)
{
	_unlink	(name);
	file	= FS.w_open	(name);
	if (file) 
	{
		IR_Capture		();	// capture input
		m_Camera.invert	(Device.mView);

		// parse yaw
		Fvector& dir	= m_Camera.k;
		Fvector DYaw;	DYaw.set(dir.x,0.f,dir.z); DYaw.normalize_safe();
		if (DYaw.x<0)	m_HPB.x = acosf(DYaw.z);
		else			m_HPB.x = 2*PI-acosf(DYaw.z);

		// parse pitch
		dir.normalize_safe	();
		m_HPB.y			= asinf(dir.y);
		m_HPB.z			= 0;

		m_Position.set	(m_Camera.c);

		m_vVelocity.set	(0,0,0);
		m_vAngularVelocity.set(0,0,0);
		iCount = 0;
		
		m_vT.set(0,0,0);
		m_vR.set(0,0,0);
		m_bMakeCubeMap		= FALSE;
		m_bMakeScreenshot	= FALSE;
	} else {
		fLifeTime = -1;
	}
}

CDemoRecord::~CDemoRecord()
{
	if (file) {
		IR_Release	();	// release input
		FS.w_close	(file);
	}
}

//								+X,				-X,				+Y,				-Y,			+Z,				-Z
static Fvector cmNorm[6]	= {{0.f,1.f,0.f}, {0.f,1.f,0.f}, {0.f,0.f,-1.f},{0.f,0.f,1.f}, {0.f,1.f,0.f}, {0.f,1.f,0.f}};
static Fvector cmDir[6]		= {{1.f,0.f,0.f}, {-1.f,0.f,0.f},{0.f,1.f,0.f}, {0.f,-1.f,0.f},{0.f,0.f,1.f}, {0.f,0.f,-1.f}};

static Flags32	s_hud_flag	= {0};
static u32		s_idx;

void CDemoRecord::MakeScreenshotFace()
{
	switch (s_idx){
	case 0:
		s_hud_flag.set	(psHUD_Flags);
		psHUD_Flags.set	(0);
	break;
	case 1:
		Render->Screenshot	();
		psHUD_Flags.set	(s_hud_flag);
		m_bMakeScreenshot= FALSE;
	break;
	}
	s_idx++;
}

void CDemoRecord::MakeCubeMapFace(Fvector &D, Fvector &N)
{
	string32 buf;
	switch (s_idx){
	case 0:
		N.set		(cmNorm[s_idx]);
		D.set		(cmDir[s_idx]);
		s_hud_flag.set(psHUD_Flags);
		psHUD_Flags.set	(0);
	break;
	case 1: 
	case 2:
	case 3:
	case 4:
	case 5:
		N.set		(cmNorm[s_idx]);
		D.set		(cmDir[s_idx]);
		Render->Screenshot	(itoa(s_idx,buf,10),TRUE);
	break;
	case 6:
		Render->Screenshot	(itoa(s_idx,buf,10),TRUE);
		N.set		(m_Camera.j);
		D.set		(m_Camera.k);
		psHUD_Flags.set(s_hud_flag);
		m_bMakeCubeMap = FALSE;
	break;
	}
	s_idx++;
}

BOOL CDemoRecord::Process(Fvector &P, Fvector &D, Fvector &N, float& fFov, float& fFar, float& fAspect)
{
	if (0==file)	return TRUE;

	if (m_bMakeScreenshot){
		MakeScreenshotFace();
		// update camera
		N.set(m_Camera.j);
		D.set(m_Camera.k);
		P.set(m_Camera.c);
	}else if (m_bMakeCubeMap){
		MakeCubeMapFace(D,N);
		P.set(m_Camera.c);
		fAspect = 1.f;
	}else{
		if (psHUD_Flags.test(HUD_DRAW)){
			if ((Device.dwTimeGlobal/500)%2==0) {
				pApp->pFontSystem->SetSize	(0.02f);
				pApp->pFontSystem->SetAligment(CGameFont::alCenter);
				pApp->pFontSystem->SetColor	(color_rgba(255,0,0,255));
				pApp->pFontSystem->OutSet	(0,+.05f);
				pApp->pFontSystem->OutNext	("%s","RECORDING");
				pApp->pFontSystem->OutNext	("Key frames count: %d",iCount);
				pApp->pFontSystem->OutNext	("(SPACE=key-frame, BACK=CubeMap, ENTER=Place&Quit, F12=ScreenShot, ESC=Quit)");
			}
		}

		m_vVelocity.lerp		(m_vVelocity,m_vT,0.3f);
		m_vAngularVelocity.lerp	(m_vAngularVelocity,m_vR,0.3f);

		float acc = 1.f, acc_angle = 1.f;
		if (Console.IR_GetKeyState(DIK_LSHIFT)){ acc=.025f; acc_angle=.025f;}
		else if (Console.IR_GetKeyState(DIK_LALT)) acc=4.0;
		else if (Console.IR_GetKeyState(DIK_LCONTROL)) acc=10.0;
		m_vT.mul				(m_vVelocity, Device.fTimeDelta * g_fSpeed * acc);
		m_vR.mul				(m_vAngularVelocity, Device.fTimeDelta * g_fAngularSpeed * acc_angle);

		m_HPB.x -= m_vR.y;
		m_HPB.y -= m_vR.x;
		m_HPB.z += m_vR.z;

		// move
		Fvector vmove;

		vmove.set				(m_Camera.k);
		vmove.normalize_safe	();
		vmove.mul				(m_vT.z);
		m_Position.add			(vmove);

		vmove.set				(m_Camera.i);
		vmove.normalize_safe	();
		vmove.mul				(m_vT.x);
		m_Position.add			(vmove);

		vmove.set				(m_Camera.j);
		vmove.normalize_safe	();
		vmove.mul				(m_vT.y);
		m_Position.add			(vmove);

		m_Camera.setHPB			(m_HPB.x,m_HPB.y,m_HPB.z);
		m_Camera.translate_over	(m_Position);

		// update camera
		N.set(m_Camera.j);
		D.set(m_Camera.k);
		P.set(m_Camera.c);

		fLifeTime-=Device.fTimeDelta;

		m_vT.set(0,0,0);
		m_vR.set(0,0,0);
	}
	return TRUE;
}

void CDemoRecord::IR_OnKeyboardPress	(int dik)
{
	if (dik == DIK_SPACE)	RecordKey();
	if (dik == DIK_BACK)	MakeCubemap();
	if (dik == DIK_F12)		MakeScreenshot();
	if (dik == DIK_ESCAPE)	fLifeTime = -1; //g_pGameLevel->Cameras.RemoveEffector(cefDemo);
	if (dik == DIK_RETURN){	
		if (g_pGameLevel->CurrentEntity()){
			g_pGameLevel->CurrentEntity()->ForceTransform(m_Camera);
			//g_pGameLevel->Cameras.RemoveEffector(cefDemo);
			fLifeTime		= -1; 
		}
	}
}

void CDemoRecord::IR_OnKeyboardHold	(int dik)
{
	switch(dik){
	case DIK_A:
	case DIK_NUMPAD1:
	case DIK_LEFT:		m_vT.x -= 1.0f; break; // Slide Left
	case DIK_D:
	case DIK_NUMPAD3:
	case DIK_RIGHT:		m_vT.x += 1.0f; break; // Slide Right
	case DIK_S:			m_vT.y -= 1.0f; break; // Slide Down
	case DIK_W:			m_vT.y += 1.0f; break; // Slide Up
	// rotate	
	case DIK_NUMPAD2:	m_vR.x -= 1.0f; break; // Pitch Down
	case DIK_NUMPAD8:	m_vR.x += 1.0f; break; // Pitch Up
	case DIK_E:	
	case DIK_NUMPAD6:	m_vR.y += 1.0f; break; // Turn Left
	case DIK_Q:	
	case DIK_NUMPAD4:	m_vR.y -= 1.0f; break; // Turn Right
	case DIK_NUMPAD9:	m_vR.z -= 2.0f; break; // Turn Right
	case DIK_NUMPAD7:	m_vR.z += 2.0f; break; // Turn Right
	}
}

void CDemoRecord::IR_OnMouseMove		(int dx, int dy)
{
	float scale			= .5f;//psMouseSens;
	if (dx||dy){
		m_vR.y			+= float(dx)*scale; // heading
		m_vR.x			+= ((psMouseInvert.test(1))?-1:1)*float(dy)*scale*(3.f/4.f); // pitch
	}
}

void CDemoRecord::IR_OnMouseHold		(int btn)
{
	switch (btn){
	case 0:			m_vT.z += 1.0f; break; // Move Backward
	case 1:			m_vT.z -= 1.0f; break; // Move Forward
	}
}

void CDemoRecord::RecordKey			()
{
	Fmatrix			g_matView;
 
	g_matView.invert(m_Camera);
	file->w			(&g_matView,sizeof(Fmatrix));
	iCount++;
}

void CDemoRecord::MakeCubemap		()
{
	m_bMakeCubeMap	= TRUE;
	s_idx			= 0;
}

void CDemoRecord::MakeScreenshot	()
{
	m_bMakeScreenshot = TRUE;
	s_idx = 0;
}
