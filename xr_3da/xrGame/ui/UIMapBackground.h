// CUIMapBackground.h:  ������������ �������� (��� ��������)
//						������������� �����
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "UIMapSpot.h"



class CUIMapBackground: public CUIButton
{
private:
	typedef CUIButton inherited;
public:
	CUIMapBackground();
	virtual ~CUIMapBackground();

	virtual void Init(int x, int y, int width, int height);
	virtual void InitMapBackground();
	virtual void Draw();
	virtual void Update();



	void ConvertToLocal(const Fvector& src, Ivector2& dest);
	void ConvertFromLocalToMap(int x, int y, Fvector2& dest);
	void ConvertToTexture(const Fvector& src, Fvector2& dest);
	void ConvertToTexture(float x, float y, Fvector2& dest);

	//������ � ������ ������������� ������� ����� � ������
	float m_fMapViewWidthMeters;
	float m_fMapViewHeightMeters;

	//������ � ������ ����� � �������� �� ������
	int m_iMapViewWidthPixels;
	int m_iMapViewHeightPixels;

	//��������� � ������� ���� �����
	float m_fMapWidthMeters;
	float m_fMapHeightMeters;
	float m_fMapLeftMeters;
	float m_fMapTopMeters;
	float m_fMapBottomMeters;

	//������� ��������� �������� ������ ���� �����
	//� ������ �� ������ ��������� 
	//(0,0)- left top
	float m_fMapX;
	float m_fMapY;

	//������ ����������� �� �����
	DEFINE_VECTOR	(CUIMapSpot*, MAP_SPOT_VECTOR, MAP_SPOT_VECTOR_IT);
	MAP_SPOT_VECTOR m_vMapSpots;

protected:
	void DrawFogOfWar();
	void DrawFogOfWarCell(int x, int y);
	
	//���������� ������ �� �����
	void UpdateMapSpots();
	void UpdateActorPos();
	//�������� ��� �����
	CUIStaticItem landscape;

	CUIMapSpot m_fogOfWarCell;


	//��� �������������� ����� ��� ������ ����
	void OnMouse(int x, int y, E_MOUSEACTION mouse_action);
	int m_iOldMouseX;
	int m_iOldMouseY;
	POINT m_previousPos;


};
