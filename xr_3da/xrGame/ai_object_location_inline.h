////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_object_location.h
//	Created 	: 27.11.2003
//  Modified 	: 27.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI object location
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CAI_ObjectLocation::CAI_ObjectLocation()
{
	Init				();
}

IC	CAI_ObjectLocation::~CAI_ObjectLocation()
{
}

IC	void CAI_ObjectLocation::Init()
{
	ai().level_graph().set_invalid_vertex	(m_dwLevelVertexID,m_tpLevelVertex);
	ai().game_graph().set_invalid_vertex	(m_tGameVertexID);
}