/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8_private\PhotoShop\dxt
File:  dxt.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#include "NVI_ImageLib.h"

#include "dxtlib.h"
#include <direct.h>
#include "string"
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include <sys/stat.h>

bool bUserSpecifiedDir = false;
bool bFlipTopToBottom = false;
bool bOutputSameDir = false;

#include "normalconvert.h"
#include "NormalMapFilterUI.h"


HRESULT compress_image_file(char * inputfile, int);
void compress_cubemap_from_list();
HRESULT compress_cube(int);

void ConvertToNormalMap(int kerneltype, int colorcnv, int alpha, float scale, int minz, bool wrap, int w, int h, void * data);

char * cubemap_name;

bool bImageMode = false;
static HFILE filein = 0;
bool timestamp = false;
bool bCubeMap = false;
bool bSwapRGB = false;

bool list = false;
std::string listfile;
int Mode24 = 0;
int Mode32 = 0;


HFILE fileout;
extern char* optarg;

bool bDuDvMap = false;
bool bNormalMap = false;
bool bBinaryAlpha = false;
bool bAlphaBorder = false;
bool bBorder = false;
tPixel BorderColor;
bool bFade = false;
tPixel FadeToColor;
bool bFadeAlpha = false;
int FadeAmount = 15;
bool bGenMipMaps = true;
DWORD TextureFormat = dDXT3;
bool bDitherColor = false;


bool bConversion = false;

int kerneltype = PSH_KERNEL_3x3;

int colorcnv = kDAVERAGE_RGB;


int alphaResult = kDAlphaNone;
/*const int16 kDAlphaNone = 33;
const int16 kDAlphaHeight = 34;
const int16 kDAlphaClear = 35;*/

float scale = 1.0;

int minz = 0;
// default 0
bool wrap = 0;


#include <shlobj.h>

char * PopupDirectoryRequestor(const char * initialdir,
                               const char * title)
{
    static char temp[MAX_PATH];
    static char dir[MAX_PATH];
    
    BROWSEINFO bi;
    LPITEMIDLIST list;
    
    //HINSTANCE hInstance = GetModuleHandle(NULL);
    //gd3dApp->Create( hInstance  );
    
    bi.hwndOwner = 0; 
    bi.pidlRoot = 0; 
    bi.pszDisplayName = dir; 
    bi.lpszTitle = title; 
    bi.ulFlags = 0; 
    bi.lpfn = 0; 
    bi.lParam = 0; 
    bi.iImage = 0; 
    
    list = SHBrowseForFolder(&bi);
    
    if (list == 0)
        return 0;
    
    SHGetPathFromIDList(list, dir);
    
    return dir;
}


char * search[] =
{
    "*.tga",
    "*.bmp",
    "*.gif",
    "*.ppm",
    "*.jpg",
    "*.jpeg",
    "*.tif",
    "*.tiff",
    "*.cel",
    "*.dds",
    0,
};

void compress_all( bool popup )
{
    
    if (popup)
    {
        char * dir;
        dir = PopupDirectoryRequestor(0,0);
        if (dir == 0)
            return;
        _chdir(dir);
    }
    
    struct _finddata_t filetga;
    long hFile;
    

    int i = 0;
    while(search[i])
    {
        // Find first .c file in current directory 
        hFile = _findfirst( search[i], &filetga );
        if (hFile != -1)
        {
            
            compress_image_file(filetga.name, TextureFormat);
            
            
            // Find the rest of the .c files 
            while( _findnext( hFile, &filetga ) == 0 )
            {
                compress_image_file(filetga.name, TextureFormat);
            }
            
            _findclose( hFile );
        }
        i++;
    }
}
  

void compress_list(  )
{
    
    FILE *fp = fopen( listfile.c_str(), "r");
    
    if (fp == 0)
    {
        fprintf(stdout, "Can't open list file <%s> [#1]\n", listfile.c_str());
        return;
    }
    
    char buff[1000];
    while(fgets(buff, 1000, fp))
    {      
        // has a crlf at the end
        int t = strlen(buff);
        buff[t - 1] = 0;


        compress_image_file(buff, TextureFormat);
    }

    fclose(fp);

}
  




