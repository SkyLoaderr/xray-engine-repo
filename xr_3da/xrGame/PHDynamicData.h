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
PHDynamicData* Childs;
unsigned int numOfChilds;

public:
	PHDynamicData& operator [] (unsigned int i) {return Childs[i];};
	void Destroy();
	void Create(unsigned int numOfchilds,dBodyID Body);
	void CalculateData(void);
	PHDynamicData * GetChild(unsigned int ChildNum);
	bool SetChild(unsigned int ChildNum,unsigned int numOfchilds,dBodyID body);
	PHDynamicData(unsigned int numOfchilds,dBodyID body);
	PHDynamicData();
	virtual ~PHDynamicData();
	void GetWorldMX(Fmatrix& transform){
			dMatrix3 R;
			dQtoR(dBodyGetQuaternion(body),R);
			DMXPStoFMX(R,dBodyGetPosition(body),transform);
			}
	void GetTGeomWorldMX(Fmatrix& transform){
			if(!geom) return;
			dMatrix3 R;
			Fmatrix NormTransform,Transform;
			dVector3 P0={0,0,0,-1};
			Fvector Translate,Translate1;
			//compute_final_tx(geom);
			//dQtoR(dBodyGetQuaternion(body),R);
			DMXPStoFMX(dBodyGetRotation(body),P0,NormTransform);
			DMXPStoFMX(dGeomGetRotation(dGeomTransformGetGeom(geom)),P0,Transform);
	

			memcpy(&Translate,dGeomGetPosition(dGeomTransformGetGeom(geom)),sizeof(Fvector));
			memcpy(&Translate1,dBodyGetPosition(body),sizeof(Fvector));

			transform.identity();
			transform.translate_over(Translate);
			transform.mulA(NormTransform);
			Transform.translate_over(Translate1);
			transform.mulA(Transform);
			
		//	Translate.add(Translate1);	
			//transform.translate_over(Translate1);

			//transform.translate_add
			//normalTransform=oMatrix4x4(dGeomGetRotation(dGeomTransformGetGeom(geom)))*normalTransform;
			//oMatrix4x4 meshTransform(normalTransform);
			
			//meshTransform.PreTranslate(oVector3(dGeomGetPosition(dGeomTransformGetGeom(geom))));
			//meshTransform.PostTranslate(oVector3(dBodyGetPosition(body)));
			}
	static inline DMXPStoFMX(const dReal* R,const dReal* pos,Fmatrix& transform){

			memcpy(&transform,R,sizeof(Fmatrix));
			transform.transpose();
			memcpy(&transform.c,pos,sizeof(Fvector));
			transform._14=0.f;
			transform._24=0.f;
			transform._34=0.f;
			transform._44=1.f;
		};
private:
	void CalculateR_N_PosOfChilds(dBodyID parent);
public:
	bool SetGeom(dGeomID ageom);
};

#endif // !defined(AFX_PHDynamicData_H__ACC01646_B581_4639_B78C_30311432021B__INCLUDED_)
