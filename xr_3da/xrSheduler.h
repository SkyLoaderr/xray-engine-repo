#pragma once

class	ENGINE_API	CSheduled
{
public:
	DWORD								dwMinUpdate;	// minimal bound of update time (sample: 20ms)
	DWORD								dwMaxUpdate;	// maximal bound of update time (sample: 200ms)
	DWORD								dwTimeStamp;	// last update global device time

	CSheduled			();
	virtual ~CSheduled	();

	virtual void						Update			(DWORD dt) {};
	virtual Fvector&					Position		() = 0;
	virtual BOOL						Ready			() = 0; 
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

	IC void			Push	(Item& I)
	{
		Items.push_back(I);
		push_heap	(Items.begin(), Items.end());
	}
	IC void			Pop		()
	{
		pop_heap	(Items.begin(), Items.end());
		Items.pop_back();
	}
	IC Item&		Top		()
	{
		return Items.front();
	}
public:
	void			Update		();
	void			Register	(CSheduled* A);
	void			Unregister	(CSheduled* A);
};