void usage()
{
    fprintf(stdout,"NVDXT\n");
    fprintf(stdout,"This program\n");
    fprintf(stdout,"   compresses images\n");
    fprintf(stdout,"   creates normal maps from color or alpha\n");
    fprintf(stdout,"   creates DuDv map\n");
    fprintf(stdout,"   creates cube maps\n");
    fprintf(stdout,"   writes out .dds file\n");
    fprintf(stdout,"   does batch processing\n");
    fprintf(stdout,"   reads .tga, .bmp, .gif, .ppm, .jpg, .tif, .cel, .dds\n");
    fprintf(stdout,"   filters MIP maps\n");
    fprintf(stdout,"\n");
    fprintf(stdout,"Options:\n");
    fprintf(stdout,"  -nomipmap : don't generate MIP maps\n");
    fprintf(stdout,"  -fade : fade MIP maps\n");
    fprintf(stdout,"  -browse : browse for directory\n");
    fprintf(stdout,"  -dither : add dithering\n");
    fprintf(stdout,"  -flip : flip top to bottom \n");
    fprintf(stdout,"  -timestamp : Update only changed files\n");
    fprintf(stdout,"  -list <filename> : list of files to convert\n");
    fprintf(stdout,"  -cubemap <filename> : create cube map <filename>. Files specified with -list option\n");
    fprintf(stdout,"  -all : all image files in current directory\n");
    fprintf(stdout,"  -outdir : output directory\n");
    fprintf(stdout,"  -outsamedir : output directory same as input\n");
    fprintf(stdout,"  -file: input file to process\n");
    fprintf(stdout,"  -24 <dxt1c,dxt1a,dxt3,dxt5,u1555,u4444,u565,u8888, u888, u555> : compress 24 bit images with this format\n");
    fprintf(stdout,"  -32 <dxt1c,dxt1a,dxt3,dxt5,u1555,u4444,u565,u8888, u888, u555> : compress 32 bit images with this format\n");
    fprintf(stdout,"\n");

    fprintf(stdout,"  -swap : swap rgb\n");
    fprintf(stdout,"  -binaryalpha : treat alpha as 0 or 1\n");
    fprintf(stdout,"  -alphaborder : border images with alpha = 0\n");
    fprintf(stdout,"  -fade : fade map (color, normal or DuDv) over MIP levels\n");
    fprintf(stdout,"  -fadeamount : percentage to fade each MIP level. Default 15\n");
    fprintf(stdout,"  -fadecolor <hex color> : color to fade to\n");
    fprintf(stdout,"  -fadealpha : fade alpha over MIP levels\n");
    fprintf(stdout,"  -border : border images with color\n");
    fprintf(stdout,"  -bordercolor <hex color> : color for border\n");
    fprintf(stdout,"\n");
    
    

    fprintf(stdout,"Texture Format options. Default DXT3:\n");
    fprintf(stdout,"  -dxt1c : DXT1 (color only)\n");
    fprintf(stdout,"  -dxt1a : DXT1 (one bit alpha)\n");
    fprintf(stdout,"  -dxt3  : DXT3\n");
    fprintf(stdout,"  -dxt5  : DXT5\n\n");
    fprintf(stdout,"  -u1555 : uncompressed 1:5:5:5\n");
    fprintf(stdout,"  -u4444 : uncompressed 4:4:4:4\n");
    fprintf(stdout,"  -u565  : uncompressed 5:6:5\n");
    fprintf(stdout,"  -u8888 : uncompressed 8:8:8:8\n");
    fprintf(stdout,"  -u888  : uncompressed 0:8:8:8\n");
    fprintf(stdout,"  -u555  : uncompressed 0:5:5:5\n");
    fprintf(stdout,"\n");

    fprintf(stdout,"Optional Mip Map Filtering. Default box filter:\n");
    fprintf(stdout,"  -cubic  : cubic filtering\n");
    fprintf(stdout,"  -kaiser : kaiser filtering\n");
    fprintf(stdout,"  -gamma  : kaiser gamma filtering\n");
    fprintf(stdout,"  -full   : full DFT filtering\n");
    fprintf(stdout,"\n");

    
    
    fprintf(stdout,"***************************\n");
    fprintf(stdout,"To make a normal or dudv map, specify one of\n");
    fprintf(stdout,"  -n4 : normal map 4 sample\n");
    fprintf(stdout,"  -n3x3 : normal map 3x3 filter\n");
    fprintf(stdout,"  -n5x5 : normal map 5x5 filter\n");
    fprintf(stdout,"  -n7x7 : normal map 7x7 filter\n");
    fprintf(stdout,"  -n9x9 : normal map 9x9 filter\n");
    fprintf(stdout,"  -dudv : DuDv\n");
    fprintf(stdout,"\n");


    fprintf(stdout,"and source of height info:\n");
    fprintf(stdout,"  -alpha : alpha channel\n");
    fprintf(stdout,"  -rgb : average rgb\n");
    fprintf(stdout,"  -biased : average rgb biased\n");
    fprintf(stdout,"  -red : red channel\n");
    fprintf(stdout,"  -green : green channel\n");
    fprintf(stdout,"  -blue : blue channel\n");
    fprintf(stdout,"  -max : max of (r,g,b)\n");
    fprintf(stdout,"  -colorspace : mix of r,g,b\n");
    fprintf(stdout,"  -norm : normalize mip maps (source is a normal map)\n");
    fprintf(stdout,"\n");

    fprintf(stdout,"Normal/DuDv Map options:\n");
    fprintf(stdout,"  -aheight : store calculated height in alpha field\n");
    fprintf(stdout,"  -aclear : clear alpha channel\n");
    fprintf(stdout,"  -awhite : set alpha channel = 1.0\n");
    fprintf(stdout,"  -scale <float> : scale of height map. Default 1.0\n");
    fprintf(stdout,"  -wrap : wrap texture around. Default off\n");
    fprintf(stdout,"  -minz <int> : minimum value for up vector [0-255]. Default 0\n\n");


    fprintf(stdout,"Examples\n");
    fprintf(stdout,"  nvdxt -cubemap cubemap.dds -list cubemapfile.lst\n");
    fprintf(stdout,"  nvdxt -file test.tga -dxt1c\n");
    fprintf(stdout,"  nvdxt -file height_field_in_alpha.tga -n3x3 -alpha -scale 10 -wrap\n");
    fprintf(stdout,"  nvdxt -file grey_scale_height_field.tga -n5x5 -rgb -scale 1.3\n");
    fprintf(stdout,"  nvdxt -file normal_map.tga -norm\n");
    fprintf(stdout,"  nvdxt -file image.tga -dudv -fade -fadeamount 10\n");
    fprintf(stdout,"  nvdxt -all -dxt3 -gamma -outdir .\\dds_dir -time\n");


    fprintf(stdout,"version 4.4\n");
    fprintf(stdout,"send comments, bug fixes and feature requests to doug@nvidia.com\n");




}



