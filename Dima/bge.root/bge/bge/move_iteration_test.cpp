////////////////////////////////////////////////////////////////////////////
//	Module 		: move_iteration_test.cpp
//	Created 	: 31.01.2005
//  Modified 	: 31.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Move iteration tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ui.h"
#include <emptytype.h>
#include <typeinfo.h>

struct default_quiescent_policy {
	template <typename board>
	bool stable_position(board &b)
	{
		return		(true);
	}

	template <typename board, typename evaluator>
	bool evaluate(board &b, evaluator &e)
	{
		VERIFY2(false,"Default_Aspiration_Policy_Doesnt_Intend_Custom_Evaluation");
	}
};

template <typename _quiescent_policy_type>
struct no_depth_policy {
	_quiescent_policy_type	*m_quiescent_policy;

	IC		no_depth_policy	(_quiescent_policy_type *quiescent_policy)
	{
		VERIFY				(quiescent_policy);
		m_quiescent_policy	= quiescent_policy;
	}

	template <
		typename board, 
		typename evaluator
	>
	IC	typename evaluator::result_type	operator()(board &b, evaluator &e) const
	{
		if (m_quiescent_policy->stable_position(b))
			return	(e.evaluate(b));
		return		(m_quiescent_policy->evaluate(b,e));
	}
};

struct board {};
struct evaluator {
	typedef int result_type;

	template <typename board>
	IC	result_type		evaluate(board &b)
	{
		return	(result_type(0));
	}
};

template <typename _result_type>
struct hash_policy {
	_result_type	m_result;

	template <typename _board_type, typename _depth_type>
	IC	bool operator()	(_board_type &board, const _depth_type &depth, const _result_type &alpha, const _result_type &beta)
	{
		return	(false);
	}

	IC	_result_type	result() const
	{
		return	(m_result);
	}
};

template <typename _result_type>
struct etc_policy {
	_result_type	m_result;

	template <typename _board_type, typename _depth_type>
	IC	bool operator()	(_board_type &board, const _depth_type &depth, const _result_type &alpha, const _result_type &beta)
	{
		return	(false);
	}

	IC	_result_type	result() const
	{
		return	(m_result);
	}
};

template <typename _result_type>
struct mpc_policy {
	_result_type	m_result;

	template <typename _board_type, typename _depth_type>
	IC	bool operator()	(_board_type &board, const _depth_type &depth, const _result_type &alpha, const _result_type &beta)
	{
		return	(false);
	}

	IC	_result_type	result() const
	{
		return	(m_result);
	}
};

template <typename _result_type>
struct eg_policy {
	_result_type	m_result;

	template <typename _board_type, typename _depth_type>
	IC	bool operator()	(_board_type &board, const _depth_type &depth, const _result_type &alpha, const _result_type &beta)
	{
		return	(false);
	}

	IC	_result_type	result() const
	{
		return	(m_result);
	}
};

template <typename _policy_type, typename _condition_type>
struct policy_holder {
	_policy_type		*m_policy;
	_condition_type		*m_condition;

	IC		 policy_holder	()
	{
	}

	IC	void setup		(_policy_type *policy, _condition_type *condition)
	{
		VERIFY			(policy);
		m_policy		= policy;

		VERIFY			(condition);
		m_condition		= condition;
	}

	template <typename _board_type, typename _depth_type, typename _result_type>
	IC	bool operator()	(_board_type &board, const _depth_type &depth, const _result_type &alpha, const _result_type &beta)
	{
		if ((*m_condition)(depth))
			return		((*m_policy)(board,depth,alpha,beta));
		return			(false);
	}
};

typedef int result_type;

struct default_condition {
	template <typename _depth_type>
	IC	bool operator()	(const _depth_type &depth) const
	{
		return			(true);
	}
};

template <typename list, template <typename _1, typename _2> class generator>
struct hierarchy_generator {
	template <typename T>
	struct helper {
		typedef typename T::Head Head;
		typedef typename T::Tail Tail;

		typedef generator<Head,typename helper<Tail>::result> result;
	};

	template <>
	struct helper<Loki::NullType> {
		typedef Loki::EmptyType result;
	};

	typedef typename helper<list>::result result;
};

template <typename _type, typename _base>
struct policy_generator_helper : public _type, public _base {
	using typename _type::setup;
	using typename _base::setup;
	using typename _type::operator();
	using typename _base::operator();
};

template <typename _type>
struct policy_generator_helper<_type,Loki::EmptyType> : public _type {
	using typename _type::setup;
	using typename _type::operator();
};

typedef hash_policy<result_type>	_hash_policy;
typedef etc_policy<result_type>		_etc_policy;
typedef mpc_policy<result_type>		_mpc_policy;
typedef eg_policy<result_type>		_eg_policy;

typedef Loki::Typelist
	<
		policy_holder<_hash_policy,default_condition>,
		Loki::Typelist<
			policy_holder<_etc_policy,default_condition>,
			Loki::Typelist<
				policy_holder<_mpc_policy,default_condition>,
				Loki::Typelist<
					policy_holder<_eg_policy,default_condition>,
					Loki::NullType
				>
			>
		>
	> final_sc_policy;

template <typename policy_list>
struct shortcut_policy_manager : 
	public hierarchy_generator<policy_list,policy_generator_helper>::result
{
//	typedef typename hierarchy_generator<policy_list,policy_generator_helper>::result inherited;
//	
//	using typename inherited::operator();
	
	template <typename _board_type, typename _depth_type, typename _result_type>
	struct iterator {
		template <typename list>
		IC	static bool	process	(shortcut_policy_manager *self, _board_type &board, const _depth_type &depth, const _result_type &alpha, const _result_type &beta)
		{
			typedef typename list::Head Head;
			typedef typename list::Tail Tail;

			if (!(*(Head*)self)(board,depth,alpha,beta))
				return	(iterator<_board_type,_depth_type,_result_type>::process<Tail>(self,board,depth,alpha,beta));
			return		(true);
		}
		
		template <>
		IC	static bool	process<Loki::NullType>	(shortcut_policy_manager *self, _board_type &board, const _depth_type &depth, const _result_type &alpha, const _result_type &beta)
		{
			return		(false);
		}
	};

	template <typename _board_type, typename _depth_type, typename _result_type>
	IC	bool	operator()	(_board_type &board, const _depth_type &depth, const _result_type &alpha, const _result_type &beta)
	{
		return		(iterator<_board_type,_depth_type,_result_type>::process<policy_list>(this,board,depth,alpha,beta));
	}
};

void move_iteration_test()
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	no_depth_policy<default_quiescent_policy>(new default_quiescent_policy())(board(),evaluator());
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	shortcut_policy_manager<final_sc_policy>	shortcuts;
	
	_hash_policy				hash;
	_etc_policy					etc;
	_mpc_policy					mpc;
	_eg_policy					eg;
	default_condition			cond;
	board						b;

	shortcuts.setup				(&hash,&cond);
	shortcuts.setup				(&etc,&cond);
	shortcuts.setup				(&mpc,&cond);
	shortcuts.setup				(&eg,&cond);

	shortcuts					(b,10,-100,100);
}