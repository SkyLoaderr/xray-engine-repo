////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluators.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker property evaluators classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wrapper_abstract.h"
#include "property_evaluator_const.h"
#include "property_evaluator_member.h"

class CAI_Stalker;

typedef CWrapperAbstract2<CAI_Stalker,CPropertyEvaluator>		CStalkerPropertyEvaluator;
typedef CWrapperAbstract2<CAI_Stalker,CPropertyEvaluatorConst>	CStalkerPropertyEvaluatorConst;
typedef CWrapperAbstract2<CAI_Stalker,CPropertyEvaluatorMember>	CStalkerPropertyEvaluatorMember;

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorALife
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorALife : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAlive
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorAlive : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItems
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorItems : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemies
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemies : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSeeEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorSeeEnemy : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorItemToKill : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemCanKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorItemCanKill : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorFoundItemToKill : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundAmmo
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorFoundAmmo : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorReadyToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorReadyToKill : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSafeToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorSafeToKill : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

protected:
	u32					m_last_cover_time;

public:
						CStalkerPropertyEvaluatorSafeToKill	(CPropertyStorage *storage);
	virtual _value_type	evaluate							();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAnomaly
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorAnomaly : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate							();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorInsideAnomaly
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorInsideAnomaly : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate							();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorPanic
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorPanic : public CStalkerPropertyEvaluator {
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	virtual _value_type	evaluate							();
};

#include "stalker_property_evaluators_inline.h"