extern int errno;
std::string output_dirname;


HRESULT callback(void * data, int miplevel, DWORD size)
{
	DWORD * ptr = (DWORD *)data;
	for(int i=0; i< size/4; i++)
	{
		DWORD c = *ptr++;
	}

	
	return 0;
}

/*For a USizexVSize texture, I create a buffer of USize*VSize DWORD's and
fill it with 0xffffffff. I would expect this to give me an all white
texture. Instead, I get an orange texture with a bunch of lavender dots.
Coincidentally (well, not really), all the dots are on the bottom right
corner of a 4x4 block*/
    /*
void test()
{
	DWORD raw_data[128 * 128];

	for(int i=0; i< 128 * 128; i++)
	{
		raw_data[i] = 0xFFFFFFFF;
	}
    
    file = _open( "test.dds", _O_WRONLY | _O_BINARY | _O_CREAT,  _S_IWRITE );

    HRESULT hr = nvDXTcompress((unsigned char *)raw_data, //  pointer to data (24 or 32 bit)
            128, // width in texels
            128, // height in texels
            TF_DXT1,
            true,
            false,
            4, 
            0);
    

    _close(file);
    
      exit(0);
}        */

int mipfilter;
bool all_files = false;
bool browse = false;
std::string inputfile;

HRESULT process_command_line(int argc, char * argv[])
{
    
    int i;
    i = 1;
    while(i<argc)
    {
        char * token = argv[i];
        
        if (strcmp(token, "-binaryalpha") == 0)
        {
            bBinaryAlpha = true;
        }
        else if (strcmp(token, "-alphaborder") == 0)
        {
            bAlphaBorder = true;
        }
        else if (strcmp(token, "-flip") == 0)
        {
            bFlipTopToBottom = true;
        }
        else if (strcmp(token, "-border") == 0)
        {
            bBorder = true;
        }
        else if (strcmp(token, "-fadeamount") == 0)
        {
            FadeAmount = atoi(argv[i+1]);
            i++;
        }
        else if (strcmp(token, "-bordercolor") == 0)
        {
            BorderColor.u = strtoul(argv[i+1], 0, 16);
            i++;
        }
        else if (strcmp(token, "-fadealpha") == 0)
        {
            bFadeAlpha = true;
        }
        else if (strcmp(token, "-fade") == 0)
        {
            bFade = true;
        }
        else if (strcmp(token, "-fadecolor") == 0)
        {
            FadeToColor.u = strtoul(argv[i+1], 0, 16);
            i++;
        }
        else if (strcmp(token, "-nomipmap") == 0)
        {
            bGenMipMaps = false;
        }
        else if (strcmp(token, "-dxt1a") == 0)
        {
            TextureFormat =  dDXT1a;
        }
        else if ((strcmp(token, "-dxt1c") == 0) || (strcmp(token, "-dxt1") == 0))
        {
            TextureFormat =  dDXT1;
        }
        else if (strcmp(token, "-dxt3") == 0)
        {
            TextureFormat =  dDXT3;
        }
        else if (strcmp(token, "-dxt5") == 0)
        {
            TextureFormat =  dDXT5;
        }
        else if (strcmp(token, "-cubemap") == 0)
        {
            bCubeMap = true;
            cubemap_name = argv[i+1];
            i++;
        }
        else if (strcmp(token, "-swap") == 0)
        {
            bSwapRGB = true;
        }
        else if (strcmp(token, "-all") == 0)
        {
            all_files = true;
        }
        else if (strcmp(token, "-browse") == 0)
        {
            browse = true;
        }
        else if (strcmp(token, "-dither") == 0)
        {
            bDitherColor = true;
        }
        else if (strcmp(token, "-outdir") == 0)
        {
            output_dirname = argv[i+1];
            i++;

            bUserSpecifiedDir = true;
        }
        else if (strcmp(token, "-outsamedir") == 0)
        {
            bOutputSameDir = true;
        }
        else if (strcmp(token, "-u4444") == 0)
        {
            TextureFormat =  d4444;
        }
        else if (strcmp(token, "-u1555") == 0)
        {
            TextureFormat =  d1555;
        }
        else if (strcmp(token, "-u565") == 0)
        {
            TextureFormat =  d565;
        }
        else if (strcmp(token, "-u8888") == 0)
        {
            TextureFormat =  d8888;
        }
        else if (strcmp(token, "-u888") == 0)
        {
            TextureFormat =  d888;
        }
        else if (strcmp(token, "-u555") == 0)
        {
            TextureFormat =  d555;
        }
        else if (strcmp(token, "-timestamp") == 0)
        {
            timestamp = true;
        }
        else if (strcmp(token, "-list") == 0)
        {
            list = true;
            
            listfile = argv[i+1];
            i++;
        }
        else if (strcmp(token, "-cubic") == 0)
        {
            mipfilter = dMIPFilterCubic;
        }
        else if (strcmp(token, "-kaiser") == 0)
        {
            mipfilter = dMIPFilterKaiser;
        }
        else if (strcmp(token, "-gamma") == 0)
        {
            mipfilter = dMIPFilterLinearLightKaiser;
        }
        else if (strcmp(token, "-full") == 0)
        {
            mipfilter = dMIPFilterFullDFT;
            
        }
        else if (strcmp(token, "-n4") == 0)
        {
            kerneltype = PSH_KERNEL_4x;
            bNormalMap = true;
            bDuDvMap = false;
            bConversion = true;
        }
        else if (strcmp(token, "-n3x3") == 0)
        {
            kerneltype = PSH_KERNEL_3x3;
            bConversion = true;
            bNormalMap = true;
            bDuDvMap = false;
        }
        else if (strcmp(token, "-n5x5") == 0)
        {
            kerneltype = PSH_KERNEL_5x5;                               
            bConversion = true;
            bNormalMap = true;
            bDuDvMap = false;
        }
        else if (strcmp(token, "-n7x7") == 0)
        {
            kerneltype = PSH_KERNEL_7x7;                               
            bConversion = true;
            bNormalMap = true;
            bDuDvMap = false;
        }
        else if (strcmp(token, "-n9x9") == 0)
        {
            kerneltype = PSH_KERNEL_9x9;                               
            bConversion = true;
            bNormalMap = true;
            bDuDvMap = false;
        }
        else if (strcmp(token, "-dudv") == 0)
        {
            kerneltype = PSH_KERNEL_DuDv;
            bConversion = true;
            bDuDvMap = true;
            bNormalMap = false;
        }
        else if (strcmp(token, "-alpha") == 0)
        {
            colorcnv = kDALPHA;
        }
        else if (strcmp(token, "-rgb") == 0)
        {
            colorcnv = kDAVERAGE_RGB;
        }
        else if (strcmp(token, "-biased") == 0)
        {

            colorcnv = kDBIASED_RGB;
        }
        else if (strcmp(token, "-red") == 0)
        {
            colorcnv = kDRED;
        }
        else if (strcmp(token, "-green") == 0)
        {
            colorcnv = kDGREEN;
        }
        else if (strcmp(token, "-blue") == 0)
        {
            colorcnv = kDBLUE;
        }
        else if (strcmp(token, "-max") == 0)
        {
            colorcnv = kDMAX;
        }
        else if (strcmp(token, "-colorspace") == 0)
        {
            colorcnv = kDCOLORSPACE;
        }
        else if (strcmp(token, "-norm") == 0)
        {
            colorcnv = kDNORMALIZE;
        }

        else if (strcmp(token, "-aheight") == 0)
        {
            alphaResult = kDAlphaHeight;
        }
        else if (strcmp(token, "-aclear") == 0)
        {
            alphaResult = kDAlphaClear;
        }
        else if (strcmp(token, "-awhite") == 0)
        {
            alphaResult = kDAlphaWhite;
        }
            
        else if (strcmp(token, "-file") == 0)
        {
            inputfile = argv[i+1];
            i++;
        }
        else if (strcmp(token, "-24") == 0)
        {
            char * mode = argv[i+1];
            i++;
            
            if (strcmp(mode, "dxt1c") == 0)
            {
                Mode24 = dDXT1;
            }
            else if (strcmp(mode, "dxt1") == 0)
            {
                Mode24 = dDXT1;
            }
            else if (strcmp(mode, "dxt1a") == 0)
            {
                Mode24 = dDXT1a;
            }
            else if (strcmp(mode, "dxt3") == 0)
            {
                Mode24 = dDXT3;
            }
            
            else if (strcmp(mode, "dxt5") == 0)
            {
                Mode24 = dDXT5;
            }
            
            else if (strcmp(mode, "u1555") == 0)
            {
                Mode24 = d1555;
            }
            
            else if (strcmp(mode, "u4444") == 0)
            {
                Mode24 = d4444;
            }
            
            else if (strcmp(mode, "u565") == 0)
            {
                Mode24 = d565;
            }
            
            else if (strcmp(mode, "u8888") == 0)
            {
                Mode24 = d8888;
            }
            else if (strcmp(mode, "u888") == 0)
            {
                Mode24 = d888;
            }
            else if (strcmp(mode, "u555") == 0)
            {
                Mode24 = d555;
            }
            else
            {
                printf("Unknown format %s, using DXT1\n", mode);
                Mode24 = dDXT1;
            }

        }
        else if (strcmp(token, "-32") == 0)
        {
            char * mode = argv[i+1];
            i++;
            
            if (strcmp(mode, "dxt1c") == 0)
            {
                Mode32 = dDXT1;
            }
            if (strcmp(mode, "dxt1") == 0)
            {
                Mode32 = dDXT1;
            }
            else if (strcmp(mode, "dxt1a") == 0)
            {
                Mode32 = dDXT1a;
            }
            else if (strcmp(mode, "dxt3") == 0)
            {
                Mode32 = dDXT3;
            }
            
            else if (strcmp(mode, "dxt5") == 0)
            {
                Mode32 = dDXT5;
            }
            
            else if (strcmp(mode, "u1555") == 0)
            {
                Mode32 = d1555;
            }
            
            else if (strcmp(mode, "u4444") == 0)
            {
                Mode32 = d4444;
            }
            
            else if (strcmp(mode, "u565") == 0)
            {
                Mode32 = d565;
            }
            
            else if (strcmp(mode, "u8888") == 0)
            {
                Mode32 = d8888;
            }
            else if (strcmp(mode, "u888") == 0)
            {
                Mode32 = d888;
            }
            else if (strcmp(mode, "u555") == 0)
            {
                Mode32 = d555;
            }
            else
            {
                printf("Unknown format %s, using DXT1\n", mode);
                Mode32 = dDXT1;
            }

        }
        else if (strcmp(token, "-scale") == 0)
        {
            scale = atof(argv[i+1]);
            i++;
                
        }
        else if (strcmp(token, "-wrap") == 0)
        {
            wrap = true;
                
        }
        else if (strcmp(token, "-minz") == 0)
        {
            minz = atoi(argv[i+1]);
            i++;
                
        }
        else if (token[0] == '-')
        {
            fprintf(stdout, "Unknown option '%s'\n", token);
            return -1;
            
        }
        else
        {
            //  must be a file name
            inputfile = argv[i];
        }
        i++;
    }
    return 0;
} 
    
