/* OggEnc
 *
 * This program is distributed under the GNU General Public License, version 2.
 * A copy of this license is included with this source.
 *
 * Copyright 2000-2002, Michael Smith <msmith@xiph.org>
 *
 * Portions from Vorbize, (c) Kenneth Arnold <kcarnold@yahoo.com>
 * and libvorbis examples, (c) Monty <monty@xiph.org>
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <errno.h>

#include "platform.h"
#include "encode.h"
#include "audio.h"
#include "utf8.h"
#include "i18n.h"


#define VERSION_STRING "OggEnc v1.0.1 (libvorbis 1.0.1)\n"
#define COPYRIGHT "(c) 2000-2003 Michael Smith <msmith@xiph.org>\n"

#define CHUNK 4096 /* We do reads, etc. in multiples of this */

struct option long_options[] = {
	{"quiet",0,0,'Q'},
	{"help",0,0,'h'},
	{"comment",1,0,'c'},
	{"artist",1,0,'a'},
	{"album",1,0,'l'},
	{"title",1,0,'t'},
    {"genre",1,0,'G'},
	{"names",1,0,'n'},
    {"name-remove",1,0,'X'},
    {"name-replace",1,0,'P'},
	{"output",1,0,'o'},
	{"version",0,0,'v'},
	{"raw",0,0,'r'},
	{"raw-bits",1,0,'B'},
	{"raw-chan",1,0,'C'},
	{"raw-rate",1,0,'R'},
    {"raw-endianness",1,0, 0},
	{"bitrate",1,0,'b'},
	{"min-bitrate",1,0,'m'},
	{"max-bitrate",1,0,'M'},
	{"quality",1,0,'q'},
	{"date",1,0,'d'},
	{"tracknum",1,0,'N'},
	{"serial",1,0,'s'},
    {"managed", 0, 0, 0},
    {"resample",1,0,0},
    {"downmix", 0,0,0},
    {"scale", 1, 0, 0}, 
    {"advanced-encode-option", 1, 0, 0},
	{"discard-comments", 0, 0, 0},
	{NULL,0,0,0}
};

static char *generate_name_string(char *format, char *remove_list, 
								  char *replace_list, char *artist, char *title, char *album, 
								  char *track, char *date, char *genre);
static void parse_options(int argc, char **argv, oe_options *opt);
static void build_comments(vorbis_comment *vc, oe_options *opt, int filenum, 
						   char **artist,char **album, char **title, char **tracknum, char **date,
						   char **genre);
static void usage(void);

int ogg_enc(const char* in_fn, const char* out_fn, float quality, void* comment, int comment_size)
{
	/* Default values */
	oe_options opt = {NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 
		0, NULL, 0, NULL, 0, NULL, 0, 1, 0, 0,16,44100,2, 0, NULL,
		DEFAULT_NAMEFMT_REMOVE, DEFAULT_NAMEFMT_REPLACE, 
		NULL, 0, -1,-1,-1,-.3f,-1,0, 0,0.f, 0}; 

	oe_enc_opt      enc_opts;
	vorbis_comment  vc;
	FILE *in, *out	= NULL;
	int foundformat = 0;
	int closeout	= 0, closein = 0;
	input_format	*format;
	int res			= 1;

	/* Set various encoding defaults */
	enc_opts.serialno			= 0;
	enc_opts.progress_update	= update_statistics_full;
	enc_opts.start_encode		= start_encode_full;
	enc_opts.end_encode			= final_statistics;
	enc_opts.error				= encode_error;
	enc_opts.comments			= &vc;

	in							= fopen(in_fn, "rb");

	if(in == NULL)				return 0;

	format						= open_audio_file(in, &enc_opts);
	if(!format){
		fclose					(in);
		return 0;
	};

	out							= fopen(out_fn, "wb");
	if(out == NULL){
		fclose					(out);
		return 0;
	}	

	/* OK, let's build the vorbis_comments structure */
//	build_comments				(&vc, &opt, comment, size);
	memset						(&vc,0,sizeof(vc));

	vc.user_comments			= _ogg_malloc	(sizeof(*vc.user_comments));
	vc.comment_lengths			= _ogg_malloc	(sizeof(*vc.comment_lengths));
	vc.comments					= 1;
	vc.user_comments[0]			= _ogg_malloc	(comment_size);
	memcpy						(vc.user_comments[0],comment,comment_size);
	vc.comment_lengths[0]		= comment_size;

	/* Now, set the rest of the options */
	enc_opts.out				= out;
	enc_opts.comments			= &vc;
	enc_opts.filename			= (char*)out_fn;
	enc_opts.infilename			= (char*)in_fn;
	enc_opts.quality			= quality;
	enc_opts.quality_set		= opt.quality_set;
	enc_opts.managed			= opt.managed;
	enc_opts.bitrate			= opt.nominal_bitrate; 
	enc_opts.min_bitrate		= opt.min_bitrate;
	enc_opts.max_bitrate		= opt.max_bitrate;
	enc_opts.advopt				= opt.advopt;
	enc_opts.advopt_count		= opt.advopt_count;

	// encode
	if(oe_encode(&enc_opts))
		res						= 0;

	// clear comment
	if(vc.user_comments[0])		_ogg_free(vc.user_comments[0]);
	if(vc.user_comments)		_ogg_free(vc.user_comments);
	if(vc.comment_lengths)		_ogg_free(vc.comment_lengths);
	if(vc.vendor)				_ogg_free(vc.vendor);
	memset						(&vc,0,sizeof(vc));
	// close files
	fclose						(in);
	fclose						(out);

	return res;
}
	
