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
	ref_str					m_bone_name;
	Fmatrix					m_offset;
	u16						m_bone_id;
public:
	IC						CAttachableItem		();
	virtual					~CAttachableItem	();
	IC		void			init				();
	virtual	void			reinit				();
	virtual	void			Load				(LPCSTR section);
	virtual	void			reload				(LPCSTR section);
	virtual void			OnH_A_Chield		();
	virtual void			renderable_Render	();
	IC		ref_str			bone_name			() const;
	IC		u16				bone_id				() const;
	IC		void			set_bone_id			(u16 bone_id);
	IC		const Fmatrix	&offset				() const;
};

#include "attachable_item_inline.h"