////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_container_types.h
//	Created 	: 22.10.2004
//  Modified 	: 22.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Types for smart container
////////////////////////////////////////////////////////////////////////////

#pragma once

struct CPositionComponent {
	float  x, y, z; // position

	IC	bool	operator==			(const CPositionComponent &v) const
	{
		return	(
			(x == v.x) &&
			(y == v.y) &&
			(z == v.z)
		);
	}
};
vertex_type_add(CPositionComponent)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(CPositionComponent)

struct CDiffuseComponent {
	DWORD  diffuse; //  diffuse vertex color

	IC	bool operator== (const CDiffuseComponent &v) const
	{
		return	(
			(diffuse == v.diffuse)
		);
	}
};
vertex_type_add(CDiffuseComponent)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(CDiffuseComponent)

struct CFirstTCComponent {
	float  u, v;    //  1st texture coordinate set

	IC	bool operator== (const CFirstTCComponent &t) const
	{
		return	(
			(u == t.u) &&
			(v == t.v)
		);
	}
};
vertex_type_add(CFirstTCComponent)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(CFirstTCComponent)

struct CSecondTCComponent {
	float  u2, v2;      //  2nd texture coordinate set

	IC	bool operator== (const CSecondTCComponent &t) const
	{
		return	(
			(u2 == t.u2) &&
			(v2 == t.v2)
		);
	}
};
vertex_type_add(CSecondTCComponent)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(CSecondTCComponent)

struct CNormalComponent {
	float  nx, ny, nz;  // normal

	IC	bool operator== (const CNormalComponent &t) const
	{
		return	(
			(nx == t.nx) &&
			(ny == t.ny) &&
			(nz == t.nz)
		);
	}
};
vertex_type_add(CNormalComponent)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(CNormalComponent)

struct VertexA : 
	public CPositionComponent,
	public CDiffuseComponent,
	public CFirstTCComponent
{
	IC			VertexA	() {}
	IC			VertexA	(float _x, float _y, float _z) {x = _x; y = _y; z = _z;}
	IC	bool operator== (const VertexA &t) const
	{
		return	(
			((CPositionComponent&)*this == t) &&
			((CDiffuseComponent&)*this == t) &&
			((CFirstTCComponent&)*this == t)
		);
	}
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
	IC			VertexB	() {}
	IC			VertexB	(float _x, float _y, float _z) {x = _x; y = _y; z = _z;}
	IC	bool operator== (const VertexB &t) const
	{
		return	(
			((CPositionComponent&)*this == t) &&
			((CNormalComponent&)*this == t) &&
			((CFirstTCComponent&)*this == t) &&
			((CSecondTCComponent&)*this == t)
		);
	}
};
vertex_type_add(VertexB)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexB)

struct VertexC : public VertexB
{
	IC			VertexC	() {}
	IC			VertexC	(float _x, float _y, float _z) {x = _x; y = _y; z = _z;}
};
vertex_type_add(VertexC)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexC)

struct VertexD : public VertexB
{
	IC			VertexD	() {}
	IC			VertexD	(float _x, float _y, float _z) {x = _x; y = _y; z = _z;}
};
vertex_type_add(VertexD)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexD)

struct VertexE : public VertexB
{
	IC			VertexE	() {}
	IC			VertexE	(float _x, float _y, float _z) {x = _x; y = _y; z = _z;}
};
vertex_type_add(VertexE)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexE)

struct VertexF : public VertexB
{
	IC			VertexF	() {}
	IC			VertexF	(float _x, float _y, float _z) {x = _x; y = _y; z = _z;}
};
vertex_type_add(VertexF)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexF)