int main(int argc, char * argv[])
{
    
    inputfile = "";
    BorderColor.u = 0;
    FadeToColor.u = 0;
    if (argc == 1)
    {
        usage();
        return 0;
    }
    
    output_dirname = ".";

    HRESULT hr = process_command_line(argc, argv);
    if (hr < 0)
    {
        //usage();
        return 0;
    }

    

    mipfilter = dMIPFilterBox;    

     
    if (bUserSpecifiedDir == true)
    {
        
        char cwd[1000];
        char * t;
        t = _getcwd(cwd, 1000);


        int md = _chdir(output_dirname.c_str());
        
        _chdir(cwd);

        if (md != 0)
        {
            md = _mkdir(output_dirname.c_str());
            
            if (md == 0)
            {
                fprintf(stdout, "directory %s created\n", output_dirname.c_str());
                fflush(stdout);
            }
            else if (errno != EEXIST)
            {
                fprintf(stdout, "problem with output directory %s\n", output_dirname.c_str());
                return 0;
            } 
            else
            {
                fprintf(stdout, "output directory %s\n", output_dirname.c_str());
                fflush(stdout);
            }
        }
    }
    
    if (list)
    {
        if (bCubeMap)
            compress_cubemap_from_list();
        else
            compress_list();
    }
    else if (all_files)
    {
        compress_all(false);
    }
    else if (browse)
    {
        compress_all(true);
    }
    else if (inputfile.size() > 0)
    {
        //inputfile = argv[optind];
        compress_image_file(const_cast<char *>(inputfile.c_str()), TextureFormat);
    }
    else
        fprintf(stdout, "nothing to do\n");

    
    return 0;
}



