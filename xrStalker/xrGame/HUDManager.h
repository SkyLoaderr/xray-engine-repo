#ifndef __XR_HUDMANAGER_H__
#define __XR_HUDMANAGER_H__
#pragma once

#include "../CustomHUD.h"
#include "HitMarker.h"
#include "UI.h"

// refs
class CContextMenu;
class CHUDCursor;

class CHUDManager :
	public CCustomHUD,
	public pureDeviceCreate 
{
	friend class CUI;
private:
	// ui
	CUI*					pUI;
	// hit marker
	CHitMarker				HitMarker;
	// hud cursor
	CHUDCursor*				m_pHUDCursor;

	float					fScale;
public:
	// hud font
	CGameFont*				pFontSmall;
	CGameFont*				pFontMedium;
	CGameFont*				pFontDI;
	CGameFont*				pFontBigDigit;

	//������������ �����
	CGameFont*				pFontHeaderRussian;
	CGameFont*				pFontHeaderEurope;
	//������ ��� ����������
	CGameFont*				pArialN21Russian;
	CGameFont*				pFontGraffiti19Russian;
	CGameFont*				pFontGraffiti22Russian;
	CGameFont*				pFontLetterica16Russian;
	CGameFont*				pFontLetterica18Russian;


public:
							CHUDManager			();
	virtual					~CHUDManager		();
	virtual		void		OnEvent				(EVENT E, u64 P1, u64 P2);

	virtual		void		Load				();
	
	virtual		void		Render_First		();
	virtual		void		Render_Last			();	   
	virtual		void		Render_Direct		();
	virtual		void		OnFrame				();

	virtual		IC CUI*		GetUI				(){return pUI;}

	virtual		void		Hit					(int idx){HitMarker.Hit(idx);}
	
	//������� ������� �� ������� ������� HUD
	Collide::rq_result&		GetCurrentRayQuery	();

	virtual		void		OnDeviceCreate		();

	float					GetScale			(){return fScale;}
	void					SetScale			(float s);

	void					ClientToScreenScaled(Irect& r, u32 align);
	void					ClientToScreenScaled(Ivector2& dest, int left, int top, u32 align);
	int						ClientToScreenScaledX(int left, u32 align);
	int						ClientToScreenScaledY(int top, u32 align);
	void					ClientToScreen		(Irect& r, u32 align);
	void					ClientToScreen		(Ivector2& dest, int left, int top, u32 align);
	int						ClientToScreenX		(int left, u32 align);
	int						ClientToScreenY		(int top, u32 align);
	
	void __cdecl 			outMessage			(u32 C, LPCSTR from, LPCSTR msg, ...);
	void __cdecl 			outMessage			(u32 C, const ref_str& from, LPCSTR msg, ...);


	//�������� �������� ���� ������� � ����������� �� ������� ���������
	void					SetCrosshairDisp	(float disp);
	void					ShowCrosshair		(bool show);
};

#endif // __XR_HUDMANAGER_H__
