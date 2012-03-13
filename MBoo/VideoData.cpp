#include "stdafx.h"

#include "resource.h"
#include "define.h"
#include "swflib/swflib.h"

#define REQUEST_BUFFER_MAX_LEN	102400
#define RESPONSE_BUFFER_MAX_LEN	102400
static char request_buf[REQUEST_BUFFER_MAX_LEN];
static TCHAR response_buf[RESPONSE_BUFFER_MAX_LEN];

UINT get_hash_index(LPCTSTR vid)
{
UINT len, sum = 0;
	
	len = _tcslen(vid);
	for(UINT i=0; i<len; i++) sum += vid[i];

	return ((sum * 9973) % VIDEO_HASHTBL_SIZE);
}

void release_tblV(RECVIDEO** ptblV)
{
RECVIDEO *p, *n;

	for(int i=0; i<VIDEO_HASHTBL_SIZE; i++)
	{
		if(NULL == ptblV[i]) continue;
		p = ptblV[i];
		while(NULL != p)
		{
			n = p->next;
			free(p);
			p = n;
		}
	}
	memset(ptblV, 0, sizeof(RECVIDEO*) * VIDEO_HASHTBL_SIZE);
}

void initilize_video_database(LPCTSTR dbfilename)
{
sqlite3 *db;
sqlite3_stmt *stmt = NULL;
int rc;
char sql[SQL_STMT_MAX_LEN] = {0};

	rc = sqlite3_open(CT2A(dbfilename), &db);

	sprintf_s(sql, SQL_STMT_MAX_LEN, 
		"CREATE TABLE config(param INTEGER, idx INTEGER, intval INTEGER, charval CHAR(256), desc CHAR(256))");
	sqlite3_prepare_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	sprintf_s(sql, SQL_STMT_MAX_LEN, 
		"INSERT INTO config VALUES(0,1,0,'http://www.boobooke.com/qv.php', 'video query url')");
	sqlite3_prepare_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	sprintf_s(sql, SQL_STMT_MAX_LEN, 
		"INSERT INTO config VALUES(1,1,0,'videos', 'local video directory')");
	sqlite3_prepare_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	sprintf_s(sql, SQL_STMT_MAX_LEN, 
		"INSERT INTO config VALUES(2,0,0,'', 'update database mode 0 - increamental, 1 - all')");
	sqlite3_prepare_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	sprintf_s(sql, SQL_STMT_MAX_LEN, 
		"CREATE TABLE series(sid INTEGER PRIMARY KEY, tutor CHAR(32), total INTEGER, title CHAR(256), summary CHAR(2048))");
	sqlite3_prepare_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	sprintf_s(sql, SQL_STMT_MAX_LEN, 
		"INSERT INTO series VALUES(0, '各种老师都有', 0, '不成系列的视频', '没有系列的视频')");
	sqlite3_prepare_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	sprintf_s(sql, SQL_STMT_MAX_LEN, 
		"CREATE TABLE video(vid CHAR(13) PRIMARY KEY, tutor CHAR(32), sid INTEGER, idx INTEGER, title CHAR(256), FOREIGN KEY(sid) REFERENCES series(sid))");
	sqlite3_prepare_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	sqlite3_close(db);

}

BOOL is_valid_bbk_video(LPCTSTR dirname, LPCTSTR filepath)
{
int len, i;
int ret, f;
SWF swf;
WIN32_FIND_DATA findata;
HANDLE hVideoFile;

	if(NULL == dirname) return FALSE;
	if(NULL == filepath) return FALSE;
	// check the video file name first :　bbk1234567890
	len = _tcslen(dirname);
	if(len > VIDEO_FILENAME_MAX_LEN) return FALSE;
	if(_T('b') != dirname[0]) return FALSE;
	if(_T('b') != dirname[1]) return FALSE;
	if(_T('k') != dirname[2]) return FALSE;

	for(i=3; i<len; i++)
	{
		if(_T('0') > dirname[i] || _T('9') < dirname[i]) return FALSE;
	}

	hVideoFile = FindFirstFile(filepath, &findata);
	if(INVALID_HANDLE_VALUE == hVideoFile) return FALSE;
	//if(FILE_ATTRIBUTE_DIRECTORY != videodata.dwFileAttributes) return FALSE;
	f = _open(CT2A(filepath), _O_RDONLY | _O_BINARY);
	if(f<0) return FALSE;
	memset(&swf, 0, sizeof(SWF));
	ret = swf_ReadSWFInfo(f, &swf);
	_close(f);
	if(ret < 0) return FALSE;
	if(swf.movieSize.xmin != 0 || swf.movieSize.ymin != 0 
			|| swf.movieSize.xmax != 20 * VIDEO_MIN_WIDTH || swf.movieSize.ymax != 20 * VIDEO_MIN_HEIGHT)
		return FALSE;
	return TRUE;
}

