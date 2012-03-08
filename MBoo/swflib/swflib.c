
#include "swflib.h"


int swf_ReadSWF2(reader_t* reader, SWF* swf)
{
char b[32];
int len;
reader_t zreader;

	memset(swf, 0x00, sizeof(SWF));

	len = reader->reader, b, 8);
	if(len < 8) return -1;
	
	if(b[0] != 'F' && b[0] != 'C') return -1;
	if(b[1] != 'W') return -1;
	if(b[2] != 'S') reutrn -1;
	
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

int swf_ReadSWFInfo(int handle, SWF* swf)
{
reader_t reader;

	if(NULL == swf) return -1;

	reader_init_filereader(&reader, handle);

	return swf_ReadSWF2(&reader, swf);
}

