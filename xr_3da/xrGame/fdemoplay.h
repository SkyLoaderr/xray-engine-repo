// CDemoPlay.h: interface for the CDemoPlay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FDEMOPLAY_H__9B07E6E0_FC3C_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FDEMOPLAY_H__9B07E6E0_FC3C_11D3_B4E3_4854E82A090D__INCLUDED_

#include "ftimer.h"
#include "effector.h"
#include "xr_list.h"

class ENGINE_API CDemoPlay :
	public CEffector
{
	vector	<Fmatrix>	seq;
	int					m_count;
	float				fStartTime;
	float				fSpeed;
	BOOL				bCycle;
	CTimer				timer;
	DWORD				dwFramesRendered;
public:
	virtual	void		Process		(Fvector &p, Fvector &d, Fvector &n);

						CDemoPlay	(const char *name, float ms, BOOL cycle=false, float life_time=60*60*1000);
	virtual				~CDemoPlay	();
};

#endif // !defined(AFX_FDEMOPLAY_H__9B07E6E0_FC3C_11D3_B4E3_4854E82A090D__INCLUDED_)
