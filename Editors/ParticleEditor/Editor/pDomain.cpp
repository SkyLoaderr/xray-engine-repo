#include "stdafx.h"
#pragma hdrstop

#include "pDomain.h"
//#include "mmath.h"
//#include "basetypes.h"

PDomain::PDomain(PDomainEnum t,	float inA0,	float inA1,	float inA2,	
								float inA3,	float inA4,	float inA5,
								float inA6,	float inA7,	float inA8	)
{
	updated = true;
	type = t;
	a0 = inA0;
	a1 = inA1;
	a2 = inA2;
	a3 = inA3;
	a4 = inA4;
	a5 = inA5;
	a6 = inA6;
	a7 = inA7;
	a8 = inA8;
	SetFloatNames();
}

PDomain::~PDomain()
{
}

//--------------------------------------------------------------------
PDomain::PDomain(PDomain& inDomain)
//	: QObject()
{
	updated = true;
	type = inDomain.Type();
	a0 = *(inDomain.PFloatA0());
	a1 = *(inDomain.PFloatA1());
	a2 = *(inDomain.PFloatA2());
	a3 = *(inDomain.PFloatA3());
	a4 = *(inDomain.PFloatA4());
	a5 = *(inDomain.PFloatA5());
	a6 = *(inDomain.PFloatA6());
	a7 = *(inDomain.PFloatA7());
	a8 = *(inDomain.PFloatA8());
	SetFloatNames();
}

PDomain& PDomain::operator = (PDomain& inDomain)
{
	updated = true;
	type = inDomain.Type();
	a0 = *(inDomain.PFloatA0());
	a1 = *(inDomain.PFloatA1());
	a2 = *(inDomain.PFloatA2());
	a3 = *(inDomain.PFloatA3());
	a4 = *(inDomain.PFloatA4());
	a5 = *(inDomain.PFloatA5());
	a6 = *(inDomain.PFloatA6());
	a7 = *(inDomain.PFloatA7());
	a8 = *(inDomain.PFloatA8());
	SetFloatNames();
	return *this;
}

PDomain& PDomain::operator = (PDomainEnum in)
{
	updated = true;
	type = in;
	a0 = 0.0;
	a1 = 0.0;
	a2 = 0.0;
	a3 = 0.0;
	a4 = 0.0;
	a5 = 0.0;
	a6 = 0.0;
	a7 = 0.0;
	a8 = 0.0;
	SetFloatNames();
	return *this;
}
	
	
//--------------------------------------------------------------------
void	PDomain::set(PDomainEnum t, 
					float inA0, float inA1, float inA2,
					float inA3, float inA4, float inA5, 
					float inA6, float inA7, float inA8	)
{
	updated = true;
	type = t;
	a0 = inA0;
	a1 = inA1;
	a2 = inA2;
	a3 = inA3;
	a4 = inA4;
	a5 = inA5;
	a6 = inA6;
	a7 = inA7;
	a8 = inA8;
	SetFloatNames();
}
//--------------------------------------------------------------------
PDomainEnum	PDomain::SetType(PDomainEnum in)
{
	updated = true;
	type = in;
	SetFloatNames();
	return type;
}

float	PDomain::SetA0(float in)
{
	updated = true;
	return a0 = in;
}
float	PDomain::SetA1(float in)
{
	updated = true;
	return a1 = in;
}
float	PDomain::SetA2(float in)
{
	updated = true;
	return a2 = in;
}

float	PDomain::SetA3(float in)
{
	updated = true;
	return a3 = in;
}
float	PDomain::SetA4(float in)
{
	updated = true;
	return a4 = in;
}
float	PDomain::SetA5(float in)
{
	updated = true;
	return a5 = in;
}
float	PDomain::SetA6(float in)
{
	updated = true;
	return a6 = in;
}
float	PDomain::SetA7(float in)
{
	updated = true;
	return a7 = in;
}
float	PDomain::SetA8(float in)
{
	updated = true;
	return a8 = in;
}
//--------------------------------------------------------------------
void	PDomain::SetA0(PFloat& in)  {	updated = true;	a0 = in;  }
void	PDomain::SetA1(PFloat& in)  {	updated = true;	a1 = in;  }
void	PDomain::SetA2(PFloat& in)  {	updated = true;	a2 = in;  }
void	PDomain::SetA3(PFloat& in)  {	updated = true;	a3 = in;  }
void	PDomain::SetA4(PFloat& in)  {	updated = true;	a4 = in;  }
void	PDomain::SetA5(PFloat& in)  {	updated = true;	a5 = in;  }
void	PDomain::SetA6(PFloat& in)  {	updated = true;	a6 = in;  }
void	PDomain::SetA7(PFloat& in)  {	updated = true;	a7 = in;  }
void	PDomain::SetA8(PFloat& in)  {	updated = true;	a8 = in;  }


