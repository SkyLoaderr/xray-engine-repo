#ifndef __XR_HUDMANAGER_H__
#define __XR_HUDMANAGER_H__
#pragma once

#include "../CustomHUD.h"
#include "HitMarker.h"
#include "UI.h"
#include "MainUI.h"
//namespace collide {
//	struct rq_result;
//};
// refs
class CContextMenu;
class CHUDCursor;

struct CFontManager :public pureDeviceReset			{
							CFontManager			();
							~CFontManager			();

	typedef xr_vector<CGameFont**>					FONTS_VEC;
	typedef FONTS_VEC::iterator						FONTS_VEC_IT;
	FONTS_VEC				m_all_fonts;
	void					Render					();
	// console font
	CGameFont*				pFontConsole;
	CGameFont*				pFontStartup;
	// hud font
	CGameFont*				pFontSmall;
	CGameFont*				pFontMedium;
	CGameFont*				pFontDI;
	CGameFont*				pFontBigDigit;
	//������������ �����
	CGameFont*				pFontHeaderRussian;
	CGameFont*				pFontHeaderEurope;
	//������ ��� ����������
	CGameFont*				pFontArial14;
	CGameFont*				pArialN21Russian;
	CGameFont*				pFontGraffiti19Russian;
	CGameFont*				pFontGraffiti22Russian;
	CGameFont*				pFontLetterica16Russian;
	CGameFont*				pFontLetterica18Russian;
	CGameFont*				pFontGraffiti32Russian;
	CGameFont*				pFontGraffiti50Russian;
	CGameFont*				pFontLetterica25;
	CGameFont*				pFontStat;

	void					InitializeFonts			();
	void					InitializeFont			(CGameFont*& F, LPCSTR section, u32 flags = 0);
	LPCSTR					GetFontTexName			(LPCSTR section);				

	virtual void			OnDeviceReset			();
};

class CHUDManager :
	public CCustomHUD
{
	friend class CUI;
private:
	// ui
	CUI*					pUI;
	// hit marker
	CHitMarker				HitMarker;
	// hud cursor
	CHUDCursor*				m_pHUDCursor;


public:
							CHUDManager			();
	virtual					~CHUDManager		();
	virtual		void		OnEvent				(EVENT E, u64 P1, u64 P2);

	virtual		void		Load				();
	
	virtual		void		Render_First		();
	virtual		void		Render_Last			();	   
	virtual		void		OnFrame				();

	virtual		void		RenderUI			();

	virtual		IC CUI*		GetUI				(){return pUI;}

	virtual		void		Hit					(int idx, float power);
	CFontManager&			Font				()							{return *(UI()->Font());}
	//������� ������� �� ������� ������� HUD
	collide::rq_result&		GetCurrentRayQuery	();

	void __cdecl 			outMessage			(u32 C, LPCSTR from, LPCSTR msg, ...);
	void __cdecl 			outMessage			(u32 C, const shared_str& from, LPCSTR msg, ...);


	//�������� �������� ���� ������� � ����������� �� ������� ���������
	void					SetCrosshairDisp	(float disp);
	void					ShowCrosshair		(bool show);
};


#endif // __XR_HUDMANAGER_H__
