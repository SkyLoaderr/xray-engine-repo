/************************************************************************

  smfrefine

  This filter refines a model by iteratively splitting edges.  It will
  also optionally add "noise" to the newly created vertices.

  Copyright (C) 1998 Michael Garland, All Rights Reserved.
  
  $Id: smfrefine.cxx,v 1.4 1999/03/24 15:08:57 garland Exp $

 ************************************************************************/

#include <stdmix.h>

#include <MxSMF.h>
#include <MxGeom3D.h>
#include <MxEdgeFilter.h>
#include <MxVector.h>

#include "cmdline.h"

static float max_sqrlen = HUGE;
static MxBounds bounds;
static float noise_level = 0.0;
static float noise_scale;
static float diameter;

static char *options = "t:n:h";
static char *usage_string =
"-t <n>		Set the maximum allowable edge length.\n"
"-n <L>		Set the noise level to L% of the model diameter [default=0].\n"
"\n";

static
void process_options(int opt, char *optarg)
{
    switch( opt )
    {
    case 't':  max_sqrlen = atof(optarg); max_sqrlen*=max_sqrlen; break;
    case 'n':  noise_level = atof(optarg)/100; break;
    case 'h':
        cerr << endl << usage_string << endl; exit(0); break;
    }
}

class RefineFilter : public MxEdgeFilter
{
private:
    MxVertexID first_new_vert;

public:
    RefineFilter(MxStdModel *m) : MxEdgeFilter(m)
	{ first_new_vert = m->vert_count(); }

    void add_noise(float scale);
};

// Run this procedure after refinement to add some noise to the model.
// Noise is added as a per-vertex displacement along the average
// normal direction.  This procedure cannot be incorporated into the
// refinement loop because (1) the results are less aesthetic and (2)
// termination is dependent on edge lengths which might never converge
// if we're always adding noise.
//
// NOTE: Only newly created vertices will be perturbed.  All vertices
//       prior to first_new_vert will remain in their original positions.
//
void RefineFilter::add_noise(float scale)
{
    for(uint i=first_new_vert; i<m->vert_count(); i++)
    {
	float n[3];
	m->compute_vertex_normal(i, n);

	float delta = (random1()-0.5) * scale;
	mxv_scale(n, delta, 3);
	mxv_addinto(m->vertex(i), n, 3);
    }
}

main(int argc, char *argv[])
{
    MxStdModel *m = process_cmdline(argc, argv, options, process_options);
    if( !m ) return 0;

    bounds.reset();
    for(uint i=0; i<m->vert_count(); i++)
	bounds.add_point(m->vertex(i));
    bounds.complete();

    diameter = sqrt(mxv_L2(bounds.max, bounds.min, 3));
    noise_scale = diameter * noise_level;

    RefineFilter filter(m);
    filter.initialize();
    filter.filter_above_rank(max_sqrlen);

    if( noise_level > 0 )
    {
	mxmsg_signalf(MXMSG_NOTE,
		      "Noise level = %f%% (%f)",
		      noise_level*100, noise_scale);
	filter.add_noise(noise_scale);
    }

    output_final_model(m, true);
    delete m;
    return 0;
}