char * extensions[] =
{
    ".tga",
    ".bmp",
    ".gif",
    ".ppm",
    ".jpg",
    ".jpeg",
    ".tif",
    ".tiff",
    ".cel",
    ".dds",
    0,
};



std::string get_input_filename(char *inputfile)
{
    std::string temp;
    temp = strlwr(inputfile);

    //if (strip)
    {
        int pos;
        pos = temp.find_last_of("\\");

        if (pos == -1)
            pos = temp.find_last_of("/");

        std::string temp2;
        if (pos != -1)
        {
            temp2 = temp.substr(pos+1);
            temp = temp2;
        }


    }
    return temp;

}

std::string get_input_dir(char *inputfile)
{
    std::string temp;
    temp = strlwr(inputfile);

    //if (strip)
    {
        int pos;
        pos = temp.find_last_of("\\");

        if (pos == -1)
            pos = temp.find_last_of("/");

        std::string temp2;
        if (pos != -1)
        {
            temp2 = temp.substr(0, pos);
            temp = temp2;
        }


    }
    return temp;

}



void FlipTopToBottom(UINT bpp, UINT width, UINT height, BYTE * pArray)
{

	BYTE * swap = new BYTE[ width * bpp ];

	DWORD row;
	BYTE * end_row;
	BYTE * start_row;

	for( row = 0; row < height / 2; row ++ )
	{
		end_row =   & (pArray[ bpp*width*( height - row - 1) ]);
		start_row = & (pArray[ bpp * width * row ]);

		// copy row toward end of image into temporary swap buffer
		memcpy( swap, end_row, bpp * width );

		// copy row at beginning to row at end
		memcpy( end_row, start_row, bpp * width );

		// copy old bytes from row at end (in swap) to row at beginning
		memcpy( start_row, swap, bpp * width );
	}

	SAFE_ARRAY_DELETE( swap );
}




