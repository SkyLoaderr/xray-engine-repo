////////////////////////////////////////////////////////////////////////////
//	Module 		: stack_reserve.h
//	Created 	: 08.12.2004
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Stack class with element reservation
////////////////////////////////////////////////////////////////////////////

#pragma once

template<
	class _Ty,
	class _Container = std::vector<_Ty>
>
struct stack_reserve : public std::stack<_Ty,_Container> {
	IC	void reserve(size_t size) {c.reserve(size);}
};
