/////////////////////////////////////////////////////
// Аномальная зона: "мясорубка"
// При попадании живого объекта в зону происходит 
// электрический разряд
// Зона восстанавливает заряд через определенное время
// (через m_dwPeriod заряжается с 0 до m_fMaxPower)
//
/////////////////////////////////////////////////////
#pragma once
#include "gravizone.h"

class CMincer :	public CGraviZone
{
private:
	typedef CGraviZone inherited;
public:
	CMincer(void);
	virtual ~CMincer(void);

	virtual void Load (LPCSTR section);
};