#include "stdafx.h"
#include "PHCommander.h"

CPHCall::CPHCall(CPHCondition* condition,CPHAction* action)	
{
	m_action=action;
	m_condition=condition;
}

CPHCall::~CPHCall()
{
	xr_delete(m_action);
	xr_delete(m_condition);
}
bool CPHCall::obsolete()
{
	return m_action->obsolete()||m_condition->obsolete();
}

void CPHCall::check()
{
	if(m_condition->is_true())m_action->run();
}


CPHCommander::~CPHCommander()
{
	clear();
}

void CPHCommander::add_call(CPHCondition* condition,CPHAction* action)
{
	m_calls.push_back(xr_new<CPHCall>(condition,action));
}

void CPHCommander::remove_call(PHCALL_I i)
{
	try{
		xr_delete(*i);
		}
	catch(...)
	{
		(*i)=NULL;
	}
	m_calls.erase(i);
}
void CPHCommander::clear()
{
	PHCALL_I	i=m_calls.begin(),e=m_calls.end();
	for(;e!=i;++i)
	{
	 remove_call(i);
	}
	m_calls.clear();
}
void CPHCommander::update()
{
	PHCALL_I	i=m_calls.begin();
	for(;m_calls.end()!=i;)
	{
		try
		{
			(*i)->check();
		} 
		catch(...)
		{
			remove_call(i);
			continue;
		}

		if((*i)->obsolete())remove_call(i);
		++i;
	}
}