// PHDynamicData.h: interface for the PHDynamicData class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4995)
#include <ode\ode.h>
#pragma warning(default:4995)
#include "PHInterpolation.h"
#if !defined(AFX_PHDynamicData_H__ACC01646_B581_4639_B78C_30311432021B__INCLUDED_)
#define AFX_PHDynamicData_H__ACC01646_B581_4639_B78C_30311432021B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PHDynamicData  
{
public:
dVector3 pos;
dMatrix3 R;
Fmatrix BoneTransform;
private:
dBodyID body;
CPHInterpolation* p_parent_body_interpolation;
CPHInterpolation body_interpolation;
dGeomID geom;
dGeomID transform;
//PHDynamicData* Childs;
vector<PHDynamicData>  Childs;
unsigned int numOfChilds;
Fmatrix ZeroTransform;
public:
	inline void UpdateInterpolation(){
		body_interpolation.UpdatePositions();
		body_interpolation.UpdateRotations();
	}
	void UpdateInterpolationRecursive()	;
	void InterpolateTransform(Fmatrix& transform);
	void InterpolateTransformVsParent(Fmatrix& transform);
	PHDynamicData& operator [] (unsigned int i) {return Childs[i];};
	void Destroy();
	void Create(unsigned int numOfchilds,dBodyID Body);
	void CalculateData(void);
	PHDynamicData * GetChild(unsigned int ChildNum);
	bool SetChild(unsigned int ChildNum,unsigned int numOfchilds,dBodyID body);
	void SetAsZero();
	void SetAsZeroRecursive();
	void SetZeroTransform(Fmatrix& aTransform);
	PHDynamicData(unsigned int numOfchilds,dBodyID body);
	PHDynamicData();
	virtual ~PHDynamicData();
	void GetWorldMX(Fmatrix& aTransform){
			dMatrix3 R;
			dQtoR(dBodyGetQuaternion(body),R);
			DMXPStoFMX(R,dBodyGetPosition(body),aTransform);
			}
	void GetTGeomWorldMX(Fmatrix& aTransform){
			if(!transform) return;
			Fmatrix NormTransform,Transform;
			dVector3 P0={0,0,0,-1};
			Fvector Translate,Translate1;
			//compute_final_tx(geom);
			//dQtoR(dBodyGetQuaternion(body),R);
			DMXPStoFMX(dBodyGetRotation(body),P0,NormTransform);
			DMXPStoFMX(dGeomGetRotation(dGeomTransformGetGeom(transform)),P0,Transform);
	

			Memory.mem_copy(&Translate,dGeomGetPosition(dGeomTransformGetGeom(transform)),sizeof(Fvector));
			Memory.mem_copy(&Translate1,dBodyGetPosition(body),sizeof(Fvector));

			aTransform.identity();
			aTransform.translate_over(Translate);
			aTransform.mulA(NormTransform);
			aTransform.translate_over(Translate1);
			aTransform.mulA(Transform);
			
		//	Translate.add(Translate1);	
			//transform.translate_over(Translate1);

			//transform.translate_add
			//normalTransform=oMatrix4x4(dGeomGetRotation(dGeomTransformGetGeom(geom)))*normalTransform;
			//oMatrix4x4 meshTransform(normalTransform);
			
			//meshTransform.PreTranslate(oVector3(dGeomGetPosition(dGeomTransformGetGeom(geom))));
			//meshTransform.PostTranslate(oVector3(dBodyGetPosition(body)));
			}
	static inline DMXPStoFMX(const dReal* R,const dReal* pos,Fmatrix& aTransform){

			Memory.mem_copy(&aTransform,R,sizeof(Fmatrix));
			aTransform.transpose();
			Memory.mem_copy(&aTransform.c,pos,sizeof(Fvector));
			aTransform._14=0.f;
			aTransform._24=0.f;
			aTransform._34=0.f;
			aTransform._44=1.f;
		};
	static inline FMX33toDMX(const Fmatrix33& aTransform,dReal* R){	
		R[0]=aTransform._11;
		R[4]=aTransform._12;
		R[8]=aTransform._13;

		R[1]=aTransform._21;
		R[5]=aTransform._22;
		R[9]=aTransform._23;

		R[2]=aTransform._31;
		R[6]=aTransform._32;
		R[10]=aTransform._33;
		};
private:
	void CalculateR_N_PosOfChilds(dBodyID parent);
public:
	bool SetGeom(dGeomID ageom);
	bool SetTransform(dGeomID ageom);
};

#endif // !defined(AFX_PHDynamicData_H__ACC01646_B581_4639_B78C_30311432021B__INCLUDED_)