int __cdecl main(int argc, char **argv)
{
	/* Default values */
	oe_options opt = {NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 
			  0, NULL, 0, NULL, 0, NULL, 0, 1, 0, 0,16,44100,2, 0, NULL,
			  DEFAULT_NAMEFMT_REMOVE, DEFAULT_NAMEFMT_REPLACE, 
			  NULL, 0, -1,-1,-1,-.3f,-1,0, 0,0.f, 0}; 

	int i;

	char **infiles;
	int numfiles;
	int errors=0;

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	parse_options(argc, argv, &opt);

	if(optind >= argc)
	{
		fprintf(stderr, _("%s%s\nERROR: No input files specified. Use -h for help.\n"), VERSION_STRING, COPYRIGHT);
		return 1;
	}
	else
	{
		infiles = argv + optind;
		numfiles = argc - optind;
	}

	/* Now, do some checking for illegal argument combinations */

	for(i = 0; i < numfiles; i++)
	{
		if(!strcmp(infiles[i], "-") && numfiles > 1)
		{
			fprintf(stderr, _("ERROR: Multiple files specified when using stdin\n"));
			exit(1);
		}
	}

	if(numfiles > 1 && opt.outfile)
	{
		fprintf(stderr, _("ERROR: Multiple input files with specified output filename: suggest using -n\n"));
		exit(1);
	}

	if(opt.serial == 0)
	{
		/* We randomly pick a serial number. This is then incremented for each file */
		srand(time(NULL));
		opt.serial = rand();
	}

	for(i = 0; i < numfiles; i++)
	{
		/* Once through the loop for each file */

		oe_enc_opt      enc_opts;
		vorbis_comment  vc;
		char *out_fn = NULL;
		FILE *in, *out = NULL;
		int foundformat = 0;
		int closeout = 0, closein = 0;
		char *artist=NULL, *album=NULL, *title=NULL, *track=NULL;
        char *date=NULL, *genre=NULL;
		input_format *format;

		/* Set various encoding defaults */

		enc_opts.serialno = opt.serial++;
		enc_opts.progress_update = update_statistics_full;
        enc_opts.start_encode = start_encode_full;
		enc_opts.end_encode = final_statistics;
		enc_opts.error = encode_error;
		enc_opts.comments = &vc;
		enc_opts.copy_comments = opt.copy_comments;

		/* OK, let's build the vorbis_comments structure */
		build_comments(&vc, &opt, i, &artist, &album, &title, &track, 
                &date, &genre);

		if(!strcmp(infiles[i], "-"))
		{
			setbinmode(stdin);
			in = stdin;
            infiles[i] = NULL;
			if(!opt.outfile)
			{
				setbinmode(stdout);
				out = stdout;
			}
		}
		else
		{
			in = fopen(infiles[i], "rb");

			if(in == NULL)
			{
				fprintf(stderr, _("ERROR: Cannot open input file \"%s\": %s\n"), infiles[i], strerror(errno));
				free(out_fn);
				errors++;
				continue;
			}

			closein = 1;
		}

		/* Now, we need to select an input audio format - we do this before opening
		   the output file so that we don't end up with a 0-byte file if the input
		   file can't be read */

		if(opt.rawmode)
		{
			enc_opts.rate=opt.raw_samplerate;
			enc_opts.channels=opt.raw_channels;
			enc_opts.samplesize=opt.raw_samplesize;
            enc_opts.endianness=opt.raw_endianness;
			raw_open(in, &enc_opts);
			foundformat=1;
		}
		else
		{
			format = open_audio_file(in, &enc_opts);
			if(format)
			{
                if(!opt.quiet)
				    fprintf(stderr, _("Opening with %s module: %s\n"), 
					    	format->format, format->description);
				foundformat=1;
			}

		}

		if(!foundformat)
		{
			fprintf(stderr, _("ERROR: Input file \"%s\" is not a supported format\n"), infiles[i]?infiles[i]:"(stdin)");
    		if(closein)
				fclose(in);
			errors++;
			continue;
		}

		/* Ok. We can read the file - so now open the output file */

		if(opt.outfile && !strcmp(opt.outfile, "-"))
		{
			setbinmode(stdout);
			out = stdout;
		}
		else if(out == NULL)
		{
			if(opt.outfile)
			{
				out_fn = strdup(opt.outfile);
			}
			else if(opt.namefmt)
			{
				out_fn = generate_name_string(opt.namefmt, opt.namefmt_remove, 
                        opt.namefmt_replace, artist, title, album, track,date,
                        genre);
			}
            /* This bit was widely derided in mid-2002, so it's been removed */
            /*
			else if(opt.title)
			{
				out_fn = malloc(strlen(title) + 5);
				strcpy(out_fn, title);
				strcat(out_fn, ".ogg");
			}
            */
			else if(infiles[i])
			{
				/* Create a filename from existing filename, replacing extension with .ogg */
				char *start, *end;

				start = infiles[i];
				end = strrchr(infiles[i], '.');
				end = end?end:(start + strlen(infiles[i])+1);
			
				out_fn = malloc(end - start + 5);
				strncpy(out_fn, start, end-start);
				out_fn[end-start] = 0;
				strcat(out_fn, ".ogg");
			}
            else {
                fprintf(stderr, _("WARNING: No filename, defaulting to \"default.ogg\"\n"));
                out_fn = strdup("default.ogg");
            }

            /* Create any missing subdirectories, if possible */
            if(create_directories(out_fn)) {
                if(closein)
                    fclose(in);
				fprintf(stderr, _("ERROR: Could not create required subdirectories for output filename \"%s\"\n"), out_fn);
				errors++;
				free(out_fn);
				continue;
            }

			out = fopen(out_fn, "wb");
			if(out == NULL)
			{
				if(closein)
					fclose(in);
				fprintf(stderr, _("ERROR: Cannot open output file \"%s\": %s\n"), out_fn, strerror(errno));
				errors++;
				free(out_fn);
				continue;
			}	
			closeout = 1;
		}

		/* Now, set the rest of the options */
		enc_opts.out = out;
		enc_opts.comments = &vc;
		enc_opts.filename = out_fn;
		enc_opts.infilename = infiles[i];
        enc_opts.managed = opt.managed;
		enc_opts.bitrate = opt.nominal_bitrate; 
		enc_opts.min_bitrate = opt.min_bitrate;
		enc_opts.max_bitrate = opt.max_bitrate;
		enc_opts.quality = opt.quality;
		enc_opts.quality_set = opt.quality_set;
        enc_opts.advopt = opt.advopt;
        enc_opts.advopt_count = opt.advopt_count;

        if(opt.resamplefreq && opt.resamplefreq != enc_opts.rate) {
            int fromrate = enc_opts.rate;
            enc_opts.resamplefreq = opt.resamplefreq;
            if(setup_resample(&enc_opts)) {
                errors++;
                goto clear_all;
            }
            else if(!opt.quiet)
                fprintf(stderr, _("Resampling input from %d Hz to %d Hz\n"), fromrate, opt.resamplefreq);
        }

        if(opt.downmix) {
            if(enc_opts.channels == 2) {
                setup_downmix(&enc_opts);
                if(!opt.quiet)
                    fprintf(stderr, _("Downmixing stereo to mono\n"));
            }
            else {
                fprintf(stderr, _("ERROR: Can't downmix except from stereo to mono\n"));
                errors++;
                if(opt.resamplefreq && opt.resamplefreq != enc_opts.rate)
                    clear_resample(&enc_opts);
                goto clear_all;
            }
        }

        if(opt.scale > 0.f) {
            setup_scaler(&enc_opts, opt.scale);
            if(!opt.quiet)
                fprintf(stderr, _("Scaling input to %f\n"), opt.scale);
        }


		if(!enc_opts.total_samples_per_channel)
			enc_opts.progress_update = update_statistics_notime;

		if(opt.quiet)
		{
            enc_opts.start_encode = start_encode_null;
			enc_opts.progress_update = update_statistics_null;
			enc_opts.end_encode = final_statistics_null;
		}

		if(oe_encode(&enc_opts))
			errors++;

        if(opt.scale > 0)
            clear_scaler(&enc_opts);
        if(opt.downmix)
            clear_downmix(&enc_opts);
        if(opt.resamplefreq && opt.resamplefreq != enc_opts.rate)
            clear_resample(&enc_opts);
clear_all:

		if(out_fn) free(out_fn);
        if(opt.outfile) free(opt.outfile);
		vorbis_comment_clear(&vc);
		if(!opt.rawmode) 
			format->close_func(enc_opts.readdata);

		if(closein)
			fclose(in);
		if(closeout)
			fclose(out);
	}/* Finished this file, loop around to next... */

	return errors?1:0;

}

