//----------------------------------------------------
// file: D3DUtils.h
//----------------------------------------------------

#ifndef _INCDEF_D3DUtils_H_
#define _INCDEF_D3DUtils_H_
//----------------------------------------------------

void DU_InitUtilLibrary();
void DU_UninitUtilLibrary();

//----------------------------------------------------

struct DU_SphereDefinition {
	IVector center;
	float radius;
	IColor color;
};
void DU_DrawLineSphere(
	LPDIRECT3DDEVICE7 device,
	DU_SphereDefinition *params );

//----------------------------------------------------

struct DU_DirectionDefinition {
	IVector center;
	IVector direction;
	IColor color;
};
void DU_DrawDirectionalLight(
	LPDIRECT3DDEVICE7 device,
	DU_DirectionDefinition *params );


//----------------------------------------------------
#endif /*_INCDEF_D3DUtils_H_*/

