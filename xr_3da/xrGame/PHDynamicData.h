// PHDynamicData.h: interface for the PHDynamicData class.
//
//////////////////////////////////////////////////////////////////////

#include <ode\ode.h>

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
dGeomID geom;
dGeomID transform;
PHDynamicData* Childs;

unsigned int numOfChilds;
Fmatrix ZeroTransform;
public:
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
	

			memcpy(&Translate,dGeomGetPosition(dGeomTransformGetGeom(transform)),sizeof(Fvector));
			memcpy(&Translate1,dBodyGetPosition(body),sizeof(Fvector));

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

			memcpy(&aTransform,R,sizeof(Fmatrix));
			aTransform.transpose();
			memcpy(&aTransform.c,pos,sizeof(Fvector));
			aTransform._14=0.f;
			aTransform._24=0.f;
			aTransform._34=0.f;
			aTransform._44=1.f;
		};
	static inline FMX33toDMX(const Fmatrix33& aTransform,dReal* R){
			dReal tmp;
			memcpy(R,&aTransform,sizeof(dMatrix3));
			tmp=R[4];
			R[4]=R[1];
			R[1]=tmp;

			tmp=R[8];
			R[8]=R[2];
			R[2]=tmp;

			tmp=R[9];
			R[9]=R[6];
			R[6]=tmp;
		};
private:
	void CalculateR_N_PosOfChilds(dBodyID parent);
public:
	bool SetGeom(dGeomID ageom);
	bool SetTransform(dGeomID ageom);
};

#endif // !defined(AFX_PHDynamicData_H__ACC01646_B581_4639_B78C_30311432021B__INCLUDED_)