static void usage(void)
{
	fprintf(stdout, 
		_("%s%s\n"
		"Usage: oggenc [options] input.wav [...]\n"
		"\n"
		"OPTIONS:\n"
		" General:\n"
		" -Q, --quiet          Produce no output to stderr\n"
		" -h, --help           Print this help text\n"
		" -r, --raw            Raw mode. Input files are read directly as PCM data\n"
		" -B, --raw-bits=n     Set bits/sample for raw input. Default is 16\n"
		" -C, --raw-chan=n     Set number of channels for raw input. Default is 2\n"
		" -R, --raw-rate=n     Set samples/sec for raw input. Default is 44100\n"
        " --raw-endianness     1 for bigendian, 0 for little (defaults to 0)\n"
		" -b, --bitrate        Choose a nominal bitrate to encode at. Attempt\n"
		"                      to encode at a bitrate averaging this. Takes an\n"
		"                      argument in kbps. This uses the bitrate management\n"
        "                      engine, and is not recommended for most users.\n"
        "                      See -q, --quality for a better alternative.\n"
		" -m, --min-bitrate    Specify a minimum bitrate (in kbps). Useful for\n"
		"                      encoding for a fixed-size channel.\n"
		" -M, --max-bitrate    Specify a maximum bitrate in kbps. Useful for\n"
		"                      streaming applications.\n"
		" -q, --quality        Specify quality between 0 (low) and 10 (high),\n"
		"                      instead of specifying a particular bitrate.\n"
		"                      This is the normal mode of operation.\n"
        "                      Fractional qualities (e.g. 2.75) are permitted\n"
        "                      Quality -1 is also possible, but may not be of\n"
        "                      acceptable quality.\n"
        " --resample n         Resample input data to sampling rate n (Hz)\n"
        " --downmix            Downmix stereo to mono. Only allowed on stereo\n"
        "                      input.\n"
		" -s, --serial         Specify a serial number for the stream. If encoding\n"
		"                      multiple files, this will be incremented for each\n"
		"                      stream after the first.\n"
		" --discard-comments   Prevents comments in FLAC and Ogg FLAC files from\n"
		"                      being copied to the output Ogg Vorbis file.\n"
		"\n"
		" Naming:\n"
		" -o, --output=fn      Write file to fn (only valid in single-file mode)\n"
		" -n, --names=string   Produce filenames as this string, with %%a, %%t, %%l,\n"
		"                      %%n, %%d replaced by artist, title, album, track number,\n"
		"                      and date, respectively (see below for specifying these).\n"
		"                      %%%% gives a literal %%.\n"
        " -X, --name-remove=s  Remove the specified characters from parameters to the\n"
        "                      -n format string. Useful to ensure legal filenames.\n"
        " -P, --name-replace=s Replace characters removed by --name-remove with the\n"
        "                      characters specified. If this string is shorter than the\n"
        "                      --name-remove list or is not specified, the extra\n"
        "                      characters are just removed.\n"
        "                      Default settings for the above two arguments are platform\n"
        "                      specific.\n"
		" -c, --comment=c      Add the given string as an extra comment. This may be\n"
		"                      used multiple times. The argument should be in the\n"
        "                      format \"tag=value\".\n"
		" -d, --date           Date for track (usually date of performance)\n"
		" -N, --tracknum       Track number for this track\n"
		" -t, --title          Title for this track\n"
		" -l, --album          Name of album\n"
		" -a, --artist         Name of artist\n"
        " -G, --genre          Genre of track\n"
		"                      If multiple input files are given, then multiple\n"
		"                      instances of the previous five arguments will be used,\n"
		"                      in the order they are given. If fewer titles are\n"
		"                      specified than files, OggEnc will print a warning, and\n"
		"                      reuse the final one for the remaining files. If fewer\n"
		"                      track numbers are given, the remaining files will be\n"
		"                      unnumbered. For the others, the final tag will be reused\n"
		"                      for all others without warning (so you can specify a date\n"
		"                      once, for example, and have it used for all the files)\n"
		"\n"
		"INPUT FILES:\n"
		" OggEnc input files must currently be 24, 16, or 8 bit PCM WAV, AIFF, or AIFF/C\n"
		" files, 32 bit IEEE floating point WAV, and optionally FLAC or Ogg FLAC. Files\n"
                "  may be mono or stereo (or more channels) and any sample rate.\n"
		" Alternatively, the --raw option may be used to use a raw PCM data file, which\n"
		" must be 16 bit stereo little-endian PCM ('headerless wav'), unless additional\n"
		" parameters for raw mode are specified.\n"
		" You can specify taking the file from stdin by using - as the input filename.\n"
		" In this mode, output is to stdout unless an output filename is specified\n"
		" with -o\n"
		"\n"), VERSION_STRING, COPYRIGHT);
}

