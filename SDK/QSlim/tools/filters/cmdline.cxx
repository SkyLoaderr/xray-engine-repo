/************************************************************************

  Command line processing for the standard SMF filters.

  Copyright (C) 1998 Michael Garland, All Rights Reserved.
  
  $Id: cmdline.cxx,v 1.8 2000/11/20 20:58:59 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <mixio.h>
#include <MxSMF.h>

#include "cmdline.h"

static char *base_options="o:";

static char *output_filename = NULL;

MxStdModel *read_model_from_file(const char *filename,
				 MxSMFReader *smf,
				 MxStdModel *m)
{
    bool local_smfreader = false;

    if( !smf )
    {
	smf = new MxSMFReader;
	local_smfreader = true;
    }

    if( !strcmp(filename, "-"))
	m = smf->read(cin, m);
    else
    {
	ifstream in(filename);
	if( !in.good() )  fatal_error("Failed to open input file");
	m = smf->read(in, m);
    }

    if( local_smfreader ) delete smf;

    return m;
}

void process_cmdline_only(int argc, char *argv[],
			    const char *user_options, option_processor proc)
{
    char *options = base_options;

    if( user_options )
    {
	int base_len = strlen(base_options);
	int user_len = strlen(user_options);

	options = new char[base_len + user_len + 1];
	strcpy(options, user_options);
	strcpy(options+user_len, base_options);
	options[base_len+user_len] = '\0';
    }

    int opt;

    while( (opt = getopt(argc, argv, options)) != EOF )
    {
	switch( opt )
	{
	case 'o':
	    output_filename = optarg;
	    break;

	default:
	    if( proc )
		(*proc)(opt, optarg);
	    break;
	}
    }
}

MxStdModel *process_cmdline(int argc, char *argv[],
			    const char *user_options, option_processor proc)
{
    process_cmdline_only(argc, argv, user_options, proc);

    MxSMFReader smf;
    MxStdModel *m = new MxStdModel(64, 64);

    if( optind==argc )
	smf.read(cin, m);

    for(; optind<argc; optind++)
	read_model_from_file(argv[optind], &smf, m);

    if( m->face_count() == 0 && m->vert_count() == 0 )
    {
	cerr << argv[0] << ": Input model is empty." << endl;
	delete m;
	m = NULL;
    }

    return m;
}

void output_final_model(MxStdModel *m, bool was_modified)
{
    if( was_modified )
	m->compact_vertices();

    MxSMFWriter writer;

    if( !output_filename )
	writer.write(cout, *m);
    else
    {
	ofstream out(output_filename);
	writer.write(out, *m);
    }
}
