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
	Fvector center;
	float radius;
	FPcolor color;
};
void DU_DrawLineSphere(
	LPDIRECT3DDEVICE7 device,
	DU_SphereDefinition *params );

//----------------------------------------------------

struct DU_DirectionDefinition {
	Fvector center;
	Fvector direction;
	FPcolor color;
};
void DU_DrawDirectionalLight(
	LPDIRECT3DDEVICE7 device,
	DU_DirectionDefinition *params );


void DU_DrawLine(
	LPDIRECT3DDEVICE7 device,
	Fvector& center,
	Fvector& direction,
    float radius,
	FPcolor& color);

//----------------------------------------------------
#endif /*_INCDEF_D3DUtils_H_*/

