////////////////////////////////////////////////////////////////////////////
//	Module 		: attachment_owner.cpp
//	Created 	: 12.02.2004
//  Modified 	: 12.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Attachment owner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "attachment_owner.h"
#include "attachable_item.h"
#include "../skeletoncustom.h"

struct CStringPredicate {
	bool		operator()	(const shared_str &s1, const shared_str &s2) const
	{
		return			(s1 < s2);
	}
};

CAttachmentOwner::~CAttachmentOwner()
{
}

void CAttachmentOwner::reload				(LPCSTR section)
{
	if (!pSettings->line_exist(section,"attachable_items")) {
		m_attach_item_sections.clear();
		return;
	}

	LPCSTR						attached_sections = pSettings->r_string(section,"attachable_items");
	u32							item_count = _GetItemCount(attached_sections);
	string16					current_item_section;
	m_attach_item_sections.resize(item_count);
	for (u32 i=0; i<item_count; ++i)
		m_attach_item_sections[i] = _GetItem(attached_sections,i,current_item_section);

	std::sort					(m_attach_item_sections.begin(),m_attach_item_sections.end(),CStringPredicate());
}

void CAttachmentOwner::reinit	()
{
	VERIFY						(m_attached_objects.empty());
}

void CAttachmentOwner::net_Destroy()
{
	while (!attached_objects().empty())
		detach					(*attached_objects().begin());
}

void CAttachmentOwner::renderable_Render		()
{
	xr_vector<CAttachableItem*>::iterator	I = m_attached_objects.begin();
	xr_vector<CAttachableItem*>::iterator	E = m_attached_objects.end();
	for ( ; I != E; ++I)
		(*I)->renderable_Render();
}

void __stdcall AttachmentCallback(CKinematics *tpKinematics)
{
	CGameObject				*game_object = smart_cast<CGameObject*>(static_cast<CObject*>(tpKinematics->Update_Callback_Param));
	VERIFY					(game_object);
	
	CAttachmentOwner		*attachment_owner = smart_cast<CAttachmentOwner*>(game_object);
	VERIFY					(attachment_owner);

	CKinematics				*kinematics = smart_cast<CKinematics*>(game_object->Visual());

	xr_vector<CAttachableItem*>::const_iterator	I = attachment_owner->attached_objects().begin();
	xr_vector<CAttachableItem*>::const_iterator	E = attachment_owner->attached_objects().end();
	for ( ; I != E; ++I) {
		(*I)->XFORM().mul_43	(kinematics->LL_GetBoneInstance((*I)->bone_id()).mTransform,(*I)->offset());
		(*I)->XFORM().mulA_43	(game_object->XFORM());
	}
}

void CAttachmentOwner::attach(CInventoryItem *inventory_item)
{
	xr_vector<CAttachableItem*>::const_iterator	I = m_attached_objects.begin();
	xr_vector<CAttachableItem*>::const_iterator	E = m_attached_objects.end();
	for ( ; I != E; ++I) {
		if( (*I)->ID() != inventory_item->object().ID() )
			return; //already attached, fake, I'll repair It
//		VERIFY								((*I)->ID() != inventory_item->object().ID());
	}

	if (can_attach(inventory_item)) {
		CAttachableItem						*attachable_item = smart_cast<CAttachableItem*>(inventory_item);
		VERIFY								(attachable_item);
		CGameObject							*game_object = smart_cast<CGameObject*>(this);
		VERIFY								(game_object && game_object->Visual());
		if (m_attached_objects.empty())
			game_object->add_visual_callback(AttachmentCallback);
		attachable_item->set_bone_id		(smart_cast<CKinematics*>(game_object->Visual())->LL_BoneID(attachable_item->bone_name()));
		m_attached_objects.push_back		(smart_cast<CAttachableItem*>(inventory_item));

		inventory_item->object().setVisible	(true);
		attachable_item->afterAttach();
	}
}

