#pragma once

class CPHScriptCondition;
class CPHScriptAction;
class CPHScriptObjectCondition;
class CPHScriptObjectAction;
class CPHScriptObjectConditionN;
class CPHScriptObjectActionN;

class CPHReqComparerV
{
	public: 
		virtual		bool compare(CPHScriptCondition* v)			{return false;}
		virtual		bool compare(CPHScriptAction* v)			{return false;}
		virtual		bool compare(CPHScriptObjectCondition* v)	{return false;}
		virtual		bool compare(CPHScriptObjectAction* v)		{return false;}
		virtual		bool compare(CPHScriptObjectConditionN* v)	{return false;}
		virtual		bool compare(CPHScriptObjectActionN* v)		{return false;}
};