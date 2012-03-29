#ifndef _MB_VIDEODATA_H_
#define _MB_VIDEODATA_H_

void thread_update_videoinfo(void* data);
void thread_query_videoinfo(void* data);
void thread_unzip_videofile(void* data);

BOOL is_valid_bbk_video(LPCTSTR videoname, WORD* pwFrameRate);
void FreeVideoHashTbl(RECVIDEO** ptblV);
BOOL initilize_video_database(LPCTSTR, LPCTSTR);


#endif //_MB_VIDEODATA_H_