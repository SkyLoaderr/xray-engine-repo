// XR_Quit.cpp: implementation of the CXR_Quit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "x_ray.h"
#include "xr_input.h"
#include "XR_Quit.h"
#include "xr_msg.h"

// CXR_Quit *xrQuit=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXR_Quit::CXR_Quit()
{
	iCapture();
	Device.seqRender2D.Add(this,REG_PRIORITY_CAPTURE);
}

CXR_Quit::~CXR_Quit()
{
	Device.seqRender2D.Remove(this);
	iRelease();
}

void CXR_Quit::OnRender2D()
{
//	CHK_DX(HW.pContext->Clear( D3DCLEAR_TARGET ));
//	splash->Render();
}

void CXR_Quit::OnMousePress(int btn)
{
	PutMessage(msg_real_quit);
}

void CXR_Quit::OnKeyboardPress(int btn)
{
	PutMessage(msg_real_quit);
}
