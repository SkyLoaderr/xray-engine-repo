// CDemoRecord.h: interface for the CDemoRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FDEMORECORD_H__D7638760_FB61_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FDEMORECORD_H__D7638760_FB61_11D3_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "fcontroller.h"
#include "effector.h"

class CDemoRecord :
	public CController,
	public CEffector
{
private:
	int			iCount;
	int			hFile;
///	D3DXMATRIX	g_matPosition;
///	D3DXVECTOR3 g_vecVelocity;
///	D3DXVECTOR3 g_vecAngularVelocity;
	Fvector		m_HPB;
	Fvector		m_Position;
	Fmatrix		m_Camera;

	Fvector		m_vT;
    Fvector		m_vR;
	Fvector		m_vVelocity;
	Fvector		m_vAngularVelocity;

	void		RecordKey				();
public:
				CDemoRecord				(const char *name, float life_time=60*60*1000);
	virtual		~CDemoRecord();

	virtual void OnKeyboardPress		(int dik);
	virtual void OnKeyboardHold			(int dik);
	virtual void OnMouseMove			(int dx, int dy);
	virtual void OnMouseHold			(int btn);
	
	virtual	void Process				(Fvector &p, Fvector &d, Fvector &n);
};

#endif // !defined(AFX_FDEMORECORD_H__D7638760_FB61_11D3_B4E3_4854E82A090D__INCLUDED_)
