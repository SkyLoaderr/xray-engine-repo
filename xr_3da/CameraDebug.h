// CameraDebug.h: interface for the CCameraDebug class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERADEBUG_H__B11F8AE3_1213_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_CAMERADEBUG_H__B11F8AE3_1213_11D4_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "CameraBase.h"
#include "fcontroller.h"

class ENGINE_API CCameraDebug :
	public CCameraBase,
	public pureRender,
	public CController
{
	D3DXMATRIX      g_matPosition;
    D3DXVECTOR3		g_vecVelocity;
    D3DXVECTOR3		g_vecAngularVelocity;
	Fvector			savedP,savedD,savedN;
	ECameraStyle	savedStyle;
	DWORD			savedDF;
public:
	virtual	void	OnMove				(void);
	virtual	void	OnCameraActivate	( CCameraBase* old_cam );
	virtual	void	OnCameraDeactivate	( );
	virtual void	OnRender			( );
	virtual void	OnKeyboardPress		(int dik);

	CCameraDebug();
	virtual ~CCameraDebug();
};

#endif // !defined(AFX_CAMERADEBUG_H__B11F8AE3_1213_11D4_B4E3_4854E82A090D__INCLUDED_)