static int strncpy_filtered(char *dst, char *src, int len, char *remove_list, 
        char *replace_list)
{
    char *hit, *drop_margin;
    int used=0;

    if(remove_list == NULL || *remove_list == 0)
    {
        strncpy(dst, src, len-1);
        dst[len-1] = 0;
        return strlen(dst);
    }

    drop_margin = remove_list + (replace_list == NULL?0:strlen(replace_list));

    while(*src && used < len-1)
    {
        if((hit = strchr(remove_list, *src)) != NULL)
        {
            if(hit < drop_margin)
            {
                *dst++ = replace_list[hit - remove_list];
                used++;
            }
        }
        else
        {
            *dst++ = *src;
            used++;
        }
        src++;
    }
    *dst = 0;

    return used;
}

static char *generate_name_string(char *format, char *remove_list,
        char *replace_list, char *artist, char *title, char *album, 
        char *track, char *date, char *genre)
{
	char *buffer;
	char next;
	char *string;
	int used=0;
	int buflen;

	buffer = calloc(CHUNK+1,1);
	buflen = CHUNK;

	while(*format && used < buflen)
	{
		next = *format++;

		if(next == '%')
		{
			switch(*format++)
			{
				case '%':
					*(buffer+(used++)) = '%';
					break;
				case 'a':
					string = artist?artist:_("(none)");
					used += strncpy_filtered(buffer+used, string, buflen-used, 
                            remove_list, replace_list);
					break;
				case 'd':
					string = date?date:_("(none)");
					used += strncpy_filtered(buffer+used, string, buflen-used,
                            remove_list, replace_list);
					break;
                case 'g':
                    string = genre?genre:_("(none)");
                    used += strncpy_filtered(buffer+used, string, buflen-used,
                            remove_list, replace_list);
                    break;
				case 't':
					string = title?title:_("(none)");
					used += strncpy_filtered(buffer+used, string, buflen-used,
                            remove_list, replace_list);
					break;
				case 'l':
					string = album?album:_("(none)");
					used += strncpy_filtered(buffer+used, string, buflen-used,
                            remove_list, replace_list);
					break;
				case 'n':
					string = track?track:_("(none)");
					used += strncpy_filtered(buffer+used, string, buflen-used,
                            remove_list, replace_list);
					break;
				default:
					fprintf(stderr, _("WARNING: Ignoring illegal escape character '%c' in name format\n"), *(format - 1));
					break;
			}
		}
		else
			*(buffer + (used++)) = next;
	}

	return buffer;
}

