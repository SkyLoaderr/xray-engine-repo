// DummyObject.h: interface for the CDummyObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUMMYOBJECT_H__3A3BE489_045B_4229_AD0F_83CACEA62145__INCLUDED_)
#define AFX_DUMMYOBJECT_H__3A3BE489_045B_4229_AD0F_83CACEA62145__INCLUDED_
#pragma once

// refs
class CObjectAnimator;

class CDummyObject		: public CObject
{
	typedef	CObject		inherited;

private:
	enum SStyle{
		esAnimated	=1<<0,	
		esSkeleton	=1<<1, 
		esParticles	=1<<2, 
		esSound		=1<<3
	};
	DWORD				style;
private:
	CObjectAnimator*	animator;
	Fvector				start_position;
	sound				snd;

public:
	virtual void		Load			( CInifile* ini, LPCSTR section);
	virtual void		Update			( DWORD dt );

	void				PlayDemo		( LPCSTR N );

	CDummyObject		();
	virtual ~CDummyObject();
};

#endif // !defined(AFX_DUMMYOBJECT_H__3A3BE489_045B_4229_AD0F_83CACEA62145__INCLUDED_)
