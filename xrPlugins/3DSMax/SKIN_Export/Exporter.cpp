// Exporter.cpp: implementation of the CExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XRaySkinExp.h"
#include "xrSKIN_BUILD.h"
#include "Exporter.h"

// ================================================== FindPhysiqueModifier()
// Find if a given node contains a Physique Modifier
// DerivedObjectPtr requires you include "modstack.h" from the MAX SDK
Modifier* FindPhysiqueModifier (INode* nodePtr)
{
    // Get object from node. Abort if no object.
    Object* ObjectPtr = nodePtr->GetObjectRef();
    

    if ( NULL == ObjectPtr)
    {
        return NULL;
    }

    // Is derived object ?
    if (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        // Yes -> Cast.
        IDerivedObject* DerivedObjectPtr = static_cast<IDerivedObject*>(ObjectPtr);

        // Iterate over all entries of the modifier stack.
        int ModStackIndex = 0;
        while (ModStackIndex < DerivedObjectPtr->NumModifiers())
        {
            // Get current modifier.
            Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);
            Class_ID clsid = ModifierPtr->ClassID();
            // Is this Physique ?
            if (ModifierPtr->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
            {
                // Yes -> Exit.
                return ModifierPtr;
            }

            // Next modifier stack entry.
            ModStackIndex++;
        }
    }

    // Not found.
    return NULL;
}

// ================================================== GetTriObjectFromObjRef
// Return a pointer to a TriObject given an object reference or return NULL
// if the node cannot be converted to a TriObject
TriObject *GetTriObjectFromObjRef
	(
	Object* pObj, 
	BOOL *pbDeleteIt
	) 
{
	TriObject *pTri;

	assert(pObj != NULL);
	assert(pbDeleteIt != NULL);

    *pbDeleteIt = FALSE;

    if (pObj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
	{ 
        pTri = (TriObject *) pObj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));

        // Note that the TriObject should only be deleted
        // if the pointer to it is not equal to the object
        // pointer that called ConvertToType()
        if (pObj != pTri) 
			*pbDeleteIt = TRUE;

        return pTri;
    } 
	else 
	{
        return NULL;
    }
}

// ================================================== IsExportableMesh()
BOOL IsExportableMesh(INode* pNode, Object* &pObj)
{
    ULONG superClassID;

    assert(pNode);
    pObj = pNode->GetObjectRef();

    if (pObj == NULL)
    {
        return FALSE;
    }

    superClassID = pObj->SuperClassID();
    //find out if mesh is renderable

    if( !pNode->Renderable() || pNode->IsNodeHidden())
    {
        return FALSE;
    }

    BOOL bFoundGeomObject = FALSE;
    //find out if mesh is renderable (more)
    switch(superClassID)
    {
    case GEN_DERIVOB_CLASS_ID:

        do
        {
            pObj = ((IDerivedObject*)pObj)->GetObjRef();
            superClassID = pObj->SuperClassID();
        }
        while( superClassID == GEN_DERIVOB_CLASS_ID );

        switch(superClassID)
        {
        case GEOMOBJECT_CLASS_ID:
            bFoundGeomObject = TRUE;
            break;
        }
        break;

    case GEOMOBJECT_CLASS_ID:
        bFoundGeomObject = TRUE;

    default:
        break;
    }

    return bFoundGeomObject;
}

// ================================================== dummyFn()
// used by 3DS progress bar
DWORD WINAPI dummyFn(LPVOID arg){
    return(0);
}

