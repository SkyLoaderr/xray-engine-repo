//---------------------------------------------------------------------------
#ifndef StatGraphH
#define StatGraphH
//---------------------------------------------------------------------------
class CStatGraph	: public pureRender
{
public:
	enum EStyle{
    	stBar,
        stCurve,
		stBarLine,
		stPoint,
    };
protected:
	struct SElement{
    	u32 		color;
        float		data;
			        SElement(float d, u32 clr)
        {
        	color	= clr;
            data	= d;
        }
    };
	DEFINE_DEQUE	(SElement,ElementsDeq,ElementsDeqIt);
	struct SSubGraph
	{
		EStyle			style;
		ElementsDeq		elements;
					SSubGraph(EStyle s)
		{
			style = s;
		};
					void SetStyle (EStyle s)
		{
			style = s;
		};
	};
	DEFINE_VECTOR	(SSubGraph,SubGraphVec,SubGraphVecIt);
	SubGraphVec		subgraphs;
	
	float			mn, mx;
	u32				max_item_count;
	Ivector2 		lt,rb;
	Ivector2 		grid;
	Fvector2		grid_step;
	u32				grid_color;
	u32				base_color;
	u32				rect_color;
	u32				back_color;
	ref_geom 		hGeomTri;
	ref_geom 		hGeomLine;
protected:
	virtual void	RenderBack		();

	virtual void	RenderBars		( FVF::TL0uv** ppv, ElementsDeq* pelements );
	virtual void	RenderLines		( FVF::TL0uv** ppv, ElementsDeq* pelements );
	virtual void	RenderBarLines	( FVF::TL0uv** ppv, ElementsDeq* pelements );
//	virtual void	RenderPoints	( FVF::TL0uv** ppv, ElementsDeq* pelements );
public:
  					CStatGraph	();
					~CStatGraph	();
    virtual void 	OnRender	();
			void 	OnDeviceCreate	();
			void 	OnDeviceDestroy	();	

    IC 	void		SetStyle	(EStyle s, u32 SubGraphID = 0)
    {
		if (SubGraphID >= subgraphs.size()) return;
		SubGraphVecIt it = subgraphs.begin() + SubGraphID;
		it->SetStyle(s);
    }
	
    IC	void		SetRect		(int l, int t, int w, int h, u32 rect_clr, u32 back_clr)
    {
        lt.set		(l,t);
        rb.set		(l+w,t+h);
        rect_color	= rect_clr;
		back_color	= back_clr;
    }
    IC	void		SetGrid		(int w_div, float w_step, int h_div, float h_step, u32 grid_clr, u32 base_clr)
    {
        grid.set	(w_div,h_div);
		grid_step.set (w_step, h_step);
        grid_color 	= grid_clr;
		base_color	= base_clr;
    }
    IC	void		SetMinMax	(float _mn, float _mx, u32 item_count)
    {
        mn			= _mn;
        mx			= _mx;
        max_item_count = item_count;
		for (SubGraphVecIt it=subgraphs.begin(); it!=subgraphs.end(); it++)
		{
			while(it->elements.size()>max_item_count) it->elements.pop_front();
		};
    }
    IC	void		AppendItem	(float d, u32 clr, u32 SubGraphID = 0)
    {
		if (SubGraphID>=subgraphs.size()) return;

    	clamp		(d,mn,mx);
		
		SubGraphVecIt it = subgraphs.begin() + SubGraphID;
        it->elements.push_back(SElement(d,clr));
        while(it->elements.size()>max_item_count) it->elements.pop_front();
    };
	IC	u32			AppendSubGraph	(EStyle S)
	{
		subgraphs.push_back(SSubGraph(S));
		return subgraphs.size()-1;
	};
};
#endif
