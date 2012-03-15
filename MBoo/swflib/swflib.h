#ifndef _SWFLIB_H_
#define _SWFLIB_H_

#ifdef __cplusplus
	extern "C" {
#endif 

#define READER_TYPE_FILE	1
#define READER_TYPE_FILE2	6

typedef signed int 	S32;
typedef unsigned int 	U32;
typedef unsigned short 	U16;
typedef unsigned char 	U8;

#define LE_16_TO_NATIVE(x)	(x)

#define GET32(ptr) (((U16)(((U8*)(ptr))[0]))+(((U16)(((U8*)(ptr))[1]))<<8)+(((U16)(((U8*)(ptr))[2]))<<16)+(((U16)(((U8*)(ptr))[3]))<<24))

typedef struct _reader
{
	int (*read)(struct _reader*, void* data, int len);
	int (*seek)(struct _reader*, int pos);
	void (*dealloc)(struct _reader*);

	void* internal;
	int type;
	unsigned char	mybyte;
	unsigned char	bitpos;
	int	pos;
} reader_t;

void reader_init_filereader(reader_t*, int);

typedef signed	SFIXED;
typedef signed	SCOORD;

typedef struct _SRECT
{
	SCOORD	xmin;
	SCOORD	ymin;
	SCOORD	xmax;
	SCOORD	ymax;
} SRECT;

typedef struct _TAG
{
   U16		id;
   U8*		data;
   U32		memsize;

   U32		len;
   U32		pos;
   struct _TAG * next;
   struct _TAG * prev;

   U8		readBit;
   U8		writeBit;
} TAG;

typedef struct _SWF
{
	U8	fileVersion;
	U8	compressed; /* SWF or SWC */
	U32	fileSize;
	SRECT	movieSize;
	U16	frameRate;
	U16	frameCount;
	TAG* firstTag;
	U32	fileAttributes;
} SWF;

int swf_ReadSWFInfo(TCHAR*, SWF*);

#ifdef __cplusplus
	}
#endif 

#endif //_SWFLIB_H_