static void parse_options(int argc, char **argv, oe_options *opt)
{
	int ret;
	int option_index = 1;

	while((ret = getopt_long(argc, argv, "A:a:b:B:c:C:d:G:hl:m:M:n:N:o:P:q:QrR:s:t:vX:", 
					long_options, &option_index)) != -1)
	{
		switch(ret)
		{
			case 0:
                if(!strcmp(long_options[option_index].name, "managed")) {
		            if(!opt->managed){
                        if(!opt->quiet)
            		        fprintf(stderr, 
                                    _("Enabling bitrate management engine\n"));
                        opt->managed = 1;
        		    }
                }
                else if(!strcmp(long_options[option_index].name, 
                            "raw-endianness")) {
				    if (opt->rawmode != 1)
    				{
	    				opt->rawmode = 1;
		    			fprintf(stderr, _("WARNING: Raw endianness specified for non-raw data. Assuming input is raw.\n"));
			    	}
				    if(sscanf(optarg, "%d", &opt->raw_endianness) != 1) {
                        fprintf(stderr, _("WARNING: Couldn't read endianness argument \"%s\"\n"), optarg);
    					opt->raw_endianness = 0;
                    }
                }
                else if(!strcmp(long_options[option_index].name,
                            "resample")) {
				    if(sscanf(optarg, "%d", &opt->resamplefreq) != 1) {
                        fprintf(stderr, _("WARNING: Couldn't read resampling frequency \"%s\"\n"), optarg);
    					opt->resamplefreq = 0;
                    }
                    if(opt->resamplefreq < 100) /* User probably specified it
                                                   in kHz accidently */
                        fprintf(stderr, 
                                _("Warning: Resample rate specified as %d Hz. Did you mean %d Hz?\n"), 
                                opt->resamplefreq, opt->resamplefreq*1000);
                }
                else if(!strcmp(long_options[option_index].name, "downmix")) {
                    opt->downmix = 1;
                }
                else if(!strcmp(long_options[option_index].name, "scale")) {
                    opt->scale = (float)atof(optarg);
				    if(sscanf(optarg, "%f", &opt->scale) != 1) {
                        opt->scale = 0;
                        fprintf(stderr, _("Warning: Couldn't parse scaling factor \"%s\"\n"), 
                                optarg);
                    }
                }
                else if(!strcmp(long_options[option_index].name, "advanced-encode-option")) {
                    char *arg = strdup(optarg);
                    char *val;

                    val = strchr(arg, '=');
                    if(val == NULL) {
                        fprintf(stderr, _("No value for advanced encoder option found\n"));
                        continue;
                    }
                    else
                        *val++=0;

                    opt->advopt = realloc(opt->advopt, (++opt->advopt_count)*sizeof(adv_opt));
                    opt->advopt[opt->advopt_count - 1].arg = arg;
                    opt->advopt[opt->advopt_count - 1].val = val;
                }
                else if(!strcmp(long_options[option_index].name, "discard-comments")) {
		  opt->copy_comments = 0;
		}

                else {
				    fprintf(stderr, _("Internal error parsing command line options\n"));
				    exit(1);
                }

				break;
			case 'a':
				opt->artist = realloc(opt->artist, (++opt->artist_count)*sizeof(char *));
				opt->artist[opt->artist_count - 1] = strdup(optarg);
				break;
			case 'c':
                if(strchr(optarg, '=') == NULL) {
                    fprintf(stderr, _("Warning: Illegal comment used (\"%s\"), ignoring.\n"), optarg);
                    break;
                }
				opt->comments = realloc(opt->comments, (++opt->comment_count)*sizeof(char *));
				opt->comments[opt->comment_count - 1] = strdup(optarg);
				break;
			case 'd':
				opt->dates = realloc(opt->dates, (++opt->date_count)*sizeof(char *));
				opt->dates[opt->date_count - 1] = strdup(optarg);
				break;
            case 'G':
                opt->genre = realloc(opt->genre, (++opt->genre_count)*sizeof(char *));
                opt->genre[opt->genre_count - 1] = strdup(optarg);
                break;
			case 'h':
				usage();
				exit(0);
				break;
			case 'l':
				opt->album = realloc(opt->album, (++opt->album_count)*sizeof(char *));
				opt->album[opt->album_count - 1] = strdup(optarg);
				break;
			case 's':
				/* Would just use atoi(), but that doesn't deal with unsigned
				 * ints. Damn */
				if(sscanf(optarg, "%u", &opt->serial) != 1)
					opt->serial = 0; /* Failed, so just set to zero */
				break;
			case 't':
				opt->title = realloc(opt->title, (++opt->title_count)*sizeof(char *));
				opt->title[opt->title_count - 1] = strdup(optarg);
				break;
			case 'b':
   				if(sscanf(optarg, "%d", &opt->nominal_bitrate)
    					!= 1) {
	    			fprintf(stderr, _("Warning: nominal bitrate \"%s\" not recognised\n"), optarg);
		    		opt->nominal_bitrate = -1;
				}

				break;
			case 'm':
				if(sscanf(optarg, "%d", &opt->min_bitrate)
						!= 1) {
					fprintf(stderr, _("Warning: minimum bitrate \"%s\" not recognised\n"), optarg);
					opt->min_bitrate = -1;
				}
				if(!opt->managed){
				  if(!opt->quiet)
				    fprintf(stderr, 
					    _("Enabling bitrate management engine\n"));
				  opt->managed = 1;
				}
				break;
			case 'M':
				if(sscanf(optarg, "%d", &opt->max_bitrate)
						!= 1) {
					fprintf(stderr, _("Warning: maximum bitrate \"%s\" not recognised\n"), optarg);
					opt->max_bitrate = -1;
				}
				if(!opt->managed){
				  if(!opt->quiet)
				    fprintf(stderr, 
					    _("Enabling bitrate management engine\n"));
				  opt->managed = 1;
				}
				break;
			case 'q':
				if(sscanf(optarg, "%f", &opt->quality) != 1) {
					fprintf(stderr, _("Quality option \"%s\" not recognised, ignoring\n"), optarg);
					break;
				}
				opt->quality_set=1;
				opt->quality *= 0.1f;
				if(opt->quality > 1.0f)
				{
					opt->quality = 1.0f;
					fprintf(stderr, _("WARNING: quality setting too high, setting to maximum quality.\n"));
				}
				break;
			case 'n':
				if(opt->namefmt)
				{
					fprintf(stderr, _("WARNING: Multiple name formats specified, using final\n"));
					free(opt->namefmt);
				}
				opt->namefmt = strdup(optarg);
				break;
            case 'X':
				if(opt->namefmt_remove && opt->namefmt_remove != 
                        DEFAULT_NAMEFMT_REMOVE)
				{
					fprintf(stderr, _("WARNING: Multiple name format filters specified, using final\n"));
					free(opt->namefmt_remove);
				}
				opt->namefmt_remove = strdup(optarg);
				break;
            case 'P':
				if(opt->namefmt_replace && opt->namefmt_replace != 
                        DEFAULT_NAMEFMT_REPLACE)
                {
					fprintf(stderr, _("WARNING: Multiple name format filter replacements specified, using final\n"));
					free(opt->namefmt_replace);
				}
				opt->namefmt_replace = strdup(optarg);
				break;
			case 'o':
				if(opt->outfile)
				{
					fprintf(stderr, _("WARNING: Multiple output files specified, suggest using -n\n"));
					free(opt->outfile);
				}
				opt->outfile = strdup(optarg);
				break;
			case 'Q':
				opt->quiet = 1;
				break;
			case 'r':
				opt->rawmode = 1;
				break;
			case 'v':
				fprintf(stdout, VERSION_STRING);
				exit(0);
				break;
			case 'B':
				if (opt->rawmode != 1)
				{
					opt->rawmode = 1;
					fprintf(stderr, _("WARNING: Raw bits/sample specified for non-raw data. Assuming input is raw.\n"));
				}
				if(sscanf(optarg, "%u", &opt->raw_samplesize) != 1)
				{
					opt->raw_samplesize = 16; /* Failed, so just set to 16 */
					fprintf(stderr, _("WARNING: Invalid bits/sample specified, assuming 16.\n"));
				}
				if((opt->raw_samplesize != 8) && (opt->raw_samplesize != 16))
				{
					fprintf(stderr, _("WARNING: Invalid bits/sample specified, assuming 16.\n"));
				}
				break;
			case 'C':
				if (opt->rawmode != 1)
				{
					opt->rawmode = 1;
					fprintf(stderr, _("WARNING: Raw channel count specified for non-raw data. Assuming input is raw.\n"));
				}
				if(sscanf(optarg, "%u", &opt->raw_channels) != 1)
				{
					opt->raw_channels = 2; /* Failed, so just set to 2 */
					fprintf(stderr, _("WARNING: Invalid channel count specified, assuming 2.\n"));
				}
				break;
			case 'N':
				opt->tracknum = realloc(opt->tracknum, (++opt->track_count)*sizeof(char *));
				opt->tracknum[opt->track_count - 1] = strdup(optarg);
				break;
			case 'R':
				if (opt->rawmode != 1)
				{
					opt->rawmode = 1;
					fprintf(stderr, _("WARNING: Raw sample rate specified for non-raw data. Assuming input is raw.\n"));
				}
				if(sscanf(optarg, "%u", &opt->raw_samplerate) != 1)
				{
					opt->raw_samplerate = 44100; /* Failed, so just set to 44100 */
					fprintf(stderr, _("WARNING: Invalid sample rate specified, assuming 44100.\n"));
				}
				break;
			case '?':
				fprintf(stderr, _("WARNING: Unknown option specified, ignoring->\n"));
				break;
			default:
				usage();
				exit(0);
		}
	}

}