// return planes from original image, planes is 4
HRESULT read_image(char * inputfile, bool isdds,
                           int &width, int &height, int & pitch, int & image_planes,
                           DWORD * & out_data)
{
    NVI_TGA_File		tga;
    NVI_GraphicsFile	bmp;
    int image_pitch;

    unsigned char * image;
    //int image_planes;

    if (isdds)
    {                                         
        filein = _open( inputfile, _O_RDONLY | _O_BINARY ,  _S_IREAD );
        
        if (filein == -1)
        {
            fprintf(stdout, "Can't open intput file '%s' [#2]\n", inputfile);
            return -1;
        }
        
        int lTotalWidth; 
        int rowBytes;
        
        image = nvDXTdecompress(width, height, image_planes, lTotalWidth, rowBytes);
        
        if (image == 0)
        {
            fprintf(stdout, "Can't open input file '%s'[#3]\n", inputfile);
            return -1;
        }

        image_pitch = lTotalWidth * image_planes;

        out_data = new DWORD[height * width];

        DWORD * out_line = out_data;
        unsigned char *image_line = image;

        if (image_planes == 4)
        {
            for(int y = 0; y<height; y++)
            {
                memcpy(out_line, image_line,  width * 4);

                out_line += width;
                image_line += pitch;
            }
        }
        else
        {          

            unsigned char * image_pix;
            unsigned char * out_ptr = (unsigned char*)out_data;
            unsigned char * image_line = image;

            for(int y=0; y < height; y++)
            {
                image_pix = image_line;
                for(int x=0; x < width; x++)
                {
                    *out_ptr++ = *image_pix++;
                    *out_ptr++ = *image_pix++;
                    *out_ptr++ = *image_pix++;
                    *out_ptr++ = 0xFF;

                }
                image_line += image_pitch;
            }
            
        }

        if (bFlipTopToBottom)
        {
            FlipTopToBottom(4, width, height, (BYTE *)out_data);
        }


            
        pitch = width * 4;
        
    }
    else
    {
        image = (unsigned char *)tga.ReadFile(inputfile, UL_GFX_PAD);
        
        //raw_data = read_tga(inputfile, w, h); 

        
        if (image == 0)
        {
            image = (unsigned char *)bmp.ReadFile(inputfile, UL_GFX_PAD);
            
            if (image == 0)
            {
                fprintf(stdout, "Can't open input file '%s'[#4]\n", inputfile);
                return -1;
            }
            image_planes = bmp.GetBytesPerPixel() / 8;
            
            width = bmp.GetWidth();
            height = bmp.GetHeight();

            if (bFlipTopToBottom)
                bmp.FlipTopToBottom();
        }  
        else
        {
            width = tga.GetWidth();
            height = tga.GetHeight();
            image_planes = tga.GetBytesPerPixel() / 8;

            if (bFlipTopToBottom)
                tga.FlipTopToBottom();

        }


        out_data = new DWORD[height * width];
        memcpy(out_data, image, height * width * 4);

        pitch = width * 4;
        /*

        unsigned char * dataline = raw_data;
        int linewidth = width * planes;

        for(int y = 0; y<height; y++)
        {
            memcpy(dataline, &image[y * pitch],  linewidth);
            out_line += width;
        } */

    }
    return 0;
}
        


HRESULT compress_image_file(char * inputfile, int localTextureFormat)
{
    int width, height; 
    int pitch;

    DWORD * raw_data;



    std::string temp = get_input_filename(inputfile);
        

    bool isdds = false;
    if (temp.find(".dds") != -1)
        isdds = true; 


    
    // determine output file name

    int i = 0;
    int pos = -1;
    while(extensions[i] != 0 && pos == -1)
    {
        pos = temp.find(extensions[i]);
        i++;
    }

    if (pos == -1)
    {
        fprintf(stdout, "Can't open input file '%s'[#5]\n", inputfile);
        return 0;
    }

    std::string ddsname, finalname_out;

    ddsname = temp.substr(0, pos);

    if (isdds)
        ddsname.append("_");

    ddsname.append(".dds");


    if (bOutputSameDir)
    {
        output_dirname =  get_input_dir(inputfile);

    }


    finalname_out = output_dirname;
    finalname_out.append("\\");
    finalname_out += ddsname;


    struct _finddata_t filedataSrc;
    struct _finddata_t filedataDest;


    if (timestamp)
    {
        // compare times 

        long hFileSrc;
        long hFileDest;
        hFileSrc = _findfirst(inputfile, &filedataSrc );
        hFileDest = _findfirst(finalname_out.c_str(), &filedataDest );


        if (hFileDest != -1)
        {
            // if it exists and is newer then return
            if (filedataSrc.time_write < filedataDest.time_write)
            {
                fprintf(stdout, "<%s> is up to date\n", finalname_out.c_str());
                return 0;
            }
        }


    }


    fprintf(stdout, "%s --> ",inputfile);
    fflush(stdout);


    int image_planes;
    HRESULT hr = read_image(inputfile, isdds,
                           width, height, pitch, image_planes,
                           raw_data);

    if (hr != 0)
        return hr;

    bImageMode = false;

    if( (IsPower2(width) == false) || (IsPower2(height) == false) )
    {
        // only a warning in uncompressed
        fprintf(stdout, "Image is not a power of 2 (%d x %d)\n", width, height);
        bImageMode = true;


        switch(localTextureFormat)
        {
        case dDXT1:  
        case dDXT1a:
        case dDXT3:
        case dDXT5:
            return -1;
        default:
            fprintf(stdout, "MIP maps are disabled for this image\n");
            break;

        }
    }

    if (Mode24 && image_planes == 3)
    {
        localTextureFormat = Mode24;
    }

    if (Mode32 && image_planes == 4)
    {
        localTextureFormat = Mode32;
    }




    fileout = _open( finalname_out.c_str(), _O_WRONLY | _O_BINARY | _O_CREAT | _O_TRUNC,  _S_IWRITE );

    fprintf(stdout, "%s\n", finalname_out.c_str());

    if (fileout== -1)
    {
        fprintf(stdout, "Can't open output file '%s'[#6]\n", finalname_out.c_str());
        return 0;
    }
        

    if (Mode24 && image_planes == 3)
    {
        localTextureFormat = Mode24;
    }

    if (Mode32 && image_planes == 4)
    {
        localTextureFormat = Mode32;
    }

              

    CompressionOptions options;

    options.bMipMapsInImage = false;  // mip have been loaded in during read
    options.MIPFilterType = mipfilter;

    if (bImageMode == true || bGenMipMaps == false)
        options.MipMapType = dNoMipMaps;      // dNoMipMaps, dUseExistingMipMaps, dGenerateMipMaps
    else
        options.MipMapType = dGenerateMipMaps;      // dNoMipMaps, dUseExistingMipMaps, dGenerateMipMaps

    options.bBinaryAlpha = bBinaryAlpha;    // clamp alpha zero or one 

    options.bNormalMap = bNormalMap;      // only renormalize MIP maps
    options.bDuDvMap = bDuDvMap;      // only renormalize MIP maps

    options.bAlphaBorder = bAlphaBorder;    // make an alpha border
    options.bBorder = bBorder;         // make a color border
    options.BorderColor = BorderColor;        // color of border


    options.bFade = bFade;               // fade to color over MIP maps
    options.bFadeAlpha = bFadeAlpha;               // fade to color over MIP maps
    options.FadeToColor = FadeToColor;        // color to fade to
    options.FadeAmount = FadeAmount;      // number of MIPs to fade over


    options.bDitherColor = bDitherColor;        // enable dithering during 16 bit conversion

	options.TextureType = dTextureType2D;    // regular decal, cube or volume  
	//dTextureType2D 
	//dTextureTypeCube 
	//dTextureTypeImage 

    options.TextureFormat = localTextureFormat;
	//  dDXT1, dDXT1a, dDXT3, dDXT5, d4444, 
	//  d1555, 	d565,	d8888, 	d888, 	d555, 


    options.bAlphaBorder = bAlphaBorder;
    options.bSwapRGB = bSwapRGB;


    if (bConversion)
        ConvertToNormalMap(kerneltype, colorcnv, alphaResult, scale, minz, wrap, width, height, raw_data);


    nvDXTcompress((unsigned char *)raw_data, width, height, pitch, &options, 4, 0);



    if (filein)
        _close(filein);

    _close(fileout);

    delete [] raw_data;

    return 0;
}
void WriteDTXnFile(DWORD count, void *buffer)
{
    _write(fileout, buffer, count);

}


