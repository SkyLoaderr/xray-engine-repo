#ifndef RAPID_H
#define RAPID_H

/****************************************************************************/
// RAPID API RETURN VALUES

const int RAPID_OK = 0;
  // Used by all API routines except constructors and destructors.

const int RAPID_ERR_MODEL_OUT_OF_MEMORY = 1;
  // Returned when an API function cannot obtain enough memory to store
  // or process a RAPID_model object.

const int RAPID_ERR_COLLIDE_OUT_OF_MEMORY = 2;
  // Returned when RAPID_Collide() cannot allocate enough storage to hold
  // collision information.  In this case, there is as much collision
  // detection information available as could be allocated for.

const int RAPID_ERR_UNPROCESSED_MODEL = 3;
  // Returned when an unprocessed model is passed to a function which
  // expects only processed models, such as RAPID_Collide().

const int RAPID_ERR_BUILD_OUT_OF_SEQUENCE = 4;
  // Returned when:
  //       1. AddTri() is called before BeginModel().  The triangle will
  //          be added anyway as if BeginModel() had been previously called.
  //       2. BeginModel() is called immediately after AddTri().  The
  //          model will be placed into an empty initial state.
  // This error code is something like a warning: the invoked
  // operation takes place anyway, but the returned error code may tip
  // off the client that something out of the ordinary is happenning.

const int RAPID_ERR_BUILD_EMPTY_MODEL = 5;
  // Returned when EndModel() is called on a model to which no
  // triangles have been added.  This is similar in spirit to the
  // OUT_OF_SEQUENCE return code, except that the requested operation
  // has FAILED -- the model remains "unprocessed".

const int RAPID_UNKNOWN_OPTION = 6;
  // Returned when SetOption or UnsetOption is called with a first
  // parameter which doesn't correspond to an unknown option.  In
  // other words, if you get this error at run-time, you probably
  // typed in the wrong RAPID_ symbol.

const int RAPID_BAD_PARAMS = 7;
  // Returned when an API function accept bad params.

/****************************************************************************/

//  for SetOption

enum RAPID_option
{
  RAPID_BUILD_NUM_TRIS_PER_LEAF,
  RAPID_BUILD_MAX_LEVELS_DEEP
};

class	box;
struct	tri;
class   ray;

class RAPID_model
{
  friend class XRCollide;
public:
  box *b;
  int num_boxes_alloced;

  tri *tris;
  int num_tris;
  int num_tris_alloced;
  int *tri_index;

  int build_opt_max_levels_deep;
  int build_opt_num_tris_per_leaf;

  int build_state;

  int build_hierarchy();

public:
  // These are everything the client needs to use RAPID.
			RAPID_model();
			~RAPID_model();

  int		BeginModel		( );
  int		AddAllTris		( int count, void *p );
  int		AddTri			( tri& one );
  int		AddTri			( Fvector& p1, Fvector& p2, Fvector& p3, DWORD link );
  int		EndModel		( );

  int		SetOption		( RAPID_option opt, int val );
  int		UnsetOption		( RAPID_option opt );
};

/****************************************************************************/
#define RAPID_MAX_RAYPICK_RANGE 1000

struct collision_pair
{
  int id1;
  int id2;
};

struct raypick_info
{
  int id;
  float range;
  float u,v;
  Fvector p1,p2,p3;
};

//------------------------------------------------------------------------------
// X-Ray Collision class
//------------------------------------------------------------------------------
class XRCollide{
// internal use
    Fmatrix33   XR_mR;
    Fvector     XR_mT;
    float       XR_ms;

    bool        bFirstContact;
    RAPID_model *model1;
    RAPID_model *model2;

    int         tri_contact (Fvector& P1, Fvector& P2, Fvector& P3,
				             Fvector& Q1, Fvector& Q2, Fvector& Q3);
    int         obb_disjoint(Fmatrix33& B, Fvector& T, Fvector& a, Fvector& b);
// raypick
    int         add_raypick     (raypick_info& rp_inf);
    int         ray_contact     (box *b1, ray *rp);
    int         raypick_recursive(box *b1, ray *rp, Fmatrix33& R, Fvector& T, float s);
// collide model
    int         add_collision   (int id1, int id2);
    int         tri_contact     (box *b1, box *b2);
    int         collide_recursive(box *b1, box *b2, Fmatrix33& R, Fvector& T, float s);
protected:
// last statistic
    int         num_box_tests;
    int         num_tri_tests;
    int			min_raypick_id;
// last collide info
public:
    CList<collision_pair>   ModelContact;
    CList<raypick_info>     RayContact;
public:
                XRCollide();
    virtual     ~XRCollide(){;}

    __forceinline bool GetMinRayPickInfo (raypick_info* rp_inf){
                                    if (min_raypick_id>=0){
										VERIFY(min_raypick_id<RayContact.count);
                                        CopyMemory(rp_inf, &(RayContact[min_raypick_id]), sizeof(raypick_info));
                                        return true;
									}else return false;
                                }
    __forceinline bool GetMinRayPickDistance (float& d){
                                    if (min_raypick_id>=0){
										VERIFY(min_raypick_id<RayContact.count);
                                        d = RayContact[min_raypick_id].range;
                                        return true;
									}else return false;
                                }

    __forceinline int  GetModelContactCount()  {return ModelContact.count;}
    __forceinline int  GetRayContactCount  ()  {return RayContact.count;}

    int Collide(Fmatrix33& R1, Fvector& T1, float s1, RAPID_model *o1,
                Fmatrix33& R2, Fvector& T2, float s2, RAPID_model *o2,
	            bool bFirstContact = false);
    int Collide(Fmatrix33& R1, Fvector& T1, RAPID_model *o1,
                Fmatrix33& R2, Fvector& T2, RAPID_model *o2,
	            bool bFirstContact = false){return Collide(R1,T1,1.f,o1,R2,T2,1.f,o2,bFirstContact);}
    int RayPick(Fmatrix& parent, float s1, RAPID_model *o, Fvector& start, Fvector& dir,
                float max_range = RAPID_MAX_RAYPICK_RANGE, bool bFirstContact = false);

};

extern XRCollide XRC;

/****************************************************************************/
#endif