//--------------------------------------------------------------------
void	PDomain::SetFloatNames()
{
	switch(type)
	{
		case PDPoint:
			numFloats_v = 3;
			typeName = "point- ";
			a0Name = "x";			
			a1Name = "y";			
			a2Name = "z";			
			a3Name = "NO_NAME";
			a4Name = "NO_NAME";
			a5Name = "NO_NAME";
			a6Name = "NO_NAME";
			a7Name = "NO_NAME";
			a8Name = "NO_NAME";
			break;
		case PDLine:
			numFloats_v = 6;
			typeName = "line- ";
			a0Name = "x1";		
			a1Name = "y1";		
			a2Name = "z1";		
			a3Name = "x2";		
			a4Name = "y2";		
			a5Name = "z2";		
			a6Name = "NO_NAME";
			a7Name = "NO_NAME";
			a8Name = "NO_NAME";
			break;
		case PDTriangle:
			numFloats_v = 9;
			typeName = "triangle- ";
			a0Name = "x1";		
			a1Name = "y1";		
			a2Name = "z1";		
			a3Name = "x2";		
			a4Name = "y2";		
			a5Name = "z2";		
			a6Name = "x3";		
			a7Name = "y3";		
			a8Name = "z3";		
			break;
		case PDPlane:
			numFloats_v = 9;
			typeName = "plane- ";
			a0Name = "ox";		
			a1Name = "oy";		
			a2Name = "oz";		
			a3Name = "Ux";		
			a4Name = "Uy";		
			a5Name = "Uz";		
			a6Name = "Vx";		
			a7Name = "Vy";		
			a8Name = "Vz";		
			break;
		case PDBox:
			numFloats_v = 6;
			typeName = "box- ";
			a0Name = "x1";		
			a1Name = "y1";		
			a2Name = "z1";		
			a3Name = "x2";		
			a4Name = "y2";		
			a5Name = "z2";		
			a6Name = "NO_NAME";
			a7Name = "NO_NAME";
			a8Name = "NO_NAME";
			break;
		case PDSphere:
			numFloats_v = 5;
			typeName = "sphere- ";
			a0Name = "x";			
			a1Name = "y";			
			a2Name = "z";			
			a3Name = "radius1";	
			a4Name = "radius2";	
			a5Name = "NO_NAME";
			a6Name = "NO_NAME";
			a7Name = "NO_NAME";
			a8Name = "NO_NAME";
			break;
		case PDCylinder:
			numFloats_v = 8;
			typeName = "cylinder- ";
			a0Name = "x1";		
			a1Name = "y1";		
			a2Name = "z1";		
			a3Name = "x2";		
			a4Name = "y2";		
			a5Name = "z2";		
			a6Name = "radius1";	
			a7Name = "radius2";	
			a8Name = "NO_NAME";
			break;
		case PDCone:
			numFloats_v = 8;
			typeName = "cone- ";
			a0Name = "x1";		
			a1Name = "y1";		
			a2Name = "z1";		
			a3Name = "x2";		
			a4Name = "y2";		
			a5Name = "z2";		
			a6Name = "radius1";	
			a7Name = "radius2";	
			a8Name = "NO_NAME";
			break;
		case PDBlob:
			numFloats_v = 4;
			typeName = "blob- ";
			a0Name = "x";			
			a1Name = "y";			
			a2Name = "z";			
			a3Name = "stdev";		
			a4Name = "NO_NAME";
			a5Name = "NO_NAME";
			a6Name = "NO_NAME";
			a7Name = "NO_NAME";
			a8Name = "NO_NAME";
			break;
		case PDDisc:
			numFloats_v = 8;
			typeName = "disk- ";
			a0Name = "x";			
			a1Name = "y";			
			a2Name = "z";			
			a3Name = "normal x";		
			a4Name = "normal y";
			a5Name = "normal z";
			a6Name = "radius 1";
			a7Name = "radius 2";
			a8Name = "NO_NAME";
			break;
		default:
			numFloats_v = 9;
			typeName = "transform- ";
			a0Name = "trans x";		
			a1Name = "trans y";		
			a2Name = "trans z";		
			a3Name = "rotate x";		
			a4Name = "rotate y";		
			a5Name = "rotate z";		
			a6Name = "scale x";	
			a7Name = "scale y";	
			a8Name = "scale z";
			break;
	}
}

//--------------------------------------------------------------------
void	PDomain::MoveXYZ(float x, float y, float z)
{
	switch(type)
	{
		case PDPoint:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDLine:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDTriangle:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			a6+=x;	a7+=y;	a8+=z;
			break;
		case PDPlane:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDBox:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDSphere:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDCylinder:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDCone:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDBlob:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDDisc:
			a0+=x;	a1+=y;	a2+=z;
			break;
	}
	
}

