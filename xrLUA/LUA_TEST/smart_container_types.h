////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_container_types.h
//	Created 	: 22.10.2004
//  Modified 	: 22.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Types for smart container
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <typelist.h>

#define final_vertex_type_list			Loki::NullType
#define vertex_type_add(a)				typedef Loki::Typelist<a,final_vertex_type_list> vertex_type_list_##a;
#define current_vertex_type_list(a)		vertex_type_list_##a

struct CPositionComponent {
	float  x, y, z; // position
};

struct CDiffuseComponent {
	DWORD  diffuse; //  diffuse vertex color
};

struct CFirstTCComponent {
	float  u, v;    //  1st texture coordinate set
};

struct CSecondTCComponent {
	float  u2, v2;      //  2nd texture coordinate set
};

struct CNormalComponent {
	float  nx, ny, nz;  // normal
};

struct VertexA : 
	public CPositionComponent,
	public CDiffuseComponent,
	public CFirstTCComponent
{
};
vertex_type_add(VertexA)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexA)

struct VertexB : 
	public CPositionComponent,
	public CNormalComponent,
	public CFirstTCComponent,
	public CSecondTCComponent
{
};
vertex_type_add(VertexB)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexB)

struct VertexC : public VertexB
{
};
vertex_type_add(VertexC)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexC)

struct VertexD : public VertexB
{
};
vertex_type_add(VertexD)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexD)

struct VertexE : public VertexB
{
};
vertex_type_add(VertexE)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexE)

struct VertexF : public VertexB
{
	IC	VertexF() {}
	IC	VertexF(const VertexF &v)
	{
		*this = v;
	}
};
vertex_type_add(VertexF)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexF)