void ReadDTXnFile(DWORD count, void *buffer)
{
        
    _read(filein, buffer, count);

}



/*void ReadDTXnFile (DWORD count, void *buffer)
{
    // stubbed, we are not reading files
}*/


/////////////////////////////
       /*

HRESULT MyMIPcallback(void * data, int miplevel, DWORD size)
{
	const DWORD CountBlock = size/8;
	for (DWORD iBlock=0; iBlock<CountBlock; iBlock++)
	{
		unsigned short*	Colors = (unsigned short*)data;
		if (Colors[0]>Colors[1])	
            printf("C0=0x%04X C1=0x%04X|DXT1  ", Colors[0], Colors[1]);
		else						
            printf("C0=0x%04X C1=0x%04X|DXT1a ", Colors[0], Colors[1]);

		// R5G6B5
		printf("C0=(0x%02X, 0x%02X, 0x%02X)   C1=(0x%02x, 0x%02x, 0x%02x)\n",
			((Colors[0]>>11) & 0x1F) << 3, ((Colors[0]>>5) & 0x3F) << 2, (Colors[0] & 0x1F) << 3 , 
			((Colors[1]>>11) & 0x1F) << 3, ((Colors[1]>>5) & 0x3F) << 2, (Colors[1] & 0x1F) << 3);
		((BYTE*&)data) += 8;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	const DWORD			Width=2, Height=2;
	BYTE				Data[Width*Height*3];
	BYTE				Data4[4*4*3];
	CompressionOptions	Options;
	BYTE				Values[] = {0x00,0x10,0x14,0x15,0x20,0x30,0x40,0x80,0xc0};
	const DWORD			cValue = sizeof(Values)/sizeof(Values[0]);
	Options.MipMapsInImage	= false;
	Options.MipMapType		= dNoMipMaps;
	Options.BinaryAlpha		= false;
	Options.NormalMap		= false;
	Options.AlphaBorder		= false;
	Options.Border			= false;
	Options.Fade			= false;
	Options.DitherColor	    = false;
	Options.TextureType		= dTextureType2D;
	Options.TextureFormat	= dDXT1;
	for (DWORD iValue=0; iValue<cValue; iValue++)
	{
		printf("0x%02X => ", Values[iValue]);
		memset(Data, Values[iValue], Width*Height*3);
		nvDXTcompress(Data, Width, Height, Width*3, &Options, 3, &MyMIPcallback);


		memcpy(Data4,		Data,		 2*3);	// texels 0,0 & 0,1
		memset(Data4+2*3,	0,			 2*3);
		memcpy(Data4+4*3,	Data+2*3,	 2*3);	// texels 1,0 & 1,1
		memset(Data4+6*3,	0,			10*3);
		printf("0x%02Xp=> ", Values[iValue]);
		nvDXTcompress(Data4, 4, 4, 4*3, &Options, 3, &MyMIPcallback);
	}
	return 0;
}       */



std::string cube_names[6]; 