int scan_video_files()
{
WIN32_FIND_DATA findata;
HANDLE hFind;
RECVIDEO *p, *precV = NULL;
int count = 0;
UINT idx;
TCHAR path[MAX_PATH] = {0};

	// Scan the local video directory to update the video infomation
	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.videodir);
	StringCchCat(path, MAX_PATH, _T("\\bbk*"));

	hFind = FindFirstFile(path, &findata);
	if(INVALID_HANDLE_VALUE == hFind) 
	{
		return 0;
	}

	if(FILE_ATTRIBUTE_DIRECTORY != findata.dwFileAttributes)
	{
		return 0;
	}

	release_tblV(g_ptblV);

	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.videodir);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, findata.cFileName);
	StringCchCat(path, MAX_PATH, _T("\\video.swf"));
	if(is_valid_bbk_video(findata.cFileName, path))
	{
		idx = get_hash_index(findata.cFileName);
		precV = (RECVIDEO*)malloc(sizeof(RECVIDEO));
		if(NULL == precV) 
		{
			return 0;
		}
		memset(precV, 0, sizeof(RECVIDEO));
		g_ptblV[idx] = precV;
		StringCchCat(precV->name, VIDEO_FILENAME_MAX_LEN, findata.cFileName);
		count++;
	}

	while(FindNextFile(hFind, &findata))
	{
		if(FILE_ATTRIBUTE_DIRECTORY != findata.dwFileAttributes) continue;

		memset(path, 0, sizeof(path));
		StringCchCat(path, MAX_PATH, g_configInfo.videodir);
		StringCchCat(path, MAX_PATH, _T("\\"));
		StringCchCat(path, MAX_PATH, findata.cFileName);
		StringCchCat(path, MAX_PATH, _T("\\video.swf"));
		if(!is_valid_bbk_video(findata.cFileName, path)) continue;

		precV = (RECVIDEO*)malloc(sizeof(RECVIDEO));
		memset(precV, 0, sizeof(RECVIDEO));
		if(NULL == precV) continue;
		StringCchCat(precV->name, VIDEO_FILENAME_MAX_LEN, findata.cFileName);
		idx = get_hash_index(findata.cFileName);
		if(NULL == g_ptblV[idx]) g_ptblV[idx] = precV;
		else
		{
			p = g_ptblV[idx];
			while(NULL != p->next) p = p->next;
			p->next = precV;
		}
		count++;
	}

	return count;
}

// query the video database to update the g_ptvlV hash table
void check_videodb_info()
{
RECVIDEO *p;
sqlite3 *db;
sqlite3_stmt *stmt = NULL;
char *pcol = NULL;
int rc;
UINT idx;
TCHAR path[MAX_PATH] = {0};
char sql[SQL_STMT_MAX_LEN] = {0};

	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.maindir);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, _T(DEFAULT_VIDEO_DB));
	rc = sqlite3_open(CT2A(path), &db);
	if( rc )
	{
		sqlite3_close(db);
		return;
	}
	sprintf_s(sql, SQL_STMT_MAX_LEN, "SELECT vid FROM video ORDER BY 1");
	if(SQLITE_OK != sqlite3_prepare_v2(db, sql, -1, &stmt,NULL))
	{
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while(TRUE)
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW != rc) break;

		pcol = (char*)sqlite3_column_text(stmt,0); // vid
		CA2T szVID(pcol, CP_ACP);
			// lookup the video hash table to find this video record
		idx = get_hash_index(szVID);
		p = g_ptblV[idx];
		while(NULL != p)
		{
			if(0 == _tcscmp(p->name, szVID))
			{
				p->db = TRUE;
				break;
			}
			p = p->next;
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

BOOL update_video_database(LPCTSTR resultbuf)
{
	if(NULL == resultbuf) return FALSE;
	return TRUE;
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//CA2T szResponse((char*)ptr, CP_UTF8);
	memset(response_buf, 0, sizeof(response_buf));
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)ptr, size * nmemb, response_buf, RESPONSE_BUFFER_MAX_LEN);
	//StringCchCat(response_buf, RESPONSE_BUFFER_MAX_LEN, szResponse);
	return 0;
}

