////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_registry_wrappers.h
//	Created 	: 20.10.2004
//  Modified 	: 20.10.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife registry wrappers
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_registry_container.h"
#include "alife_registry_wrapper.h"

typedef CALifeRegistryWrapper<CInfoPortionRegistry> KNOWN_INFO_REGISTRY;

//������ ��������� ������� � ������� �����������
typedef CALifeRegistryWrapper<CKnownContactsRegistry> KNOWN_CONTACTS_REGISTRY;

//������ ������ ������������, � ������� ����� �����
typedef CALifeRegistryWrapper<CEncyclopediaRegistry> ENCYCLOPEDIA_REGISTRY;

//������ �������, ���������� �������
typedef CALifeRegistryWrapper<CGameTaskRegistry> GAME_TASK_REGISTRY;

//������ ��������, ���������� �������
typedef CALifeRegistryWrapper<CGameNewsRegistry> GAME_NEWS_REGISTRY;


template <typename T>
class CALifeRegistryWrapperObject {
	T				*m_registry;

public:
	IC				CALifeRegistryWrapperObject		()
	{
		m_registry	= xr_new<T>();
	}

	virtual			~CALifeRegistryWrapperObject	()
	{
		xr_delete	(m_registry);
	}

	IC		T		&registry						() const
	{
		VERIFY		(m_registry);
		return		(*m_registry);
	}
};

class CKnownContactsRegistryWrapper :	public CALifeRegistryWrapperObject<KNOWN_CONTACTS_REGISTRY> {};
class CEncyclopediaRegistryWrapper :	public CALifeRegistryWrapperObject<ENCYCLOPEDIA_REGISTRY> {};
class CGameTaskRegistryWrapper :		public CALifeRegistryWrapperObject<GAME_TASK_REGISTRY> {};
class CGameNewsRegistryWrapper :		public CALifeRegistryWrapperObject<GAME_NEWS_REGISTRY> {};
class CInfoPortionWrapper :				public CALifeRegistryWrapperObject<KNOWN_INFO_REGISTRY> {};
class CRelationRegistryWrapper :		public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CRelationRegistry> > {};
class CMapLocationWrapper :				public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CMapLocationRegistry> > {};
class CFogOfWarWrapper :				public CALifeRegistryWrapperObject<CALifeRegistryWrapper<CFogOfWarRegistry> > {};
