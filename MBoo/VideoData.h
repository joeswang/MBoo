#ifndef _MB_VIDEODATA_H_
#define _MB_VIDEODATA_H_

BOOL is_valid_bbk_video(LPCTSTR dirname, LPCTSTR filepath);
void thread_update_videoinfo(void* data);
void thread_query_videoinfo(void* data);
void release_tblV(RECVIDEO** ptblV);
void initilize_video_database(LPCTSTR);
#endif //_MB_VIDEODATA_H_