BOOL send_request_to_bbk(LPCSTR rqst_buf)
{
CURL *curl;
CURLcode res;

	if(NULL == rqst_buf) return FALSE;
	curl = curl_easy_init();
	if(curl) 
	{
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Accept-Encoding:utf-8");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "MBoo Player v1.0");
		curl_easy_setopt(curl, CURLOPT_URL, CT2A(g_configInfo.url));
		//curl_easy_setopt(curl, CURLOPT_URL, CT2A(_T("http://192.168.137.188/x.php")));
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, rqst_buf);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return TRUE;
}

void thread_update_videoinfo(void* data)
{
HWND hWndUI;
RECVIDEO *p;
int i, count;

	hWndUI = (HWND)data;
	if(NULL == hWndUI)	return;

	::PostMessage(hWndUI, WM_PROGRESS_SYNC_SHOW, 0, 10);
	Sleep(500);
	count = scan_video_files();
	if(0 == count)
	{
		::PostMessage(hWndUI, WM_PROGRESS_SYNC_SHOW, 0, 0);
		::PostMessage(hWndUI, WM_PROGRESS_SYNC_UPDATE, 100, 100);
		return;  // no video found
	}
	::PostMessage(hWndUI, WM_PROGRESS_SYNC_UPDATE, 100, 40);  // show 40% in the UI
	Sleep(500);
	if( 0 == g_configInfo.update_mode)
	{
		check_videodb_info();
	}
	::PostMessage(hWndUI, WM_PROGRESS_SYNC_UPDATE, 100, 50); 
	Sleep(500);

	memset(request_buf, 0, sizeof(request_buf));
	strcat_s(request_buf, REQUEST_BUFFER_MAX_LEN, "q=BBKBEGIN|");
	for(i=0; i<VIDEO_HASHTBL_SIZE; i++)
	{
		if(NULL == g_ptblV[i]) continue;
		p = g_ptblV[i];
		while(NULL != p)
		{
			if(0 == p->db)
			{
				strcat_s(request_buf, REQUEST_BUFFER_MAX_LEN, CT2A(p->name));
				strcat_s(request_buf, REQUEST_BUFFER_MAX_LEN, "|");
			}
			p = p->next;
		}
	}
	strcat_s(request_buf, REQUEST_BUFFER_MAX_LEN, "BBKEND");
	//::MessageBoxA(hWndUI, request_buf, "", MB_OK); 
	send_request_to_bbk(request_buf);
	Sleep(1000);
	::PostMessage(hWndUI, WM_PROGRESS_SYNC_UPDATE, 100, 90);  // show 20% in the UI
	Sleep(1000);
	//::MessageBox(hWndUI, response_buf, _T(""), MB_OK); 

	update_video_database(response_buf);

	// The task has been finished!
	::PostMessage(hWndUI, WM_PROGRESS_SYNC_UPDATE, 100, 100);
	Sleep(1000);
	::PostMessage(hWndUI, WM_PROGRESS_SYNC_SHOW, 0, 0);

/*
	for(i=8; i<10; i++)
	{
		::PostMessage(hWndUI, WM_PROGRESS_SYNC_UPDATE, 100, i*10); 
		Sleep(500);
	}
*/
}


