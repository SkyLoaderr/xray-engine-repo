#pragma once

// refs
struct FSlideWindowItem;

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

	_5color						c_scale;
	_5color						c_bias;
	Fmatrix						xform;
public:
	virtual void Render			(float LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void Load			(LPCSTR N, IReader *data, u32 dwFlags);
	virtual void Copy			(IRender_Visual *pFrom	);
	virtual void Release		();

	FTreeVisual(void);
	virtual ~FTreeVisual(void);
};

class FTreeVisual_ST :	public FTreeVisual
{
	typedef FTreeVisual inherited;
public:
					FTreeVisual_ST	(void);
	virtual			~FTreeVisual_ST	(void);

	virtual void	Render			(float LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void	Load			(LPCSTR N, IReader *data, u32 dwFlags);
	virtual void	Copy			(IRender_Visual *pFrom	);
	virtual void	Release			();
};

class FTreeVisual_PM :	public FTreeVisual
{
	typedef FTreeVisual inherited;
private:
	FSlideWindowItem*	pSWI;
	u32					last_lod;
public:
					FTreeVisual_PM	(void);
	virtual			~FTreeVisual_PM	(void);

	virtual void	Render			(float LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void	Load			(LPCSTR N, IReader *data, u32 dwFlags);
	virtual void	Copy			(IRender_Visual *pFrom	);
	virtual void	Release			();
};

const int		FTreeVisual_tile	= 16;
const int		FTreeVisual_quant	= 32768/FTreeVisual_tile;
