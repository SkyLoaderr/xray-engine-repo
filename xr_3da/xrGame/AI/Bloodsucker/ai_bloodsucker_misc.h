#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////
// ���������� ����������� (��������)
///////////////////////////////////////////////////////////////////////////////////////////////

class CBloodsuckerVisibility {
	bool				flagVisibility;					// true - ���� �����

	// ��������� ��������
	bool				bCurVisibility;					// ������� ���������
	TTime				timeBlinkInterval;				// ����� ������ ��������
	TTime				timeStartBlinking;				// ����� ������ ��������� ��������� �������
	TTime				timeBlinking;					// ����� ��������
	TTime				timeLastBlink;					// ����� ���������� ������������ ������ ��������
	TTime				timeBlinkIntervalCurrent;		// ������� ����� ������ ��������
	bool				bBlinking;						// ������ � ��������� ��������

	// ��������� ����� ���������
	TTime				timeStartRestore;				// ����� ������ �������������� (����� ����������� ��� ������� ��������������, ����� �������� ������ ����� ������ ����� ���������)
	TTime				timeRestoreInterval;			// ����� ��������������

	TTime				timeStartInvisible;
	TTime				timeInvisibleInterval;
	TTime				timeInvisibilityMin;
	TTime				timeInvisibilityMax;
		
	TTime				timeCurrent;					// ������� �����

public:
				CBloodsuckerVisibility	();
	
		void	Load					(LPCTSTR section);
		bool	Switch					(bool bVis);	// ���������� true, ���� ������������ �������
		bool	Update					();				// ���������� ����� ��������� ���������
		
	IC	bool	IsCurrentVisible		() {return bCurVisibility;}
	IC	bool	IsInvisibilityReady		() {return (timeStartRestore + timeRestoreInterval < timeCurrent);}	
	
	// Temp 
	IC	bool	IsActiveBlinking		() {if (timeStartBlinking + timeBlinking > timeCurrent) return true; else return false;}
};