void thread_query_videoinfo(void* data)
{
HWND hWndUI;
int count;
UINT idx;
RECVIDEO *p, *current, *precV = NULL;
TCHAR path[MAX_PATH] = {0};
sqlite3 *db;
sqlite3_stmt *stmt0 = NULL;
sqlite3_stmt *stmt1 = NULL;
char *pcol = NULL;
int rc;
char sql[SQL_STMT_MAX_LEN] = {0};

	hWndUI = (HWND)data;
	if(NULL == hWndUI) return;

	count = scan_video_files();

	if(0 == count) return;  // no video found

	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.maindir);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, _T(DEFAULT_VIDEO_DB));
	
	rc = sqlite3_open(CT2A(path), &db);
	if( rc )
	{
		sqlite3_close(db);
		::PostMessage(hWndUI, WM_UPDATE_VIDEO_TREE, 0, 0);
		return;
	}
	sprintf_s(sql, SQL_STMT_MAX_LEN, "SELECT sid, tutor, total, title, summary FROM series ORDER BY 1");
	if(SQLITE_OK != sqlite3_prepare_v2(db, sql, -1, &stmt0,NULL))
	{
		sqlite3_finalize(stmt0);
		sqlite3_close(db);
		::PostMessage(hWndUI, WM_UPDATE_VIDEO_TREE, 0, 0);
		return;
	}
	
	memset(g_tblS, 0, sizeof(RECSERIES) * SERIES_MAX_NUMBERS);
	int i = -1;
	while(TRUE)
	{
		i++;
		if(SERIES_MAX_NUMBERS <= i) break;
		rc = sqlite3_step(stmt0);
		if(SQLITE_ROW != rc)
		{
			break;
		}
		g_tblS[i].valid = TRUE;
		int sid = sqlite3_column_int(stmt0,0); // sid
		g_tblS[i].sid = sid;
		pcol = (char*)sqlite3_column_text(stmt0,1); // tutor
		CA2T szTUTOR(pcol, CP_ACP);
		g_tblS[i].total = sqlite3_column_int(stmt0, 2); // total
		pcol = (char*)sqlite3_column_text(stmt0,3); // title
		CA2T szTITLE(pcol, CP_ACP);
		//_stprintf_s(g_tblS[i].title, VIDEO_TITLE_MAX_LEN, _T("%s"), szTITLE);
		//_stprintf_s(g_videoSeries[idx].title, VIDEO_TITLE_MAX_LEN, _T("[系列%s/共%d集]: %s"), szSID, g_videoSeries[idx].total, szTITLE);
		_stprintf_s(g_tblS[i].title, VIDEO_TITLE_MAX_LEN, _T("[系列%d/共%d集]: - %s"), sid, g_tblS[i].total, szTITLE);

		sprintf_s(sql, SQL_STMT_MAX_LEN, "SELECT vid, idx, title FROM video WHERE sid=%d ORDER BY idx", sid);
		if(SQLITE_OK != sqlite3_prepare_v2(db, sql, -1, &stmt1,NULL))
		{
			sqlite3_finalize(stmt1);
			continue;
		}
		current = NULL;
		BOOL bFirst = TRUE;
		while(TRUE)
		{
			rc = sqlite3_step(stmt1);
			if(SQLITE_ROW != rc)
			{
				break;
			}
			pcol = (char*)sqlite3_column_text(stmt1,0);	  // vid
			CA2T szVID(pcol, CP_ACP);
			// lookup the video hash table to find this video record
			idx = get_hash_index(szVID);
			p = g_ptblV[idx];
			while(NULL != p)
			{
				if(0 == _tcscmp(p->name, szVID))
				{
					p->db = TRUE;
					pcol = (char*)sqlite3_column_text(stmt1,2);	  // title
					CA2T szT(pcol, CP_ACP);
					StringCchCat(p->title, VIDEO_TITLE_MAX_LEN, szT);
					break;
				}
				p = p->next;
			}
			if(NULL == p) continue;  // not found, go to the next record
			if(bFirst)
			{
				bFirst = FALSE;
				g_tblS[i].firstVideo = p;
				current = p;
			}
			else
			{
				current->nextVideo = p;
				current = p;
			}
		}
		sqlite3_finalize(stmt1);
	}

	sqlite3_finalize(stmt0);
	sqlite3_close(db);
	::PostMessage(hWndUI, WM_UPDATE_VIDEO_TREE, 0, 0);
}

