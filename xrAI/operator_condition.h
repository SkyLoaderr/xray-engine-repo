////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_condition.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator condition
////////////////////////////////////////////////////////////////////////////

#pragma once

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
	u32					m_hash;

public:
	IC								COperatorConditionAbstract	(const _condition_type condition, const _value_type value);
	virtual							~COperatorConditionAbstract	();
	IC		const _condition_type	&condition					() const;
	IC		const _value_type		&value						() const;
	IC		const u32				&hash_value					() const;
	IC		bool					operator<					(const COperatorCondition &condition) const;
	IC		bool					operator==					(const COperatorCondition &condition) const;
};

#include "operator_condition_inline.h"