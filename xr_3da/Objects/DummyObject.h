// DummyObject.h: interface for the CDummyObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUMMYOBJECT_H__3A3BE489_045B_4229_AD0F_83CACEA62145__INCLUDED_)
#define AFX_DUMMYOBJECT_H__3A3BE489_045B_4229_AD0F_83CACEA62145__INCLUDED_
#pragma once

// refs
class ENGINE_API C3DSound;
class CObjectAnimator;

class CDummyObject : public CObject, public pureRender
{
	CObjectAnimator*	animator;
	Fvector				start_position;

	enum SStyle{
		esAnimated		= 0x0001,
		esSkeleton		= 0x0002
	};
	DWORD				style;

	sound				sndDummy;

	typedef	CObject		inherited;
public:
	virtual void		Load			(CInifile* ini, const char* section);
	virtual void		Update			( DWORD dt );

	void				PlayDemo		(LPCSTR N);

	virtual void		OnRender		( );
	CDummyObject		();
	virtual ~CDummyObject();
};

#endif // !defined(AFX_DUMMYOBJECT_H__3A3BE489_045B_4229_AD0F_83CACEA62145__INCLUDED_)
