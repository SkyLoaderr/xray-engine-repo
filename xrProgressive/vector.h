#pragma once

// ************************* Vector
class  Vector {
  public:
	float x,y,z;
	IC Vector(float _x=0.0,float _y=0.0,float _z=0.0){x=_x;y=_y;z=_z;};
	operator float *() { return &x;};
};

 float	magnitude(Vector &v);
 float	magnitude_sqr(Vector &v);
 Vector	normalize(Vector v);
 Vector operator+(Vector& v1,Vector& v2);
 Vector operator-(Vector& v);
 Vector operator-(Vector& v1,Vector& v2);
 Vector operator*(Vector& v1,float s);
 Vector operator*(float s,Vector& v1);
 Vector operator/(Vector& v1,float s);
 float  operator^(Vector& v1,Vector& v2);  // DOT product
 Vector operator*(Vector& v1,Vector& v2);  // CROSS product
 Vector planelineintersection(Vector n,float d,Vector p1,Vector p2);

// ************************* List
template <class Type> 
class  List {
	public:
		vector<Type>	V;
		typedef vector<Type>::iterator VI;

		List	(int s=0) 
		{	if (s) V.reserve(s);}

		IC Type	&operator[](int i)	{ return V[i];				}
		IC void	clear()				{ V.clear();				}
		IC void	SetSize(int s)		{ V.resize(s);				}
		IC void	Pack()				{ ;							}
		IC void	Add(Type T)			{ V.push_back(T);			}
		IC void	AddUnique(Type T)	{ if (!Contains(T)) Add(T);	}
		IC void	DelIndex(int i)		{ V.erase(V.begin()+i);		}
		IC int 	Contains(Type T)	{ return count(V.begin(),V.end(),T); }
		IC int	num()				{ return V.size();			}
		IC int 	Place(Type T)		{ 
			VI it = find(V.begin(),V.end(),T); 
			if (it!=V.end())	return it-V.begin();
			else				return -1;
		}
		IC void	Remove(Type T)		{
			VI it = remove(V.begin(),V.end(),T);
			V.erase(it,V.end());
		}
};

// ************************* List
class  P_Triangle;
class  P_Vertex;
/*
 *  For the polygon reduction algorithm we use data structures
 *  that contain a little bit more information than the usual
 *  indexed face set type of data structure.
 *  From a vertex we wish to be able to quickly get the
 *  neighboring faces and vertices.
 */

// ************************* P_Triangle
class  P_Triangle {
private:
	void			_Construct(void);
public:
	P_Vertex *      vertex[3]; // the 3 points that make this tri
	Vector          normal;    // unit vector othogonal to this face

					P_Triangle(P_Vertex *v0,P_Vertex *v1,P_Vertex *v2);
					P_Triangle(int v0, int v1, int v2);
	                ~P_Triangle();

	void            ComputeNormal();
	void            ReplaceVertex(P_Vertex *vold,P_Vertex *vnew);
	int             HasVertex(P_Vertex *v);
};

// ************************* P_Vertex
class  P_Vertex {
private:
	bool				LiesOnBorder();
public:
	Vector          	position; // location of point in euclidean space
	P_UV				uv[16];	  // 16 presets of texture coordinates
	int             	id;       // place of vertex in original list
	List<P_Vertex *>  	neighbor; // adjacent vertices
	List<P_Triangle *>	face;     // adjacent triangles
	float           	objdist;  // cached cost of collapsing edge
	P_Vertex *        	collapse; // candidate vertex for collapse
	bool				bBorder;
	bool				bCriticalBorder;
	
	                	P_Vertex(Vector v,int _id,P_UV *uv);
	                	~P_Vertex();

	void				RemoveIfNonNeighbor	(P_Vertex *n);
	void				DetectBorder()		{ bBorder = LiesOnBorder(); }
	void				OptimizeBorder();
};
