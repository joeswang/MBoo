#ifndef _SWFLIB_H_
#define _SWFLIB_H_

typedef signed int 	S32;
typedef unsigned int 	U32;
typedef unsigned short 	U16;
typedef unsigned char 	U8

typedef signed	SFIXED;
typedef signed	SCOORD;

typedef struct _SRECT
{
	SCOORD	xmin;
	SCOORD	ymin;
	SCOORD	xmax;
	SCOORD	ymax;
} SRECT;

typedef struct _SWF
{
	U8	fileVersion;
	U8	compressed; /* SWF or SWC */
	U32	fileSize;
	SRECT	movieSize;
	U16	frameRate;
	U16	frameCount;
	U32	fileAttributes;
} SWF;

int swf_ReadSWFInfo(int handle, SWF* swf);

#endif //_SWFLIB_H_
