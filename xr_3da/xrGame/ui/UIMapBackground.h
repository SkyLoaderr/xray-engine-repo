// CUIMapBackground.h:  ������������ �������� (��� ��������)
//						������������� �����
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "UIMapSpot.h"

//////////////////////////////////////////////////////////////////////////

class CUIMapBackground: public CUIButton
{
private:
	typedef CUIButton inherited;
public:
	CUIMapBackground();
	virtual ~CUIMapBackground();

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

	//������ � ������ ������������� ������� ����� � ������
	float			m_fMapViewWidthMeters;
	float			m_fMapViewHeightMeters;

	//������ � ������ ����� � �������� �� ������
	int				m_iMapViewWidthPixels;
	int				m_iMapViewHeightPixels;

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
	Fbox			m_LevelBox;

	//������ ����������� �� �����
	typedef boost::shared_ptr<CUIMapSpot> MapSpotPtr;
	DEFINE_VECTOR	(MapSpotPtr, MAP_SPOT_VECTOR, MAP_SPOT_VECTOR_IT);
	MAP_SPOT_VECTOR m_vMapSpots;
	CUIMapSpot		*m_pActiveMapSpot;

	// ���������� ����� �� �����������
	void			UpdateActivePos			();
	// ������ �� ��, ��� �� ���� ��� ��������� ������ ����� ������������ �� �� ������, ��� ��� 
	// ��������� ���������� �������� ������������ ����� �� �����
	bool			m_bNoActorFocus;

	// �������������/������ �������� ������� �� �����
	Fvector			GetActivePos			() const					{ return m_vActivePos; }
	void			SetActivePos			(const Fvector &vNewPos)	{ m_vActivePos = vNewPos; }

	// ������� �����
	void			MoveMap					(const int deltaX, const int deltaY);
	void			RemoveAllSpots			();

protected:
	void			DrawFogOfWar			();
	void			DrawFogOfWarCell		(int x, int y);
	
	//���������� ������ �� �����
	void			UpdateMapSpots			();
	void			UpdateActorPos			();

	//�������� ��� �����
	CUIStaticItem	landscape;
	CUIMapSpot		m_fogOfWarCell;

protected:
	int				m_iOldMouseX;
	int				m_iOldMouseY;
	POINT			m_previousPos;

	// ����� �� ����� �� ������� ���� �������������
	Fvector			m_vActivePos;
};
