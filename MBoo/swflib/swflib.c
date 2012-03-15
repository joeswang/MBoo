#include <io.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <tchar.h>
#include <share.h>
//#include <sys/stat.h>

#include "swflib.h"

static int reader_fileread(reader_t* r, void* data, int len)
{
int ret;

	ret = _read((int)r->internal, data, len);
	if(ret >= 0) r->pos += ret;

	return ret;
}

static long reader_fileread_seek(reader_t* r, int pos)
{
	return _lseek((int)r->internal, pos, SEEK_SET);
}

static void reader_fileread_dealloc(reader_t* r)
{
	if(READER_TYPE_FILE2 == r->type)
	{
		_close((int)r->internal);
	}
	memset(r, 0, sizeof(reader_t));
}

void reader_init_filereader(reader_t* r, int handle)
{
	r->read = reader_fileread;
	r->seek = reader_fileread_seek;
	r->dealloc = reader_fileread_dealloc;
	r->internal = (void*)handle;
	r->type = READER_TYPE_FILE;
	r->mybyte = 0;
	r->bitpos = 8;
	r->pos = 0;
}

unsigned int reader_readbit(reader_t* r)
{
	if(8 == r->bitpos)
	{
		r->bitpos = 0;
		r->read(r, &r->mybyte, 1);
	}
	return (r->mybyte >> (7 - r->bitpos++)) & 1;
}

unsigned int reader_readbits(reader_t* r, int num)
{
int t;
int val = 0;
	for(t=0; t<num; t++)
	{
		val <<= 1;
		val |= reader_readbit(r);
	}
	return val;
}

U32 reader_GetBits(reader_t* reader, int nbits)
{
	return reader_readbits(reader, nbits);
}

S32 reader_GetSBits(reader_t* reader, int nbits)
{
U32 res;
	res = reader_readbits(reader, nbits);
	if( res & (1<<(nbits-1))) res |= (0xFFFFFFFF << nbits);

	return (S32)res;
}

int reader_GetRect(reader_t* reader, SRECT* r)
{
int nbits;
SRECT dummy;
	
	if(NULL == r) r = &dummy;
	nbits = (int) reader_GetBits(reader, 5);
	r->xmin = reader_GetSBits(reader, nbits);
	r->xmax = reader_GetSBits(reader, nbits);
	r->ymin = reader_GetSBits(reader, nbits);
	r->ymax = reader_GetSBits(reader, nbits);

	return 0;
}

int swf_ReadSWF2(reader_t* reader, SWF* swf)
{
char b[32];
int len;
//reader_t zreader;

	memset(swf, 0x00, sizeof(SWF));

	len = reader->read(reader, b, 8);
	if(len < 8) return -1;
	
	if(b[0] != 'F' && b[0] != 'C') return -1;
	if(b[1] != 'W') return -1;
	if(b[2] != 'S') return -1;
	
	swf->fileVersion = b[3];
	swf->compressed = ('C' == b[0]) ? 1 : 0;
	swf->fileSize = GET32(&b[4]);

	reader_GetRect(reader, &swf->movieSize);
	reader->read(reader, &swf->frameRate, 2);

	swf->frameRate = LE_16_TO_NATIVE(swf->frameRate);
	reader->read(reader, &swf->frameCount, 2);
	swf->frameCount = LE_16_TO_NATIVE(swf->frameCount);

	swf->firstTag = NULL;

	return 0;
}

int swf_ReadSWFInfo(TCHAR* filename, SWF* swf)
{
reader_t reader;
int f, ret;
	
	if(0 != _tsopen_s(&f, filename, _O_RDONLY | _O_BINARY, _SH_DENYWR, 0)) return -1;
		
	reader_init_filereader(&reader, f);

	ret = swf_ReadSWF2(&reader, swf);

	_close(f);

	return ret;
}

