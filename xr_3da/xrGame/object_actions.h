////////////////////////////////////////////////////////////////////////////
//	Module 		: object_actions.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object actions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CAI_Stalker;
class CInventoryItem;

//////////////////////////////////////////////////////////////////////////
// CObjectActionBase
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
class CObjectActionBase : public CActionBase<CAI_Stalker> {
protected:
	typedef CActionBase<CAI_Stalker>				inherited;
	typedef CGraphEngine::_solver_condition_type	_condition_type;
	typedef CGraphEngine::_solver_value_type		_value_type;

protected:
	_item_type			*m_item;

public:
	IC					CObjectActionBase	(_item_type *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name = "");
	virtual void		initialize			();
	IC		void		set_property		(_condition_type condition_id, _value_type value);
};

typedef CObjectActionBase<CGameObject> CSObjectActionBase;

//////////////////////////////////////////////////////////////////////////
// CObjectActionMember
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
class CObjectActionMember : public CObjectActionBase<_item_type> {
protected:
	typedef CObjectActionBase<_item_type>			inherited;

protected:
	_condition_type		m_condition_id;
	_value_type			m_value;

public:
	IC					CObjectActionMember	(_item_type *item, CAI_Stalker *owner, CPropertyStorage *storage, _condition_type condition_id, _value_type value, LPCSTR action_name = "");
	virtual void		execute				();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionCommand
//////////////////////////////////////////////////////////////////////////

class CObjectActionCommand : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

protected:
	u32					m_command;

public:
						CObjectActionCommand(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 command, LPCSTR action_name = "");
	virtual void		initialize			();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionShow
//////////////////////////////////////////////////////////////////////////

class CObjectActionShow : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

public:
						CObjectActionShow	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name = "");
	virtual void		initialize			();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionHide
//////////////////////////////////////////////////////////////////////////

class CObjectActionHide : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

public:
						CObjectActionHide	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name = "");
	virtual void		initialize			();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionReload
//////////////////////////////////////////////////////////////////////////

class CObjectActionReload : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

protected:
	u32					m_type;

public:
						CObjectActionReload	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 type, LPCSTR action_name = "");
	virtual void		initialize			();
	virtual void		execute				();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionFire
//////////////////////////////////////////////////////////////////////////

class CObjectActionFire : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

protected:
	u32					m_type;

public:
						CObjectActionFire	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 type, LPCSTR action_name = "");
	virtual void		initialize			();
	virtual void		execute				();
	virtual void		finalize			();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionAim
//////////////////////////////////////////////////////////////////////////

class CObjectActionAim : public CObjectActionMember<CInventoryItem> {
protected:
	typedef CObjectActionMember<CInventoryItem> inherited;

public:
						CObjectActionAim	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, _condition_type condition_id, _value_type value, LPCSTR action_name = "");
	virtual void		initialize			();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionStrapping
//////////////////////////////////////////////////////////////////////////

class CObjectActionStrapping : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

public:
						CObjectActionStrapping	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name = "");
	virtual void		initialize				();
	virtual void		execute					();
	virtual void		finalize				();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionUnstrapping
//////////////////////////////////////////////////////////////////////////

class CObjectActionUnstrapping : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

public:
						CObjectActionUnstrapping(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name = "");
	virtual void		initialize				();
	virtual void		execute					();
	virtual void		finalize				();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionQueueWait
//////////////////////////////////////////////////////////////////////////

class CObjectActionQueueWait : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

protected:
	u32					m_type;

public:
						CObjectActionQueueWait	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 type, LPCSTR action_name = "");
	virtual void		initialize				();
	virtual void		execute					();
	virtual void		finalize				();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionSwitch
//////////////////////////////////////////////////////////////////////////

class CObjectActionSwitch : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

protected:
	u32					m_type;

public:
						CObjectActionSwitch	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 type, LPCSTR action_name = "");
	virtual void		initialize			();
	virtual void		execute				();
	virtual void		finalize			();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionDrop
//////////////////////////////////////////////////////////////////////////

class CObjectActionDrop : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

public:
						CObjectActionDrop	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name = "");
	virtual void		initialize			();
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionThreaten
//////////////////////////////////////////////////////////////////////////

class CObjectActionThreaten : public CObjectActionBase<CAI_Stalker> {
protected:
	typedef CObjectActionBase<CAI_Stalker> inherited;

public:
						CObjectActionThreaten	(CAI_Stalker *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name = "");
	virtual void		execute					();
};

#include "object_actions_inline.h"