#pragma once

class FTreeVisual :	public IRender_Visual
{
private:
	struct	_5color
	{
		Fvector					rgb;		// - all static lighting
		float					hemi;		// - hemisphere
		float					sun;		// - sun
	};
protected:
	IDirect3DVertexBuffer9*		pVertices;
	u32							vBase;
	u32							vCount;

	IDirect3DIndexBuffer9*		pIndices;
	u32							iBase;
	u32							iCount;
	u32							dwPrimitives;

	ref_str						c_consts;
	ref_str						c_wave;
	ref_str						c_wind;
	ref_str						c_c_bias;
	ref_str						c_c_scale;
	ref_str						c_c_sun;

	_5color						c_scale;
	_5color						c_bias;
public:
	Fmatrix						xform;
public:
	virtual void Render			(float LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void Load			(LPCSTR N, IReader *data, u32 dwFlags);
	virtual void Copy			(IRender_Visual *pFrom	);
	virtual void Release		();

	FTreeVisual(void);
	virtual ~FTreeVisual(void);
};

const int		FTreeVisual_tile	= 16;
const int		FTreeVisual_quant	= 32768/FTreeVisual_tile;
