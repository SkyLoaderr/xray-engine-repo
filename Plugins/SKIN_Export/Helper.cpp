//----------------------------------------------------------------------------//
// Helper.cpp                                                                 //
// Copyright (C) 2001 Bruno 'Beosil' Heidelberger                             //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "Helper.h"

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

Helper::Helper()
{
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

Helper::~Helper()
{
}

//----------------------------------------------------------------------------//
// Convert euler angles to a quaternion                                       //
//----------------------------------------------------------------------------//

void Helper::ConvertEulerToQuaternion(Point3& euler, D3DXQUATERNION& quaternion)
{
	D3DXQuaternionRotationYawPitchRoll(
		&quaternion, euler.y,euler.x,euler.z);
}

//----------------------------------------------------------------------------//
// Get the transformation matrix of a bone node                               //
//----------------------------------------------------------------------------//
Matrix3 Helper::GetBoneTM(INode *pNode, TimeValue t)
{
	// get node transformation
	Matrix3 tm;
	tm = pNode->GetNodeTM(t);

	// make transformation uniform
	tm.NoScale();

	return tm;
}

//----------------------------------------------------------------------------//
// Check if the given node is a biped bone                                    //
//----------------------------------------------------------------------------//
bool Helper::IsBipedBone(INode *pNode)
{
	// check for invalid and root nodes
	if((pNode == 0) || pNode->IsRootNode()) return false;

	// check for biped nodes
	Control *pControl;
	pControl = pNode->GetTMController();
	if((pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || (pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)) return true;

	return false;
}

//----------------------------------------------------------------------------//
// Check if the given node is a bone                                          //
//----------------------------------------------------------------------------//
bool Helper::IsBone	(INode *pNode)
{
	// check for invalid and root nodes
	if((pNode == 0) || pNode->IsRootNode())	return false;

	// check for bone and dummy nodes
	ObjectState os = pNode->EvalWorldState(0);
	if(os.obj->ClassID() == Class_ID(BONE_CLASS_ID, 0))		return true;
	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))	return false;

	// check for biped nodes
	Control *pControl;
	pControl = pNode->GetTMController();
	if((pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || (pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)) return true;

	return false;
}

//----------------------------------------------------------------------------//
// Check if the given node is a mesh                                          //
//----------------------------------------------------------------------------//
bool Helper::IsMesh(INode *pNode)
{
	// check for invalid and root nodes
	if((pNode == 0) || pNode->IsRootNode()) return false;

	// check for mesh
	ObjectState os;
	os = pNode->EvalWorldState(0);
	if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID) return true;

	return false;
}

//----------------------------------------------------------------------------//
// Set/Unset biped uniform scale                                              //
//----------------------------------------------------------------------------//

void Helper::SetBipedUniform(INode *pNode, BOOL bUniform, BOOL bFigure)
{
	if(IsBipedBone(pNode))
	{
		// get the TM controller of the node
		Control *pControl;
		pControl = pNode->GetTMController();

		// get the biped export interface
		IBipedExport *pBipedExport;
		pBipedExport = (IBipedExport *)pControl->GetInterface(I_BIPINTERFACE);

		// remove/add uniform scale
		pBipedExport->RemoveNonUniformScale			(bUniform);
		if (bFigure) pBipedExport->BeginFigureMode	(0);
		else		 pBipedExport->EndFigureMode	(0);

		// notify all dependents
		Control *pMasterControl;
		pMasterControl = (Control *)pControl->GetInterface(I_MASTER);
		pMasterControl->NotifyDependents(FOREVER, PART_TM, REFMSG_CHANGE);
		pControl->ReleaseInterface(I_MASTER, pMasterControl);

		// release the biped export interface
		pControl->ReleaseInterface(I_BIPINTERFACE, pBipedExport);
	}
}