void CAttachmentOwner::detach(CInventoryItem *inventory_item)
{
	xr_vector<CAttachableItem*>::iterator	I = m_attached_objects.begin();
	xr_vector<CAttachableItem*>::iterator	E = m_attached_objects.end();
	for ( ; I != E; ++I) {
		if ((*I)->ID() == inventory_item->object().ID()) {
			m_attached_objects.erase	(I);
			(*I)->afterDetach();
			if (m_attached_objects.empty()) {
				CGameObject					*game_object = smart_cast<CGameObject*>(this);
				VERIFY						(game_object && game_object->Visual());
				game_object->remove_visual_callback(AttachmentCallback);
				
				inventory_item->object().setVisible	(false);

			}
			break;
		}
	}
}

bool CAttachmentOwner::attached				(const CInventoryItem *inventory_item) const
{
	return (attachedItem(inventory_item->object().ID())!= NULL);
/*
	xr_vector<CAttachableItem*>::const_iterator	I = m_attached_objects.begin();
	xr_vector<CAttachableItem*>::const_iterator	E = m_attached_objects.end();
	for ( ; I != E; ++I)
		if ((*I)->ID() == inventory_item->object().ID())
			return		(true);
	return				(false);
*/
}

bool  CAttachmentOwner::attached			(shared_str sect_name) const
{
	return (attachedItem(sect_name)!= NULL);
/*
	xr_vector<CAttachableItem*>::const_iterator	I = m_attached_objects.begin();
	xr_vector<CAttachableItem*>::const_iterator	E = m_attached_objects.end();
	for ( ; I != E; ++I)
		if (!xr_strcmp((*I)->cNameSect(), sect_name) && !(*I)->m_drop)
			return		(true);
	return				(false);
*/
}

bool CAttachmentOwner::can_attach			(const CInventoryItem *inventory_item) const
{
	const CAttachableItem	*item = smart_cast<const CAttachableItem*>(inventory_item);
	if (!item || !item->enabled() || !item->can_be_attached())
		return			(false);

	//����� �� ������������ ������� ������ ����
	if(!(std::binary_search(m_attach_item_sections.begin(),m_attach_item_sections.end(),inventory_item->object().cNameSect(),CStringPredicate())))
		return false;
	//���� ��� ���� �������������� ����� ������ ���� 
	if(attached(inventory_item->object().cNameSect()))
		return false;

	return true;
}

void CAttachmentOwner::reattach_items		()
{
	CGameObject							*game_object = smart_cast<CGameObject*>(this);
	VERIFY								(game_object && game_object->Visual());

	xr_vector<CAttachableItem*>::const_iterator	I = m_attached_objects.begin();
	xr_vector<CAttachableItem*>::const_iterator	E = m_attached_objects.end();
	for ( ; I != E; ++I) {
		CAttachableItem* attachable_item = *I;
		VERIFY (attachable_item);
		attachable_item->set_bone_id		(smart_cast<CKinematics*>(game_object->Visual())->LL_BoneID(attachable_item->bone_name()));
	}
	//---------------------------------------------------------------------------
	if (m_attached_objects.empty())
		game_object->SetKinematicsCallback(false);
	else
		game_object->SetKinematicsCallback(true);
}

CAttachableItem* CAttachmentOwner::attachedItem			(CLASS_ID clsid) const
{
	xr_vector<CAttachableItem*>::const_iterator	I = m_attached_objects.begin();
	xr_vector<CAttachableItem*>::const_iterator	E = m_attached_objects.end();
	for ( ; I != E; ++I)
		if ((*I)->CLS_ID == clsid)
			return (*I);

	return NULL;

}

CAttachableItem* CAttachmentOwner::attachedItem			(u16 id) const
{
	xr_vector<CAttachableItem*>::const_iterator	I = m_attached_objects.begin();
	xr_vector<CAttachableItem*>::const_iterator	E = m_attached_objects.end();
	for ( ; I != E; ++I)
		if ((*I)->ID() == id)
			return (*I);

	return NULL;
}

CAttachableItem* CAttachmentOwner::attachedItem			(shared_str& section) const
{
	xr_vector<CAttachableItem*>::const_iterator	I = m_attached_objects.begin();
	xr_vector<CAttachableItem*>::const_iterator	E = m_attached_objects.end();
	for ( ; I != E; ++I)
		if (!xr_strcmp((*I)->cNameSect(), section) && !(*I)->m_drop)
			return		(*I);

	return				NULL;

}
