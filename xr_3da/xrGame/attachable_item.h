////////////////////////////////////////////////////////////////////////////
//	Module 		: attachable_item.h
//	Created 	: 11.02.2004
//  Modified 	: 11.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Attachable item
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item.h"

class CAttachableItem : public CInventoryItem
{
	typedef CInventoryItem inherited;
protected:
	LPCSTR					m_bone_name;
	Fmatrix					m_offset;
public:
	IC						CAttachableItem		();
	virtual					~CAttachableItem	();
	IC		void			init				();
	virtual	void			reinit				();
	virtual	void			Load				(LPCSTR section);
	virtual	void			reload				(LPCSTR section);
	virtual void			OnH_A_Chield		();
	IC		LPCSTR			bone_name			() const;
	IC		const Fmatrix	&offset				() const;
};

#include "attachable_item_inline.h"