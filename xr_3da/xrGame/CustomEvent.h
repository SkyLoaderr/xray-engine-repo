// CustomEvent.h: interface for the CCustomEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMEVENT_H__092059D7_D2E8_4BC3_95C5_F2B3D48AAE5C__INCLUDED_)
#define AFX_CUSTOMEVENT_H__092059D7_D2E8_4BC3_95C5_F2B3D48AAE5C__INCLUDED_
#pragma once

class CCustomEvent		: public CObject, public pureRender
{
private:
	typedef CObject			inherited;
protected:
	// t-defs
	struct DEF_EVENT	{
		EVENT	E;
		LPSTR	P1;
	};
	struct DEF_ACTION	{
		DEF_EVENT	OnEnter;
		DEF_EVENT	OnLeave;
		CLASS_ID	CLS;
		u8			bOnce;
	};
	typedef svector<DEF_ACTION,16>	tActions;

	tActions				Actions;
	vector<CObject*>		Contacted;
protected:
	DEF_EVENT				Parse				(LPCSTR DEF);
public:
	CCustomEvent();
	virtual ~CCustomEvent();

	virtual void			Load				( CInifile* ini, const char * section );
	virtual BOOL			Spawn				( BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags );
	
	
	// Update
	virtual void			Update				( DWORD dt );

	// Collision
	virtual void			OnNear				( CObject* near   );
	virtual void			OnRender			( );
};

#endif // !defined(AFX_CUSTOMEVENT_H__092059D7_D2E8_4BC3_95C5_F2B3D48AAE5C__INCLUDED_)
