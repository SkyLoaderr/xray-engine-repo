#pragma once

class CBloodsuckerVisibility {
	bool				flagVisibility;					// true - ���� �����
	TTime				timeBlinkInterval;				// ����� ������ ��������
	TTime				timeStartBlinking;				// ����� ������ ��������� ��������� �������
	TTime				timeBlinking;					// ����� ��������
	bool				bCurVisibility;					// ������� ���������

	TTime				timeLastBlink;					// ����� ���������� ������������ ������ ��������
	TTime				timeBlinkIntervalCurrent;		// ������� ����� ������ ��������
public:
				CBloodsuckerVisibility	();
	
		void	Load					(LPCTSTR section);
		void	Switch					(bool bVis);
		bool	Update					();				// ���������� ����� ��������� ���������
	IC	bool	IsVisible				() {return bCurVisibility;}
		
	// Temp 
	IC	bool	IsActive				() {if (timeStartBlinking + timeBlinking > Level().timeServer()) return true; else return false;}
};