void compress_cubemap_from_list()
{
    
    FILE *fp = fopen( listfile.c_str(), "r");
    
    if (fp == 0)
    {
        fprintf(stdout, "Can't open list file <%s>\n", listfile.c_str());
        return;
    }
    
    char buff[1000];

    int i = 0;
    while(fgets(buff, 1000, fp))
    {      
        // has a crlf at the end
        int t = strlen(buff);
        buff[t - 1] = 0;

        cube_names[i] = buff;
        i++;

    }

    fclose(fp);

    if (i != 6)
    {
        fprintf(stdout, "There are not six images in listfile '%s'\n", listfile.c_str());
        return;
    }

    compress_cube(TextureFormat);



}



HRESULT compress_cube(int localTextureFormat)
{
    int width, height; 
    int pitch;
    //int planes = 4;

    DWORD * raw_data[6];


    char * inputfile = 0;
    int w1, h1;

    for(int i=0; i<6; i++)
    {
        
        inputfile = const_cast<char *>(cube_names[i].c_str());
        std::string temp = get_input_filename(inputfile);
        
        
        bool isdds = false;
        if (temp.find(".dds") != -1)
            isdds = true; 
        
        
        
        fprintf(stdout, "%s",inputfile);
        if (i == 5)
            fprintf(stdout, " -> ");
        else
            fprintf(stdout, ", ");

        fflush(stdout);
        
        
        int image_planes;
        HRESULT hr = read_image(inputfile, isdds,
            width, height, pitch, image_planes,
            raw_data[i]);
        
        if (hr != 0)
            return hr;

        if (width != height)
        {
            fprintf(stdout, "Image is not square. Width %d, Height %d\n", width, height);
            exit(1);
            
        }
        if (i == 0)
        {
            h1 = height;
            w1 = width;
        }
        else
        {
            if (width != w1 && height != h1)
            {
                fprintf(stdout, "Images are not the same size\n");
                exit(1);
            }
        }
      
        
        if ((IsPower2(width) == false) || (IsPower2(height) == false))
        {
            // only a warning in uncompressed
            fprintf(stdout, "Image is not a power of 2 (%d x %d)\n", width, height);
            exit(1);
            
        }
    }
        
        
      
    int cube_pitch = width * 6;
    DWORD *data = new DWORD[height * cube_pitch];
    
    for(i=0; i<6; i++)
    {
        DWORD *ptr = raw_data[i];
        
        for(int y = 0; y<height; y++)
        {
            memcpy(&data[y * cube_pitch + i * width], &ptr[y * width],  width * 4);
            //for(int x = 0; x<width; x++)
            //{
            //    data[y * cube_pitch + i * width + x] = *ptr++;
            //}
        }
    }
    
    
    /*{
        // set to zero
        memset(data, 0xFF, height * width * 4 * 6);
        
        /*int i = 0;
        for(int y = 0; y<height; y++)
            for(int x = 0; x<width; x++)
            {
                data[y * cube_pitch + i * width + x] = 0xFF00FF00;

            }
    }*/
    
    
        
    char * outputfile = cubemap_name;

    fileout = _open( outputfile, _O_WRONLY | _O_BINARY | _O_CREAT,  _S_IWRITE );

    fprintf(stdout, "%s\n", outputfile);

    if (fileout== -1)
    {
        fprintf(stdout, "Can't open output file %s\n", outputfile);
        return 0;
    }
 

        
              

    CompressionOptions options;

    options.bMipMapsInImage = false;  // mip have been loaded in during read

    if (bGenMipMaps == false)
        options.MipMapType = dNoMipMaps;      // dNoMipMaps, dUseExistingMipMaps, dGenerateMipMaps
    else
        options.MipMapType = dGenerateMipMaps;      // dNoMipMaps, dUseExistingMipMaps, dGenerateMipMaps
    
    options.bBinaryAlpha = bBinaryAlpha;
    options.MIPFilterType = mipfilter;


    options.bBinaryAlpha = bBinaryAlpha;    // clamp alpha zero or one 

    options.bNormalMap = bNormalMap;      // only renormalize MIP maps
    options.bDuDvMap = bDuDvMap;      // only renormalize MIP maps

    options.bAlphaBorder = bAlphaBorder;    // make an alpha border
    options.bBorder = bBorder;         // make a color border
    tPixel color;
    color.u = 0;
    options.BorderColor = BorderColor;        // color of border


    options.bFade = bFade;               // fade to color over MIP maps
    options.bFadeAlpha = bFadeAlpha;
    options.FadeToColor = FadeToColor;        // color to fade to
    options.FadeAmount = FadeAmount;      // percentage to fade each time


    options.bDitherColor = bDitherColor;        // enable dithering during 16 bit conversion

	options.TextureType = dTextureTypeCube;    // regular decal, cube or volume  
	//dTextureType2D 
	//dTextureTypeCube 
	//dTextureTypeImage 

    options.TextureFormat = localTextureFormat;
	//  dDXT1, dDXT1a, dDXT3, dDXT5, d4444, 
	//  d1555, 	d565,	d8888, 	d888, 	d555, 


    options.bSwapRGB = bSwapRGB;


        
    if (bConversion)
        ConvertToNormalMap(kerneltype, colorcnv, alphaResult, scale, minz, wrap, width * 6, height, data);

    nvDXTcompress((unsigned char *)data, width * 6, height, cube_pitch * 4, &options, 4, 0);


    if (filein)
        _close(filein);

    _close(fileout);

    for(i=0; i<6; i++)
    {
        delete [] raw_data[i];
    }

    return 0;
}