VOID CExporter::CAPTURE(INode *pNode){
	Modifier*			pPhysique;
    IPhysiqueExport*	pExport;
    IPhyContextExport*	pContext;
	Object*				pObject;

    Matrix3				matMesh;
    Matrix3				matOffset;
    Matrix3				matZero;

	R_ASSERT			(pNode);

	Log					("NODE: ",pNode->GetName());

	if (isBone(pNode))	g_all_bones.push_back(pNode);

	if (!pNode->Selected() || !IsExportableMesh(pNode,pObject)){
		// process sub-nodes
		int	cChildren = pNode->NumberOfChildren();
		for (int iChild = 0; iChild < cChildren; iChild++){
			// enumerate the child
			CAPTURE(pNode->GetChildNode(iChild));
	    }
		return;
	}
	Log("***** Begin");

	R_ASSERT(!bCaptured);
	bCaptured = TRUE;

	// Get export interface
    pPhysique	= FindPhysiqueModifier(pNode);
	R_ASSERT(pPhysique);
    pExport		= (IPhysiqueExport *)pPhysique->GetInterface(I_PHYINTERFACE);
	R_ASSERT(pExport);
	
	// Get mesh initial transform (used to mult by the bone matrices)
	int rval = CGINTM(pExport->GetInitNodeTM(pNode, matMesh));
	Matrix3		matMesh2 = pNode->GetNodeTM(0);
	Matrix3		matMesh3 = pNode->GetObjTMAfterWSM(0);
    matZero.Zero();
	if (rval || matMesh.Equals(matZero, 0.0)){
		ERR("Old CS version. Can't export mesh");
		matMesh.IdentityMatrix();
	}

	// For a given Object's INode get a
    // ModContext Interface from the Physique Export Interface:
    pContext = (IPhyContextExport *)pExport->GetContextInterface(pNode);
	R_ASSERT(pContext);

	// convert to rigid with blending
    pContext->ConvertToRigid(TRUE);
    pContext->AllowBlending	(FALSE);

	// process vertices
	int	numVertices = pContext->GetNumberVertices();
    for (int iVertex = 0; iVertex < numVertices; iVertex++ ){
		IPhyVertexExport *pVertexExport = (IPhyVertexExport *)pContext->GetVertexInterface(iVertex);
		R_ASSERT(pVertexExport);
		
		// What kind of vertices are these?
        int iVertexType = pVertexExport->GetVertexType();
        pContext->ReleaseVertexInterface( pVertexExport );
		
		R_ASSERT(iVertexType == RIGID_TYPE);
		
		IPhyRigidVertex *	pRigidVertexExport;
		pRigidVertexExport = (IPhyRigidVertex *)pContext->GetVertexInterface(iVertex);
		R_ASSERT(pRigidVertexExport);
		
		// get bone and create vertex
		CVertexDef* pVertex = AddVertex();
		pVertex->SetBone	(AddBone(pRigidVertexExport->GetNode(),matMesh,pExport));
		pVertex->SetOffset	(pRigidVertexExport->GetOffsetVector());
		pContext->ReleaseVertexInterface( pRigidVertexExport );
	}

	// Process mesh
    // Get object from node. Abort if no object.
	BOOL		bDeleteTriObject;
	R_ASSERT	(pObject);
    TriObject *	pTriObject	= GetTriObjectFromObjRef(pObject, &bDeleteTriObject);
	R_ASSERT	(pTriObject);
    Mesh&		M = pTriObject->mesh;

	// Vertices
	DWORD		maxBPV = 0;
	{
		// check match with
		int iNumVert = M.getNumVerts();
		if (!(iNumVert==numVertices && iNumVert==g_vertices.size()))
		{
			Log("Non attached vertices found");
			R_ASSERT(0);
		}

		// for all
		for (int i=0; i<iNumVert; i++)
		{
			Point3 P = M.getVert(i);
			Point3 T = VectorTransform(matMesh,P);
			g_vertices[i]->SetPosition(P);
		}
	}

	// Materials
	Mtl *pMtlMain = pNode->GetMtl();
	R_ASSERT(pMtlMain);

	DWORD cSubMaterials=pMtlMain->NumSubMtls();
	if (cSubMaterials < 1) {
		// Count the material itself as a submaterial.
		cSubMaterials = 1;
	}
	if( pMtlMain->ClassID() == Class_ID(MULTI_CLASS_ID,0) ){
		MultiMtl *mmtl = (MultiMtl*)pMtlMain;
		
		for (DWORD i=0; i<mmtl->NumSubMtls(); i++){
			if (mmtl->GetSubMtl(i)==NULL){
				mat.push_back("");
				continue;
			} 
			if( mmtl->GetSubMtl(i)->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
				StdMat2 *smtl = (StdMat2*) mmtl->GetSubMtl(i);
				char temp[256]; temp[0]=0;
				if (!GetTName(smtl,ID_DI,temp)){ ERR("Can't get material"); }
				mat.push_back(string(temp));
			} else { ERR("Bad sub material"); }
		}
	}else if( pMtlMain->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
		StdMat2 *smtl = (StdMat2*)pMtlMain;
		char temp[256]; temp[0]=0;
		if (!GetTName(smtl,ID_DI,temp)){ ERR("Can't get material"); }
		mat.push_back(string(temp));
	}else{ ERR("Unknown material");	}
	R_ASSERT(mat.size()==cSubMaterials);
	for (int mt=0; mt<mat.size(); mt++) Log("*material*",mat[mt].c_str());
	
	// build normals
	M.buildRenderNormals();

	// our Faces and Vertices
	{
		for (int i=0; i<M.getNumFaces(); i++){
			Face*	gF	= M.faces  + i;
			TVFace*	tF	= M.tvFace + i;

			int m_id = gF->getMatID();
			if (cSubMaterials == 1){
				m_id = 0;
			}else{
				// SDK recommends mod'ing the material ID by the valid # of materials, 
				// as sometimes a material number that's too high is returned.
				m_id %= cSubMaterials;
			}

			static int remap[3] = { 0, 2, 1	};

			st_FACE*	nF	= new st_FACE;
			nF->m		= m_id;
			for (int VVV=0; VVV<3; VVV++){
				int vert_id = gF->v[remap[VVV]];

				CVertexDef	&D	= *(g_vertices[vert_id]);
				Point3		&UV	= M.tVerts[tF->t[remap[VVV]]];

				st_VERT*	v	= new st_VERT; 
				v->P.set		(D.P); 
				v->O.set		(D.O);
				v->SetUV		(UV.x,1-UV.y);
				v->SetBone		(D.bone);
				v->sm_group		= gF->getSmGroup(); // smooth group
				verts.push_back	(v);

				nF->VSet(VVV,v);
			}
			faces.push_back(nF);
		}
	}

	// Add hierrarhy parts that has no effect on vertices, 
	// but required for hierrarhy stability
	{
		for (int i=0; i<g_all_bones.size(); i++)
			AddBone(g_all_bones[i], matMesh, pExport);
	}

	if (bDeleteTriObject)	delete pTriObject;
};

