#pragma once

class CBloodsuckerVisibility {
	bool				flagVisibility;					// true - если видим
	TTime				timeBlinkInterval;				// врем€ кванта мерцани€
	TTime				timeStartBlinking;				// врем€ начала изменени€ видимости монстра
	TTime				timeBlinking;					// врем€ мерцани€
	bool				bCurVisibility;					// текуща€ видимость

	TTime				timeLastBlink;					// врем€ последнего переключени€ кванта мерцани€
	TTime				timeBlinkIntervalCurrent;		// текущее врем€ кванта мерцани€
public:
				CBloodsuckerVisibility	();
	
		void	Load					(LPCTSTR section);
		void	Switch					(bool bVis);
		bool	Update					();				// возвращает новое состо€ние видимости
	IC	bool	IsVisible				() {return bCurVisibility;}
		
	// Temp 
	IC	bool	IsActive				() {if (timeStartBlinking + timeBlinking > Level().timeServer()) return true; else return false;}
};

