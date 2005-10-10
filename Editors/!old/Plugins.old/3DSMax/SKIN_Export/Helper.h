//----------------------------------------------------------------------------//
// Helper.h                                                                   //
// Copyright (C) 2001 Bruno 'Beosil' Heidelberger                             //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef HELPER_H
#define HELPER_H

//----------------------------------------------------------------------------//
// Defines                                                                    //
//----------------------------------------------------------------------------//

// this is the class for all biped controllers except the root and the footsteps
#define BIPSLAVE_CONTROL_CLASS_ID Class_ID(0x9154,0)
// this is the class for the center of mass, biped root controller ("Bip01")
#define BIPBODY_CONTROL_CLASS_ID  Class_ID(0x9156,0) 

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class Helper
{
// constructors/destructor
protected:
	Helper();
	virtual ~Helper();

// member functions
public:
	static void		ConvertEulerToQuaternion(Point3& euler, D3DXQUATERNION& quaternion);
	static Matrix3	GetBoneTM(INode *pNode, TimeValue t);
	static bool		IsBipedBone(INode *pNode);
	static bool		IsBone(INode *pNode);
	static bool		IsMesh(INode *pNode);
	static void		SetBipedUniform(INode *pNode, BOOL bUniform, BOOL bFigure);
};

#endif

//----------------------------------------------------------------------------//
