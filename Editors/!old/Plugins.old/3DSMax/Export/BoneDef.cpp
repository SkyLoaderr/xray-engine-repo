#include "stdafx.h"
#include "BoneDef.h"
#include "Exporter.h"
#include "MeshExpUtility.h"

CBoneDef::CBoneDef(INode* pNode)	
{
	parent	= 0;
	pBone	= pNode;
	name	= Helper::ConvertSpace(string(pBone->GetName()));

	matInit.identity();
	matOffset.identity();
}
LPCSTR CBoneDef::GetParentName()
{
	INode* node=pBone->GetParentNode();
	if (Helper::IsBone(node,U.m_SkinAllowDummy)) return node->GetName();
	return 0;
}
BOOL CBoneDef::SetInitTM(IPhysiqueExport* pExport, const Matrix3& matMesh)
{
	BOOL bErr = false;
	R_ASSERT(pBone);
	R_ASSERT(Helper::IsBone(pBone,U.m_SkinAllowDummy));
	Matrix3 tmp;
	if(Helper::IsBipedBone(pBone))	{
		Helper::SetBipedUniform(pBone, TRUE, TRUE);
		bErr = CGINTM(pBone,pExport->GetInitNodeTM(pBone, tmp));
		if (bErr) tmp.IdentityMatrix();
		Helper::SetBipedUniform(pBone, FALSE, FALSE);
	} else {
		bErr = CGINTM(pBone,pExport->GetInitNodeTM(pBone, tmp));
		if (bErr) tmp.IdentityMatrix();
	}

	if (!bErr){
		Helper::ConvertMatrix(tmp,matInit);
		matOffset.invert(matInit);
		//S matMesh
//		pBone->matOffset = matMesh * Inverse(pBone->matInit);
	}

	return !bErr;
}
