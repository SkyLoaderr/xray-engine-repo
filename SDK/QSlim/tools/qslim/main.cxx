/************************************************************************

  Main QSlim file.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: main.cxx,v 1.21.2.1 2002/01/31 19:53:14 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <mixio.h>
#include "qslim.h"

// Configuration variables and initial values
//
int cam_mode = 0; // 0 - plane, 1 - arcball
int slim_mode = 0; // 0 - edge, 1 - face, 2 - prop

extern float slim_max_error=1.f;
unsigned int face_target = 0;
double boundary_weight = 1000.0;
double compactness_ratio = 0.5;
double meshing_penalty = 1000.0;
int placement_policy = MX_PLACE_ENDORMID;
int weighting_policy = MX_WEIGHT_AREA_AVG;
bool will_record_history = false;
bool will_join_only = false;
bool be_quiet = false;
OutputFormat output_format = SMF;
char *output_filename = NULL;

// Globally visible variables
//
MxSMFReader *smf = NULL;
MxStdModel	*m = NULL;
MxStdModel	*m_orig = NULL;
MxStdSlim	*slim = NULL;
MxEdgeQSlim *eslim = NULL;
MxFaceQSlim *fslim = NULL;
MxPropSlim	*pslim = NULL;
QSlimLog	*history = NULL;
MxDynBlock<MxEdge> *target_edges = NULL;

const char *slim_copyright_notice =
"Copyright (C) 1998-2002 Michael Garland.  See \"COPYING.txt\" for details.";

const char *slim_version_string = "2.1";

static ostream *output_stream = NULL;

static
bool qslim_smf_hook(char *op, int, char *argv[], MxStdModel& m)
{
    if( streq(op, "e") )
    {
	if( !target_edges )
	    target_edges = new MxDynBlock<MxEdge>(m.vert_count() * 3);

	MxEdge& e = target_edges->add();

	e.v1 = atoi(argv[0]) - 1;
	e.v2 = atoi(argv[1]) - 1;

	return true;
    }

    return false;
}

bool (*unparsed_hook)(char *, int, char*[], MxStdModel&) = qslim_smf_hook;

void slim_print_banner(ostream& out)
{
    out << "QSlim surface simplification software." << endl
	<< "Version " << slim_version_string << " "
	<< "[Built " << __DATE__ << "]." << endl
	<< slim_copyright_notice << endl;
}

void slim_init()
{
    if( !slim )
    {
		switch (slim_mode){
		case 0: slim = eslim = new MxEdgeQSlim(*m); break;
		case 1: slim = fslim = new MxFaceQSlim(*m); break;
		case 2: slim = pslim = new MxPropSlim(m);  break;
		}
    }
    else
    {
		switch (slim_mode){
		case 0: eslim = new MxEdgeQSlim(*m); break;
		case 1: fslim = new MxFaceQSlim(*m); break;
		case 2: pslim = new MxPropSlim(m);  break;
		}
    }

    slim->placement_policy = placement_policy;
    slim->boundary_weight = boundary_weight;
    slim->weighting_policy = weighting_policy;
    slim->compactness_ratio = compactness_ratio;
    slim->meshing_penalty = meshing_penalty;
    slim->will_join_only = will_join_only;

    if( eslim && target_edges )	eslim->initialize	(*target_edges, target_edges->length());
    else						slim->initialize	();

	switch (slim_mode){
	case 0: break;
	case 1: break;
	case 2: pslim->collect_edges();  break;
	}

    if( will_record_history )
    {
	if( !eslim )
	    mxmsg_signal(MXMSG_WARN,
			 "History only available for edge contractions.");
	else
	{
	    history = new QSlimLog(100);
	    eslim->contraction_callback = slim_history_callback;
	}
    }
}

#define CLEANUP(x)  if(x) { delete x; x=NULL; }

void slim_cleanup()
{
    CLEANUP(smf);
    CLEANUP(m);
    CLEANUP(slim);
    eslim = NULL;
    fslim = NULL;
    CLEANUP(history);
    CLEANUP(target_edges);
    if( output_stream != &cout )
    	CLEANUP(output_stream);
}

static
void setup_output()
{
    if( !output_stream )
    {
	if( output_filename )
	    output_stream = new ofstream(output_filename);
	else
	    output_stream = &cout;
    }
}

bool select_output_format(const char *fmt)
{
    bool h = false;

    if     ( streq(fmt, "mmf") ) { output_format = MMF; h = true; }
    else if( streq(fmt, "pm")  ) { output_format = PM;  h = true; }
    else if( streq(fmt, "log") ) { output_format = LOG; h = true; }
    else if( streq(fmt, "smf") ) output_format = SMF;
    else if( streq(fmt, "iv")  ) output_format = IV;
    else if( streq(fmt, "vrml")) output_format = VRML;
    else return false;

    if( h ) will_record_history = true;

    return true;
}

void output_preamble()
{
    if( output_format==MMF || output_format==LOG )
	output_current_model();
}

void output_current_model()
{
    setup_output();

    MxSMFWriter writer;
    writer.write(*output_stream, *m);
}

static
void cleanup_for_output()
{
    // First, mark stray vertices for removal
    //
    for(unsigned int i=0; i<m->vert_count(); i++)
	if( m->vertex_is_valid(i) && m->neighbors(i).length() == 0 )
	    m->vertex_mark_invalid(i);

	// Compact vertex array so only valid vertices remain
    m->compact_vertices();
}

void output_final_model()
{
    setup_output();

    switch( output_format )
    {
    case MMF:
	output_regressive_mmf(*output_stream);
	break;

    case LOG:
	output_regressive_log(*output_stream);
	break;

    case PM:
	output_progressive_pm(*output_stream);
	break;

    case IV:
	cleanup_for_output();
	output_iv(*output_stream);
	break;

    case VRML:
	cleanup_for_output();
	output_vrml(*output_stream);
	break;

    case SMF:
	cleanup_for_output();
	output_current_model();
	break;
    }


}

void input_file(const char *filename)
{
    if( streq(filename, "-") )
	smf->read(cin, m);
    else
    {
	ifstream in(filename);
	if( !in.good() )
	    mxmsg_signal(MXMSG_FATAL, "Failed to open input file", filename);
	smf->read(in, m);
	in.close();
    }
}

static
MxDynBlock<char*> files_to_include(2);

void defer_file_inclusion(char *filename)
{
    files_to_include.add(filename);
}

void include_deferred_files()
{
    for(unsigned int i=0; i<files_to_include.length(); i++)
	input_file(files_to_include[i]);
}

void slim_history_callback(const MxPairContraction& conx, float cost)
{
    history->add(conx);
}
