#pragma once

class	ENGINE_API	CSheduled
{
public:
	DWORD								shedule_Min;		// minimal bound of update time (sample: 20ms)
	DWORD								shedule_Max;		// maximal bound of update time (sample: 200ms)
	DWORD								shedule_TimeStamp;	// last update global device time
	float								shedule_PMON;		// performance monitor (microseconds)

	CSheduled			();
	virtual ~CSheduled	();

	void								shedule_Register	();
	void								shedule_Unregister	();

	virtual float						shedule_Scale		()			= 0;
	virtual void						Update				(DWORD dt)	{};
	virtual BOOL						Ready				()			= 0; 
	virtual LPCSTR						cName				()			{ return "UNKNOWN"; }; 
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
	vector<Item>	Items;

	IC void			Push	(Item& I);
	IC void			Pop		();
	IC Item&		Top		()
	{
		return Items.front();
	}
public:
	u64				cycles_start;
	u64				cycles_limit;
public:
	void			ProcessStep	();
	void			Process		();
	void			Update		(DWORD mcs		);

	void			Switch		();
	void			Slice		()
	{
		if ((CPU::GetCycleCount()-cycles_start)>cycles_limit)
			Switch();
	}

	void			Register	(CSheduled* A	);
	void			Unregister	(CSheduled* A	);

	void			Initialize	();
	void			Destroy		();
};
