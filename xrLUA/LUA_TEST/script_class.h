#pragma once

#include <typelist.h>

#define add_to_type_list(type)	typedef Loki::TL::Append<script_type_list,type>::Result TypeList_##type;
#define save_type_list(type)	TypeList_##type

#define script_type_list		Loki::NullType

