// CustomEvent.h: interface for the CCustomEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMEVENT_H__092059D7_D2E8_4BC3_95C5_F2B3D48AAE5C__INCLUDED_)
#define AFX_CUSTOMEVENT_H__092059D7_D2E8_4BC3_95C5_F2B3D48AAE5C__INCLUDED_
#pragma once

struct EV_LIST	{
	// t-defs
	struct EV_DEF	{
		EVENT	E;
		LPSTR	P1;
	};
	typedef svector<EV_DEF,8>	EV;
	typedef EV_LIST::iterator	EV_IT;
	
	// data
	EV		List;
	
	// methods
	void	_CreateOne	(const char* DEF);
	void	Create		(const char* DEF);
	void	Destroy		();
	void	Signal		(DWORD P2);
};

class CCustomEvent : public CObject, public pureRender
{
	EV_LIST				OnEnter;
	EV_LIST				OnExit;
	CLASS_ID			clsid_Target;
	vector<CObject*>	Contacted;
	BOOL				ExecuteOnce;
public:
	CCustomEvent();
	virtual ~CCustomEvent();

	virtual void						Load					( CInifile* ini, const char * section );

	// Update
	virtual void						Update					( DWORD dt );

	// Collision
	virtual void						OnNear					( CObject* near   );
	virtual void						OnRender				( );
};

#endif // !defined(AFX_CUSTOMEVENT_H__092059D7_D2E8_4BC3_95C5_F2B3D48AAE5C__INCLUDED_)
