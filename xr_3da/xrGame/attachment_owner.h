////////////////////////////////////////////////////////////////////////////
//	Module 		: attachment_owner.h
//	Created 	: 12.02.2004
//  Modified 	: 12.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Attachment owner
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAttachableItem;
class CInventoryItem;

class CAttachmentOwner {
protected:
	xr_vector<ref_str>			m_attach_item_sections;
	xr_vector<CAttachableItem*>	m_attached_objects;
public:
	IC						CAttachmentOwner	();
	virtual					~CAttachmentOwner	();
	virtual	void			reinit				();
	virtual	void			reload				(LPCSTR section);
	virtual void			renderable_Render	();
			void			attach				(CInventoryItem *inventory_item);
			void			detach				(CInventoryItem *inventory_item);
	virtual	bool			can_attach			(const CInventoryItem *inventory_item) const;
			bool			attached			(const CInventoryItem *inventory_item) const;
	IC		const xr_vector<CAttachableItem*> &attached_objects	() const;
};

#include "attachment_owner_inline.h"