BOOL BFloatsEqual(float f1, float f2){
    // first do a bitwise compare
    if ((*(DWORD*)&f1) == (*(DWORD*)&f2))
        return TRUE;

    // next do an epsilon compare
    float fDiff = (f1 - f2);
    return (fDiff <= 1e-6f) && (fDiff >= -1e-6f);
}

BOOL BEqualMatrices(float *rgfMat1, float *rgfMat2){
    DWORD iFloat;

    for (iFloat = 0; iFloat < 16; iFloat++)
    {
        if (!BFloatsEqual(rgfMat1[iFloat], rgfMat2[iFloat]))
            return FALSE;
    }

    return TRUE;
}

extern int startup(CExporter* E, Interface *pInterface, char* fnTOSAVE );
extern void finalize();

// ================================================== CDataSave::CDataSave()
// -------------------------------------------------------------------------------
//   class      CFindRootNode
//
//   devnote    Helper class for FindRootNode, used to implement callback function
//                  for scene traversal.  Finds the root node by aborting the search
//                  after the first node is returned.  It takes the first node and
//                  walks to the root from there.
//
class CFindRootNode : public ITreeEnumProc
{
public:
    CFindRootNode()
        :m_pNodeRoot(NULL) {}

    virtual int callback(INode *pNode)
    {
        INode *pNodeCur = pNode;

        while (!pNodeCur->IsRootNode())
        {
            pNodeCur = pNodeCur->GetParentNode();
        }
        m_pNodeRoot = pNodeCur;

        return TREE_ABORT;
    }

    INode *m_pNodeRoot;
};

// -------------------------------------------------------------------------------
//  function    FindRootNode
//
//   devnote    Finds the root node of the scene by aborting a tree walk after the first node
//
//   returns    S_OK if a node is found, E_FAIL if not
//
HRESULT FindRootNode(IScene *pScene, INode **ppNode){
    CFindRootNode RootNode;

    // grab the first node of the scene graph
    pScene->EnumTree(&RootNode);

    *ppNode = RootNode.m_pNodeRoot;

    return RootNode.m_pNodeRoot != NULL ? S_OK : E_FAIL;
}

HRESULT ExportSkinFile(	const TCHAR *szFilename, ExpInterface *pExportInterface, Interface *pInterface, 
						BOOL bSuppressPrompts, BOOL bSaveSelection, HWND hwndParent){
    R_ASSERT		(szFilename && pExportInterface && pInterface);

    INode			*pRootNode;
    FindRootNode	(pExportInterface->theScene, &pRootNode);
	R_ASSERT		(pRootNode);

	CExporter		*E = new CExporter;
	E->CAPTURE		(pRootNode);
	startup			(E, pInterface,LPSTR(szFilename));
	delete			E;

	// final cleanup
	finalize		();

    return			S_OK;
}