//----------------------------------------------------------
void	PDomain::RotateXYZ	(float x, float y, float z)
{
	MVertex	v1;
	MVertex v2;
	MVertex v3;
	MVertex	r1;
	MVertex r2;
	MVertex r3;
//	MVertex	tmpV;
	MMatrix	trans;
	
	MVertex	c;
	
	trans.setIdentity();
	trans.rotateX(x);
	trans.rotateZ(z);
	trans.rotateY(y);
//	float	xloc;
//	float	yloc;
//	float	zloc;
	switch(type)
	{
		case PDPoint:
			break;
		case PDLine:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			break;
		case PDTriangle:
			c = GetCenter();
			r1 = trans*(v1.set( (float)a0-c.X(), (float)a1-c.Y(), (float)a2-c.Z() ));
			r2 = trans*(v2.set( (float)a3-c.X(), (float)a4-c.Y(), (float)a5-c.Z() ));
			r3 = trans*(v3.set( (float)a6-c.X(), (float)a7-c.Y(), (float)a8-c.Z() ));
			a0 = r1.X()+c.X();	a1 = r1.Y()+c.Y();   a2 = r1.Z()+c.Z();
			a3 = r2.X()+c.X();	a4 = r2.Y()+c.Y();   a5 = r2.Z()+c.Z();
			a6 = r3.X()+c.X();	a7 = r3.Y()+c.Y();   a8 = r3.Z()+c.Z();
			break;
		case PDPlane:
//			r2 = trans*(v2.set( a3, a4, a5 ));
//			r3 = trans*(v3.set( a6, a7, a8 ));
//			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
//			a6 = r3.X();	a7 = r3.Y();	a8 = r3.Z();
			
			c = GetCenter();
			v1.set(a0-c.X(), a1-c.Y(), a2-c.Z());
			v2.set((float)a0+((float)a3/2.0), (float)a1+((float)a4/2.0), (float)a2+((float)a5/2.0));
			v3.set((float)a0+((float)a6/2.0), (float)a1+((float)a7/2.0), (float)a2+((float)a8/2.0));
			
			r1 = trans*v1;
			r2 = trans*v2;
			r3 = trans*v3;
			
			a0 = r1.X()+c.X();	a1 = r1.Y()+c.Y();   a2 = r1.Z()+c.Z();
			
			
			break;
		case PDBox:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
			break;
		case PDSphere:
			r1 = trans*(v1.set( a0, a1, a2 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			break;
		case PDCylinder:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
			break;
		case PDCone:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
			break;
		case PDBlob:
			r1 = trans*(v1.set( a0, a1, a2 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			break;
		case PDDisc:
			break;
	}
}
//----------------------------------------------------------
void	PDomain::ScaleXYZ	(float x, float y, float z)
{
	x = x;
	y = y;
	z = z;
	
	switch(type)
	{
		case PDPoint:
			break;
		case PDLine:
			break;
		case PDTriangle:
			break;
		case PDPlane:
			break;
		case PDBox:
			break;
		case PDSphere:
			break;
		case PDCylinder:
			break;
		case PDCone:
			break;
		case PDBlob:
			break;
		case PDDisc:
			break;
	}
}

//----------------------------------------------------------
MVertex&	PDomain::GetCenter()
{
	static MVertex center;
	MVertex	v1;
	MVertex	v2;
	MVertex	v3;
	MVertex	v4;
	
	switch(type)
	{
		case PDPoint:
			center.set(a0, a1, a2);
			break;
		case PDLine:
			break;
		case PDTriangle:
			center.set( ((float)a0+(float)a3+(float)a6)/3.0, 
						((float)a1+(float)a4+(float)a7)/3.0, 
						((float)a2+(float)a5+(float)a8)/3.0 );
			break;
		case PDPlane:
			v1.set(a0, a1, a2);
			v2.set(a0+(a3/2.0f), a1+(a4/2.0f), a2+(a5/2.0f));
			v3.set(a0+(a6/2.0f), a1+(a7/2.0f), a2+(a8/2.0f));
			v4.set(a0+(a6/2.0f)+(a3/2.0f), a1+(a7/2.0f)+(a4/2.0f), a2+(a8/2.0f)+(a5/2.0f));
			center.set(	(v1.X()+v2.X()+v3.X()+v4.X())/4.0,
						(v1.Y()+v2.Y()+v3.Y()+v4.Y())/4.0,
						(v1.Z()+v2.Z()+v3.Z()+v4.Z())/4.0 );
			break;
		case PDBox:
			break;
		case PDSphere:
			break;
		case PDCylinder:
			break;
		case PDCone:
			break;
		case PDBlob:
			break;
		case PDDisc:
			center.set(a0, a1, a2);
			break;
	}
	return center;
}
