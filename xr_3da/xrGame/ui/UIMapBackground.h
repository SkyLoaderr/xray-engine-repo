// CUIMapBackground.h:  ������������ �������� (��� ��������)
//						������������� �����
//////////////////////////////////////////////////////////////////////
/*
#pragma once

#include "UIStatic.h"
#include "UIMapSpot.h"

//////////////////////////////////////////////////////////////////////////

class CUIMapBackground: public CUIButton
{
private:
	typedef CUIButton inherited;
protected:
	//�������� ��� �����
	CUIStaticItem	landscape;
	CUIMapSpot		m_fogOfWarCell;

	int				m_iOldMouseX;
	int				m_iOldMouseY;

	// ����� �� ����� �� ������� ���� �������������
	Fvector			m_vActivePos;
public:
	//������ � ������ ������������� ������� ����� � ������
	float			m_fMapViewWidthMeters;
	float			m_fMapViewHeightMeters;

	//��������� � ������� ���� �����
	float			m_fMapWidthMeters;
	float			m_fMapHeightMeters;
	float			m_fMapLeftMeters;
	float			m_fMapTopMeters;
	float			m_fMapBottomMeters;

	//������� ��������� �������� ������ ���� �����
	//� ������ �� ������ ��������� 
	//(0,0)- left top
	float			m_fMapX;
	float			m_fMapY;

	// ������ �� ��, ��� �� ���� ��� ��������� ������ ����� ������������ �� �� ������, ��� ��� 
	// ��������� ���������� �������� ������������ ����� �� �����
	bool			m_bNoActorFocus;

	//������ ����������� �� �����
	typedef boost::shared_ptr<CUIMapSpot> MapSpotPtr;
	DEFINE_VECTOR	(MapSpotPtr, MapSpotPtrVec, MapSpotPtrVecIt);
	MapSpotPtrVec	m_vMapSpots;
	CUIMapSpot*		m_pActiveMapSpot;
protected:
	void			DrawFogOfWar			();
	void			DrawFogOfWarCell		(int x, int y);

	//���������� ������ �� �����
	void			UpdateMapSpots			();
	void			UpdateActorPos			();
public:
	// ���������� ����� �� �����������
	void			UpdateActivePos			();

	// �������������/������ �������� ������� �� �����
	Fvector			GetActivePos			() const					{ return m_vActivePos; }
	void			SetActivePos			(const Fvector &vNewPos)	{ m_vActivePos = vNewPos; }

	// ������� �����
	void			MoveMap					(const int deltaX, const int deltaY);
	void			RemoveAllSpots			();
public:
					CUIMapBackground		();
	virtual			~CUIMapBackground		();

	virtual void	Init					(int x, int y, int width, int height);
	virtual void	InitMapBackground		(const ref_shader &sh);
	virtual void	Draw					();
	virtual void	Update					();
	virtual void	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	//��� �������������� ����� ��� ������ ����
	virtual void	OnMouse					(int x, int y, EUIMessages mouse_action);

	void			ConvertToLocal			(const Fvector& src, Ivector2& dest);
	void			ConvertFromLocalToMap	(int x, int y, Fvector2& dest);
	void			ConvertToTexture		(const Fvector& src, Fvector2& dest);
	void			ConvertToTexture		(float x, float y, Fvector2& dest);
};
*/