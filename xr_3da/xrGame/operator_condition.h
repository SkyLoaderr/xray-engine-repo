////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_condition.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator condition
////////////////////////////////////////////////////////////////////////////

#pragma once

#define USE_HASH

#ifdef USE_HASH
class CRandom32 {
private:
	u32			m_seed;

public:
	IC	u32		seed	()
	{
		return	(m_seed);
	}

	IC	void	seed	(u32 seed)
	{
		m_seed	= seed;
	}

	IC	u32		random	(u32 range)
	{
		m_seed	= 0x08088405*m_seed + 1;
		return	(u32(u64(m_seed)*u64(range) >> 32));
	}
};

extern CRandom32 rand32;
#endif

template <
	typename _condition_type,
	typename _value_type
>
class COperatorConditionAbstract {
protected:
	typedef COperatorConditionAbstract<_condition_type,_value_type>	COperatorCondition;
protected:
	_condition_type		m_condition;
	_value_type			m_value;
#ifdef USE_HASH
	u32					m_hash;
#endif

public:
	IC								COperatorConditionAbstract	(const _condition_type condition, const _value_type value);
	virtual							~COperatorConditionAbstract	();
	IC		const _condition_type	&condition					() const;
	IC		const _value_type		&value						() const;
#ifdef USE_HASH
	IC		const u32				&hash_value					() const;
#endif
	IC		bool					operator<					(const COperatorCondition &condition) const;
	IC		bool					operator==					(const COperatorCondition &condition) const;
};

#include "operator_condition_inline.h"