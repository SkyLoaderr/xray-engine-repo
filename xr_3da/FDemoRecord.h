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
	D3DXMATRIX	g_matPosition;
	D3DXVECTOR3 g_vecVelocity;
	D3DXVECTOR3 g_vecAngularVelocity;

	void		RecordKey				();
public:
	virtual void OnKeyboardPress	(int dik);
	virtual	void Process			(Fvector &p, Fvector &d, Fvector &n);

	CDemoRecord	(const char *name, float life_time=60*60*1000);
	virtual ~CDemoRecord();
};

#endif // !defined(AFX_FDEMORECORD_H__D7638760_FB61_11D3_B4E3_4854E82A090D__INCLUDED_)
