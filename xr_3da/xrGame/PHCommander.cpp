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

bool CPHCall::equal(CPHReqComparerV* cmp_condition,CPHReqComparerV* cmp_action)
{
	return m_action->compare(cmp_action)&&m_condition->compare(cmp_condition);
}
bool CPHCall::is_any(CPHReqComparerV* v)
{
	return m_action->compare(v)||m_condition->compare(v);
}


CPHCommander::~CPHCommander()
{
	clear();
}

void CPHCommander::add_call(CPHCondition* condition,CPHAction* action)
{
	m_calls.push_back(xr_new<CPHCall>(condition,action));
}

void delete_call(CPHCall* &call)
{
	try{
		xr_delete(call);
	}
	catch(...)
	{
		call=NULL;
	}
}

void CPHCommander::remove_call(PHCALL_I i)
{
	delete_call(*i);
	m_calls.erase(i);
}



void CPHCommander::clear	()
{
	while (m_calls.size())	{
		remove_call(m_calls.end()-1);
	}
}
void CPHCommander::update	()
{
	for(u32 i=0; i<m_calls.size(); i++)
	{
		try
		{
			m_calls[i]->check();
		} 
		catch(...)
		{
			remove_call(m_calls.begin()+i);
			i--;
			continue;
		}

		if(m_calls[i]->obsolete())
		{
			remove_call(m_calls.begin()+i);
			i--;
			continue;
		}
	}
}
struct SFEqualPred
{
	CPHReqComparerV* cmp_condition,*cmp_action;
	SFEqualPred(CPHReqComparerV* cmp_c,CPHReqComparerV* cmp_a)
	{
		cmp_condition=cmp_c;cmp_action=cmp_a;
	}
	bool operator()(CPHCall* call)
	{
		return	call->equal(cmp_condition,cmp_action);
	}
};
PHCALL_I CPHCommander::find_call(CPHReqComparerV* cmp_condition,CPHReqComparerV* cmp_action)
{
	return std::find_if( m_calls.begin(),m_calls.end(),SFEqualPred(cmp_condition,cmp_action));
}

void CPHCommander::remove_call(CPHReqComparerV* cmp_condition,CPHReqComparerV* cmp_action)
{
	PHCALL_I i=find_call(cmp_condition,cmp_action);
	if(i!=m_calls.end())remove_call(i);
}

void CPHCommander::add_call_unique(CPHCondition* condition,CPHReqComparerV* cmp_condition,CPHAction* action,CPHReqComparerV* cmp_action)
{
	if(m_calls.end()==find_call(cmp_condition,cmp_action))
	{
		add_call(condition,action);
	}
}
struct SRemoveRped
{
	CPHReqComparerV*	cmp_object;
	SRemoveRped(CPHReqComparerV* cmp_o)
	{
		cmp_object=cmp_o;
	}
	bool operator() (CPHCall* call)
	{
		if(call->is_any(cmp_object))
		{
			delete_call(call);
			return true;
		}
		else
			return false;
	}
};
void CPHCommander::remove_calls(CPHReqComparerV* cmp_object)
{
	remove_if(m_calls.begin(),m_calls.end(),SRemoveRped(cmp_object));
}