////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_client_server_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item client and server class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef object_item_client_server_inlineH
#define object_item_client_server_inlineH

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _client_type, typename _server_type>
#define CSObjectItemClientServer CObjectItemClientServer<_client_type,_server_type>

TEMPLATE_SPECIALIZATION
IC	CSObjectItemClientServer::CObjectItemClientServer	(const CLASS_ID &clsid, LPCSTR script_clsid) :
	inherited			(clsid,script_clsid)
{
}

#ifndef NO_XR_GAME
TEMPLATE_SPECIALIZATION
ObjectFactory::CLIENT_BASE_CLASS *CSObjectItemClientServer::client_object	() const
{
	return				(xr_new<CLIENT_TYPE>());
}
#endif

TEMPLATE_SPECIALIZATION
ObjectFactory::SERVER_BASE_CLASS *CSObjectItemClientServer::server_object	(LPCSTR section) const
{
	return				(xr_new<SERVER_TYPE>(section));
}

#undef TEMPLATE_SPECIALIZATION
#undef CSObjectItemClientServer

#endif