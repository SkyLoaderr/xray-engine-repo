//---------------------------------------------------------------------------
#ifndef StatGraphH
#define StatGraphH
//---------------------------------------------------------------------------
class CStatGraph:public pureRender, public pureDeviceCreate, public pureDeviceDestroy{
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
	ElementsDeq		elements;
	float			mn, mx;
	u32				max_item_count;
	Ivector2 		lt,rb;
	Ivector2 		grid;
	u32				grid_color;
	u32				rect_color;
	ref_geom 		hGeom; 
public:
	enum EStyle{
    	stBar,
        stCurve
    };
protected:
    EStyle			style;
public:
  					CStatGraph	();
					~CStatGraph	();
    virtual void 	OnRender	();
    virtual void 	OnDeviceCreate	();
    virtual void 	OnDeviceDestroy	();	
    IC 	void		SetStyle	(EStyle s)
    {
    	style		= s;
    }
    IC	void		SetRect		(int l, int t, int w, int h, u32 rect_clr)
    {
        lt.set		(l,t);
        rb.set		(l+w,t+h);
        rect_color	= rect_clr;
    }
    IC	void		SetGrid		(int w_div, int h_div, u32 clr)
    {
        grid.set	(w_div,h_div);
        grid_color 	= clr;
    }
    IC	void		SetMinMax	(float _mn, float _mx, u32 item_count)
    {
        mn			= _mn;
        mx			= _mx;
        max_item_count = item_count;
        while(elements.size()>max_item_count) elements.pop_front();
    }
    IC	void		AppendItem	(float d, u32 clr)
    {
    	clamp		(d,mn,mx);
        elements.push_back(SElement(d,clr));
        while(elements.size()>max_item_count) elements.pop_front();
    };
};
#endif
