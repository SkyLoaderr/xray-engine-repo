#ifndef PH_COMMANDER_H
#define PH_COMMANDER_H

class CPHCondition
{
public:
	virtual bool 			is_true							()		=0				;
	virtual bool 			obsolete						()		=0				;
};

class CPHAction 
{
public:
	virtual void 			run								()		=0				;
	virtual bool 			obsolete						()		=0				;
};


class CPHCall
{
	CPHAction*		m_action			;
	CPHCondition*	m_condition			;
public:
					CPHCall							(CPHCondition* condition,CPHAction* action)	;
					~CPHCall						()											;
	void 			check							()											;
	bool 			obsolete						()											;
};

DEFINE_VECTOR(CPHCall*,PHCALL_STORAGE,PHCALL_I);
class CPHCommander
{
	
	PHCALL_STORAGE	m_calls;
public:
						~CPHCommander				()											;
	void				add_call					(CPHCondition* condition,CPHAction* action)	;
	void				remove_call					(PHCALL_I i);
	void				update  					()											;
	void				clear						()											;
};
#endif