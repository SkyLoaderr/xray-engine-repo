// 2DSoundRender.h: interface for the C2DSoundRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DSOUNDRENDER_H__588F4B10_3C51_496B_9BE0_B36391D62B74__INCLUDED_)
#define AFX_2DSOUNDRENDER_H__588F4B10_3C51_496B_9BE0_B36391D62B74__INCLUDED_
#pragma once

// refs
class ENGINE_API C2DSound;
class ENGINE_API CInifile;

class ENGINE_API C2DSoundRender
{
	vector <vector<C2DSound*> >	sounds;
	vector <int>				refcounts;

	C2DSound*					GetFreeSound	(int hSound);
	int							FindByName		(LPCSTR name);
	int							FindEmptySlot	();
	int							Append			(C2DSound *p);
public:
	int							CreateSound	(CInifile *pIni, LPCSTR section);
	int							CreateSound	(LPCSTR name	);
	void						DeleteSound	(int& hSound);
	C2DSound*					Play		(int  hSound, BOOL bLoop=false, int iLoopCnt=0);

	void						Reload		();

	void						OnMove		(void);

	C2DSoundRender();
	~C2DSoundRender();
};

#endif // !defined(AFX_2DSOUNDRENDER_H__588F4B10_3C51_496B_9BE0_B36391D62B74__INCLUDED_)
