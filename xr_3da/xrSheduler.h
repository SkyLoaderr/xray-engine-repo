#pragma once

enum
{
	SHEDULE_LOW		= 0,
	SHEDULE_NORMAL	= 1,
	SHEDULE_HIGH	= 2
};

class	ENGINE_API	CSheduled
{
public:
	DWORD								shedule_Min;		// minimal bound of update time (sample: 20ms)
	DWORD								shedule_Max;		// maximal bound of update time (sample: 200ms)
	DWORD								shedule_TimeStamp;	// last update global device time
	float								shedule_PMON;		// performance monitor (microseconds)

	CSheduled			();
	virtual ~CSheduled	();

	void								shedule_Register	(DWORD priority=SHEDULE_NORMAL);
	void								shedule_Unregister	();

	virtual void						Update				(DWORD dt) {};
	virtual Fvector&					Position			() = 0;
	virtual BOOL						Ready				() = 0; 
};

class	ENGINE_API CSheduler
{
private:
	struct Item
	{
		DWORD		dwTimeForExecute;
		DWORD		dwTimeOfLastExecute;
		CSheduled*	Object;
		DWORD		dwPadding;				// for align-issues

		IC bool		operator < (Item& I)
		{	return dwTimeForExecute > I.dwTimeForExecute; }
	};
private:
	vector<Item>	Items	[3];

	IC void			Push	(DWORD P, Item& I)
	{
		Items.push_back(I);
		push_heap	(Items.begin(), Items.end());
	}
	IC void			Pop		(DWORD P)
	{
		pop_heap	(Items.begin(), Items.end());
		Items.pop_back();
	}
	IC Item&		Top		(DWORD P)
	{
		return Items.front();
	}
	void			UpdateLevel	(DWORD P, DWORD mcs);
public:
	void			Update		();
	void			Register	(CSheduled* A, DWORD priority=SHEDULE_NORMAL);
	void			Unregister	(CSheduled* A);
};
