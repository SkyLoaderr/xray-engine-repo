#ifndef GeometryCollectorH
#define GeometryCollectorH
//---------------------------------------------------------------------------

struct GCVertex{
	Fvector				pos;
    					GCVertex	(const Fvector& p){pos=p;}
};

struct GCFace{
	u32 				verts[3];
    bool				valid;
};

class VCPacked
{
protected:
	typedef xr_vector<xr_vector<xr_vector<U32Vec> > > GCHash;

    xr_vector<GCVertex>	verts;

    GCHash				VM;
    Fvector				VMmin, VMscale;
    Fvector				VMeps;
    float				eps;
    u32					clpMX,clpMY,clpMZ;
public:
    					VCPacked	(const Fbox &bb, float eps=EPS, u32 clpMX=24, u32 clpMY=16, u32 clpMZ=24, int apx_vertices=5000);
	virtual				~VCPacked	()	{ clear();					}
    virtual void		clear		();

    u32					add_vert	(const Fvector& V);

    GCVertex*			getV		()	{ return &*verts.begin();	}
    size_t				getVS		()	{ return verts.size();		}
    xr_vector<GCVertex>&Vertices	()	{ return verts;				}
};

class GCPacked: public VCPacked
{
    xr_vector<GCFace>	faces;
    void				validate   	(GCFace& F)
    {
		if ((F.verts[0]==F.verts[1])||(F.verts[0]==F.verts[2])||(F.verts[1]==F.verts[2]))	F.valid=false;
        else																				F.valid=true;
    }
public:
    					GCPacked	(const Fbox &bb, float eps=EPS, u32 clpMX=24, u32 clpMY=16, u32 clpMZ=24, int apx_vertices=5000, int apx_faces=5000):
			                        VCPacked(bb,eps,clpMX,clpMY,clpMZ,apx_vertices){faces.reserve(apx_faces);}
	virtual				~GCPacked	()	{ clear();					}
    virtual void		clear		();

    xr_vector<GCFace>&	Faces		()	{ return faces;				}

	void				add_face	(const Fvector& v0, const Fvector& v1, const Fvector& v2);

    GCFace*				getF		()	{ return &*faces.begin();	}
    size_t				getFS		()	{ return faces.size();		}
};

#endif
