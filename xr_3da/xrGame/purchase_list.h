////////////////////////////////////////////////////////////////////////////
//	Module 		: purchase_list.h
//	Created 	: 12.01.2006
//  Modified 	: 12.01.2006
//	Author		: Dmitriy Iassenev
//	Description : purchase list class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "associative_vector.h"

class CInventoryOwner;

class CPurchaseList {
public:
	typedef associative_vector<shared_str,float>	DEFICITS;

private:
	DEFICITS				m_deficits;


public:
			void			process			(const CInifile &config, LPCSTR section, CInventoryOwner *owner);

public:
	IC		const float		&deficit		(const shared_str &section) const;
	IC		const DEFICITS	&deficits		() const;
};

#include "purchase_list_inline.h"