#pragma once

const DWORD BONE_NONE =	0xffffffff;

#include "face.h"

extern string convert_space(string input);

class CBoneDef
{
public:
	string	name;
	INode*	pBone;
	Matrix3	matOffset;
	Matrix3 matInit;

	CBoneDef(INode* pNode)
	{
		pBone	= pNode;
		name	= convert_space(string(pBone->GetName()));
	}
	BOOL	isEqual(INode* pNode)
	{
		return	pBone==pNode;
	}
};

DEF_VECTOR(vMAT,string);

extern vVERT	verts;
extern vFACE	faces;
extern vMAT		mat;
