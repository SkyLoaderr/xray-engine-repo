// Exporter.h: interface for the CExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTER_H__9EB217E3_917D_4286_97F6_DE0C32EF38E9__INCLUDED_)
#define AFX_EXPORTER_H__9EB217E3_917D_4286_97F6_DE0C32EF38E9__INCLUDED_
#pragma once

#include "helper.h"

inline void ERR(const char *s) { Log("---------- Error: ",s);}
IC int CGINTM(int r)
{
	char* msg = 0;
	switch (r) {
	case MATRIX_RETURNED:		return r;
	case NODE_NOT_FOUND:		msg = "NODE_NOT_FOUND"; break;
	case NO_MATRIX_SAVED:		msg = "NO_MATRIX_SAVED"; break;
	case INVALID_MOD_POINTER:	msg = "INVALID_MOD_POINTER"; break;
	}

	if (msg) {
		Msg("*** ERROR: GetInitNodeTM failed (%s)",msg);
	}
	return r;
}

//-----------------------------------------------------------------------------
class CVertexDef
{
public:
	Fvector			P;
	Fvector			O;
	DWORD			bone;
	
	void SetPosition(Point3 &p)
	{	P.set		(p.x,p.z,p.y);	}
	void SetOffset	(Point3 &p)
	{	O.set		(p.x,p.z,p.y);	}
	void SetBone	(DWORD B)
	{	bone = B;	}
};

//-----------------------------------------------------------------------------
class CExporter  
{
public:
	BOOL bCaptured;
	std::vector<CBoneDef*>		g_bones;
	std::vector<INode*>			g_all_bones;
	std::vector<CVertexDef*>	g_vertices;
	
private:
	//-----------------------------------------------------------------------------
	CVertexDef* AddVertex()
	{
		CVertexDef* V = new CVertexDef;
		g_vertices.push_back(V);
		return V;
	}
	//-----------------------------------------------------------------------------
	void ComputeInitialTM(IPhysiqueExport* pExport, INode* pNode, Matrix3& tm)
	{
		R_ASSERT(isBone(pNode));
		Matrix3 T = pNode->GetNodeTM(0);

		if(Helper::IsBipedBone(pNode))	{
			Helper::SetBipedUniform(pNode, TRUE, TRUE);
			// tm = pNode->GetNodeTM(0);
			
			int rval = CGINTM(pExport->GetInitNodeTM(pNode, tm));
			if (rval) {
				ERR(pNode->GetName());
				tm.IdentityMatrix();
			}
			Helper::SetBipedUniform(pNode, FALSE, FALSE);
		} else {
			int rval = CGINTM(pExport->GetInitNodeTM(pNode, tm));
			if (rval) {
				ERR(pNode->GetName());
				tm.IdentityMatrix();
			} 
		}
	}
	DWORD	AddBone(INode* pNode, Matrix3 &matMesh, IPhysiqueExport* pExport)
	{
		R_ASSERT(isBone(pNode));
		
		for (DWORD I=0; I<g_bones.size(); I++) {
			if (g_bones[I]->isEqual(pNode)) return I;
		}
		
		CBoneDef*	pBone= new CBoneDef(pNode);
		ComputeInitialTM(pExport, pNode, pBone->matInit);
		pBone->matOffset = matMesh * Inverse(pBone->matInit);
		g_bones.push_back	(pBone);
		
		return g_bones.size()-1;
	}
	//-----------------------------------------------------------------------------
	BOOL isBone(INode* pNode)
	{
		if (0==pNode)	return FALSE;
		Object* obj = pNode->EvalWorldState(0).obj;
		if (0==obj)		return FALSE;
		
		const DWORD BIP_BONE_CLASS_ID =	0x00009125;
		if( (obj->ClassID() == Class_ID(BONE_CLASS_ID,0)) || 
			(obj->ClassID() == Class_ID(BIP_BONE_CLASS_ID, 0)) )
		{
			Control *pControl = pNode->GetTMController();
			if (
				/* (pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID) || */
				(pControl->ClassID() == FOOTPRINT_CLASS_ID)) return FALSE;

			return TRUE;
		}
		return FALSE;
	}
	//-----------------------------------------------------------------------------
	BOOL GetTName(StdMat2 *_Mstd, DWORD _Tid, char *name) {
		if( !_Mstd->MapEnabled( _Tid ) )				return FALSE;
		Texmap *map = 0;
		if(0==(map = _Mstd->GetSubTexmap(_Tid)) )		return FALSE;
		if(map->ClassID() != Class_ID(BMTEX_CLASS_ID,0))return FALSE;
		BitmapTex *bmap = (BitmapTex*)map;
		_splitpath( bmap->GetMapName(), 0, 0, name, 0 );
		return TRUE;
	}
	//-----------------------------------------------------------------------------
public:
	VOID CAPTURE(INode *pNode);
	
	CExporter() {
		bCaptured = FALSE;
	};
};

#endif // !defined(AFX_EXPORTER_H__9EB217E3_917D_4286_97F6_DE0C32EF38E9__INCLUDED_)
