#include "stdafx.h"
#include "xrCDB.h"

using namespace CDB;
using namespace Opcode;

// Detects if this box intersect other
IC	bool	intersect(const _fbox& box )
{
	if( max.x < box.min.x )	return false;
	if( max.y < box.min.y )	return false;
	if( max.z < box.min.z )	return false;
	if( min.x > box.max.x )	return false;
	if( min.y > box.max.y )	return false;
	if( min.z > box.max.z )	return false;
	return true;
};

void COLLIDER::box_query(const MODEL *m_def, const Fvector& b_center, const Fvector& b_dim)
{
}