static void add_tag(vorbis_comment *vc, oe_options *opt,char *name, char *value)
{
	char *utf8;
	if(utf8_encode(value, &utf8) >= 0)
	{
		if(name == NULL)
			vorbis_comment_add( vc, utf8);
		else
			vorbis_comment_add_tag(vc, name, utf8);
		free(utf8);
	}
	else
		fprintf(stderr, _("Couldn't convert comment to UTF-8, cannot add\n"));
}

static void build_comments(vorbis_comment *vc, oe_options *opt, int filenum, 
		char **artist, char **album, char **title, char **tracknum, 
        char **date, char **genre)
{
	int i;

	vorbis_comment_init(vc);

	for(i = 0; i < opt->comment_count; i++)
		add_tag(vc, opt, NULL, opt->comments[i]);

	if(opt->title_count)
	{
		if(filenum >= opt->title_count)
		{
			if(!opt->quiet)
				fprintf(stderr, _("WARNING: Insufficient titles specified, defaulting to final title.\n"));
			i = opt->title_count-1;
		}
		else
			i = filenum;

		*title = opt->title[i];
		add_tag(vc, opt, "title", opt->title[i]);
	}

	if(opt->artist_count)
	{
		if(filenum >= opt->artist_count)
			i = opt->artist_count-1;
		else
			i = filenum;
	
		*artist = opt->artist[i];
		add_tag(vc, opt, "artist", opt->artist[i]);
	}

    if(opt->genre_count)
    {
        if(filenum >= opt->genre_count)
            i = opt->genre_count-1;
        else
            i = filenum;

        *genre = opt->genre[i];
        add_tag(vc, opt, "genre", opt->genre[i]);
    }

	if(opt->date_count)
	{
		if(filenum >= opt->date_count)
			i = opt->date_count-1;
		else
			i = filenum;
	
		*date = opt->dates[i];
		add_tag(vc, opt, "date", opt->dates[i]);
	}
	
	if(opt->album_count)
	{
		if(filenum >= opt->album_count)
		{
			i = opt->album_count-1;
		}
		else
			i = filenum;

		*album = opt->album[i];	
		add_tag(vc, opt, "album", opt->album[i]);
	}

	if(filenum < opt->track_count)
	{
		i = filenum;
		*tracknum = opt->tracknum[i];
		add_tag(vc, opt, "tracknumber", opt->tracknum[i]);
	}
}


