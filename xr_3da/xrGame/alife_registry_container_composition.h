////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_registry_container_composition.h
//	Created 	: 01.07.2004
//  Modified 	: 01.07.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife registry container class, composition of the registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_abstract_registry.h"
#include "InfoPortionDefs.h"
#include "PdaMsg.h"
#include "encyclopedia_article_defs.h"
#include "character_info_defs.h"


#pragma warning(push)
#pragma warning(disable:4005)


//��� ���� ����������, �� ������ ����������, ������� ��� ������
typedef CALifeAbstractRegistry<u16, KNOWN_INFO_VECTOR > CInfoPortionRegistry;
add_to_registry_type_list(CInfoPortionRegistry);
#define info_portions define_constant(CInfoPortionRegistry) 
#define registry_type_list save_registry_type_list(CInfoPortionRegistry)

//��� ���� ����������, ��������� � ������� �����������
typedef CALifeAbstractRegistry<u16, RELATION_MAP > CRelationRegistry;
add_to_registry_type_list(CRelationRegistry);
#define character_relations define_constant(CRelationRegistry) 
#define registry_type_list save_registry_type_list(CRelationRegistry)


//��� �������, ������ ���������� � �������� ���� ���������
typedef CALifeAbstractRegistry<u16, TALK_CONTACT_VECTOR > CKnownContactsRegistry;
add_to_registry_type_list(CKnownContactsRegistry);
#define known_contacts define_constant(CKnownContactsRegistry) 
#define registry_type_list save_registry_type_list(CKnownContactsRegistry)

//������ ������ ������������, ������� ����� �����
typedef CALifeAbstractRegistry<u16, ARTICLE_VECTOR > CEncyclopediaRegistry;
add_to_registry_type_list(CEncyclopediaRegistry);
#define encyclopedia_articles define_constant(CEncyclopediaRegistry) 
#define registry_type_list save_registry_type_list(CEncyclopediaRegistry)


#pragma warning(pop)