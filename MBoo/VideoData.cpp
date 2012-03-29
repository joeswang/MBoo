#include "stdafx.h"

#include "resource.h"
#include "define.h"
#include "swflib/swflib.h"

typedef struct _BBKRESULT
{
	TCHAR buffer[RESPONESE_BUFFER_MAX_LEN];	
	int index;
} BBKRESULT;

static BBKRESULT bbk_result;
static char request_buf[REQUEST_BUFFER_MAX_LEN];


UINT get_hash_index(LPCTSTR vid)
{
UINT len, sum = 0;
	
	len = _tcslen(vid);
	for(UINT i=0; i<len; i++) sum += vid[i];

	return ((sum << 2) % VIDEO_HASHTBL_SIZE);
}

void FreeVideoHashTbl(RECVIDEO** ptblV)
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

RECVIDEO* lookup_video(LPCTSTR vid)
{
RECVIDEO* p = NULL;
UINT idx = 0;

	idx = get_hash_index(vid);
	p = g_ptblV[idx];
	while(NULL != p)
	{
		if(0 == _tcscmp(p->name, vid)) return p;
		p = p->next;
	}
	return NULL;
}

BOOL initilize_video_database(LPCTSTR dbfilename, LPCTSTR videodir)
{
sqlite3 *db;
sqlite3_stmt *stmt = NULL;
int rc;
TCHAR sql[SQL_STMT_MAX_LEN] = { 0 };

	rc = sqlite3_open16(dbfilename, &db);
	_stprintf_s(sql, SQL_STMT_MAX_LEN, 
		_T("CREATE TABLE config(param INTEGER, idx INTEGER, intval INTEGER, charval CHAR(256), desc CHAR(256))"));
	sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	_stprintf_s(sql, SQL_STMT_MAX_LEN, 
		_T("INSERT INTO config VALUES(0,1,0,'http://www.boobooke.com/qv.php', 'video query url')"));
	sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	_stprintf_s(sql, SQL_STMT_MAX_LEN, 
		_T("INSERT INTO config VALUES(1,1,0,'%s', 'local video directory')"), videodir);
	sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	_stprintf_s(sql, SQL_STMT_MAX_LEN, 
		_T("INSERT INTO config VALUES(2,0,0,'', 'update database mode 0 - increamental, 1 - all')"));
	sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	_stprintf_s(sql, SQL_STMT_MAX_LEN, 
		_T("CREATE TABLE series(sid INTEGER PRIMARY KEY, tutor CHAR(32), total INTEGER, title CHAR(256), summary CHAR(2048))"));
	sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	_stprintf_s(sql, SQL_STMT_MAX_LEN, 
		_T("INSERT INTO series VALUES(0, '各种老师都有', 0, '不成系列的视频', '没有系列的视频')"));
	sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	_stprintf_s(sql, SQL_STMT_MAX_LEN, 
		_T("CREATE TABLE video(vid CHAR(13) PRIMARY KEY, tutor CHAR(32), sid INTEGER, idx INTEGER, title CHAR(256), FOREIGN KEY(sid) REFERENCES series(sid))"));
	sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	sqlite3_close(db);
	
	return TRUE;
}

BOOL is_valid_bbk_video(LPCTSTR videoname, WORD* pwFrameRate)
{
int len, i;
int ret;
SWF swf;
WIN32_FIND_DATA findata;
HANDLE hVideoFile;
TCHAR vf[MAX_PATH] = { 0 };
TCHAR path[MAX_PATH] = { 0 };

	if(NULL == videoname) return FALSE;

	// check the video file name first :　bbk1234567890
	len = _tcslen(videoname);
	if(len < 3 || len > VIDEO_FILENAME_MAX_LEN) return FALSE;
	if(_T('b') != videoname[0]) return FALSE;
	if(_T('b') != videoname[1]) return FALSE;
	if(_T('k') != videoname[2]) return FALSE;
	for(i=3; i<len; i++)
	{
		if(_T('0') > videoname[i] || _T('9') < videoname[i]) return FALSE;
	}

	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.videodir);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, videoname);
	StringCchCat(path, MAX_PATH, _T("\\"));

	memset(vf, 0, sizeof(vf));
	StringCchCat(vf, MAX_PATH, path);
	StringCchCat(vf, MAX_PATH, _T(DEFAULT_VIDEO_FILE));
	hVideoFile = FindFirstFile(vf, &findata);
	if(INVALID_HANDLE_VALUE == hVideoFile)
	{
		memset(vf, 0, MAX_PATH * sizeof(TCHAR));
		StringCchCat(vf, MAX_PATH, path);
		StringCchCat(vf, MAX_PATH, videoname);
		StringCchCat(vf, MAX_PATH, _T(".swf"));
		hVideoFile = FindFirstFile(vf, &findata);
		if(INVALID_HANDLE_VALUE == hVideoFile)	return FALSE;
	}

	if(0 != (FILE_ATTRIBUTE_DIRECTORY & findata.dwFileAttributes)) return FALSE;

	memset(&swf, 0, sizeof(SWF));
	ret = swf_ReadSWFInfo(vf, &swf);
	if(ret < 0) return FALSE;
	if(swf.movieSize.xmin != 0 || swf.movieSize.ymin != 0 
			|| swf.movieSize.xmax != 20 * VIDEO_MIN_WIDTH || swf.movieSize.ymax != 20 * VIDEO_MIN_HEIGHT)
		return FALSE;

	if(NULL != pwFrameRate) *pwFrameRate = swf.frameRate;  // record the frameRate
	return TRUE;
}

/*----------------------------------------------------------------------------------------------------*
 *  Scan the video local directory specified by g_configInfo.videodir to find the valid boobooke videos
 *----------------------------------------------------------------------------------------------------*/
int scan_video_files()
{
WIN32_FIND_DATA findata;
HANDLE hFind;
RECVIDEO *p, *pVNode = NULL;
int count = 0;
UINT idx;
WORD wFrameRate;
TCHAR path[MAX_PATH] = {0};

	// Scan the local video directory to update the video infomation
	FreeVideoHashTbl(g_ptblV);

	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.videodir);
	StringCchCat(path, MAX_PATH, _T("\\bbk*"));

	hFind = FindFirstFile(path, &findata);
	if(INVALID_HANDLE_VALUE == hFind) 
	{
		return 0;
	}

	if(0 != (FILE_ATTRIBUTE_DIRECTORY & findata.dwFileAttributes))
	{
		wFrameRate = 0;
		if(is_valid_bbk_video(findata.cFileName, &wFrameRate))
		{
			idx = get_hash_index(findata.cFileName);
			pVNode = (RECVIDEO*)malloc(sizeof(RECVIDEO));
			if(NULL == pVNode) 
			{
				return 0;
			}
			memset(pVNode, 0, sizeof(RECVIDEO));
			g_ptblV[idx] = pVNode;
			pVNode->frameRate = wFrameRate;
			StringCchCat(pVNode->name, VIDEO_FILENAME_MAX_LEN, findata.cFileName);
			count++;
		}
	}

	while(FindNextFile(hFind, &findata))
	{
		if(0 == (FILE_ATTRIBUTE_DIRECTORY & findata.dwFileAttributes)) continue;

		wFrameRate = 0;
		if(!is_valid_bbk_video(findata.cFileName, &wFrameRate)) continue;

		pVNode = (RECVIDEO*)malloc(sizeof(RECVIDEO));
		memset(pVNode, 0, sizeof(RECVIDEO));
		if(NULL == pVNode) continue;
		pVNode->frameRate = wFrameRate;
		StringCchCat(pVNode->name, VIDEO_FILENAME_MAX_LEN, findata.cFileName);
		idx = get_hash_index(findata.cFileName);
		if(NULL == g_ptblV[idx]) g_ptblV[idx] = pVNode;
		else
		{
			p = g_ptblV[idx];
			while(NULL != p->next) p = p->next;
			p->next = pVNode;
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
LPCTSTR pvid = NULL;
int rc;
UINT idx;
TCHAR path[MAX_PATH] = {0};
TCHAR sql[SQL_STMT_MAX_LEN] = {0};

	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.basedir);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, _T(DEFAULT_VIDEO_DB));
	rc = sqlite3_open16(path, &db);
	if( rc )
	{
		sqlite3_close(db);
		return;
	}
	_stprintf_s(sql, SQL_STMT_MAX_LEN, _T("SELECT vid FROM video ORDER BY 1"));
	if(SQLITE_OK != sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL))
	{
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while(TRUE)
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW != rc) break;

		pvid = (LPCTSTR)sqlite3_column_text16(stmt,0); // vid
		idx = get_hash_index(pvid);
		p = g_ptblV[idx];
		while(NULL != p)
		{
			if(0 == _tcscmp(p->name, pvid))
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

BOOL update_video_database(BBKRESULT* result)
{
int i, left, right, len;
int j, k, L, R;
int state = 0;   // 0 - not in a record, 1 - in a record
int idx;
RECVIDEO* p = NULL;
sqlite3 *db;
sqlite3_stmt *stmt = NULL;
int rc;
TCHAR sql[SQL_STMT_MAX_LEN] = { 0 };
TCHAR tmpbuf[VIDEO_TITLE_MAX_LEN + 1] = { 0 };

	if(NULL == result) return FALSE;
	
	memset(g_tblS, 0, sizeof(g_tblS));

	idx = -1;
	i=0; len = result->index;
	while(i < len)
	{
		left = right = 0;
		if(0 == state)
		{
			while(i < len && _T('|') != result->buffer[i]) { i++; }
			if(i >= len) break; 
			left = i; state = 1;
			i++;
		}
		if(1 == state)
		{
			while(i < len && _T('|') != result->buffer[i]) { i++; }
			if(i >= len) break; 
			right = i; state = 0;
		}

		if(left >0 && right>0 && (right - left > 3)) // seem to get a good record
		{
			if(_T('S') == result->buffer[left+1])  // series record
			{
				idx++;
				if(idx >= SERIES_MAX_NUMBERS) continue;
				j = left + 2;
				L = j;
				while(j<right && _T(':') != result->buffer[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) tmpbuf[k-L] = result->buffer[k];
				g_tblS[idx].valid = TRUE;
				g_tblS[idx].sid = _tstoi(tmpbuf);
				
				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->buffer[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) tmpbuf[k-L] = result->buffer[k];
				g_tblS[idx].total = _tstoi(tmpbuf);

				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->buffer[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				//memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) g_tblS[idx].title[k-L] = result->buffer[k];

				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->buffer[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > TUTOR_NAME_MAX_LEN) continue; // too long
				//memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) g_tblS[idx].tutor[k-L] = result->buffer[k];
			}

			if( _T('b') == result->buffer[left+1] &&
				_T('b') == result->buffer[left+2] &&
				_T('k') == result->buffer[left+3] ) // video record
			{
				j = left + 1;
				L = j;
				while(j<right && _T(':') != result->buffer[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_FILENAME_MAX_LEN) continue; // too long
				memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) tmpbuf[k-L] = result->buffer[k];  // vid
				p = lookup_video(tmpbuf);
				if(NULL == p) continue;  // cannot find the video node in the hash table
				
				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->buffer[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) tmpbuf[k-L] = result->buffer[k]; // sid
				p->sid = _tstoi(tmpbuf);

				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->buffer[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) tmpbuf[k-L] = result->buffer[k]; // idx
				p->idx = _tstoi(tmpbuf);

				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->buffer[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				//memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) p->title[k-L] = result->buffer[k];

				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->buffer[j]) { j++; }
				//if(j==right) continue;
				R = j;
				if(R - L > TUTOR_NAME_MAX_LEN) continue; // too long
				//memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) p->tutor[k-L] = result->buffer[k];
			}
		}
	}

	rc = sqlite3_open16(g_configInfo.dbfile, &db);

	if(1 == g_configInfo.update_mode) // clean the series and video table in the database
	{
		_stprintf_s(sql, SQL_STMT_MAX_LEN,  _T("DELETE FROM video"));
		sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
		rc = sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		_stprintf_s(sql, SQL_STMT_MAX_LEN,  _T("DELETE FROM series WHERE sid <> 0"));
		sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
		rc = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}

	for(i=0; i<=idx; i++)
	{
		int count;
		_stprintf_s(sql, SQL_STMT_MAX_LEN,  _T("SELECT count(1) FROM series WHERE sid=%d"), g_tblS[i].sid); 
		sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
		rc = sqlite3_step(stmt);
		count = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);

		if(count == 0)
		{
			_stprintf_s(sql, SQL_STMT_MAX_LEN,  _T("INSERT INTO series VALUES(%d, '%s', %d, '%s', NULL)"), 
				g_tblS[i].sid, g_tblS[i].tutor, g_tblS[i].total, g_tblS[i].title);
			sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}
	}

	for(i=0; i<VIDEO_HASHTBL_SIZE; i++)
	{
		if(NULL == g_ptblV[i]) continue;
		p = g_ptblV[i];
		while(NULL != p)
		{
			if(FALSE == p->db && 0 < _tcslen(p->title))
			{
				_stprintf_s(sql, SQL_STMT_MAX_LEN,  _T("INSERT INTO video VALUES('%s', '%s', %d, %d, '%s')"), 
					p->name, p->tutor, p->sid, p->idx, p->title);
				sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL);
				rc = sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
			p = p->next;
		}
	}

	sqlite3_close(db);

	return TRUE;
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int len;
	
	len = size * nmemb;
	if(bbk_result.index + len < RESPONESE_BUFFER_MAX_LEN)
	{
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)ptr, len, bbk_result.buffer + bbk_result.index, len);
		bbk_result.index += len;
		return len;
	}
	else
	{
		len = RESPONESE_BUFFER_MAX_LEN - bbk_result.index;
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)ptr, len, bbk_result.buffer + bbk_result.index, len);
		bbk_result.index += len;
		return 0;
	}
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
	// test
	//memset(request_buf, 0, sizeof(request_buf));
	//strcat_s(request_buf, REQUEST_BUFFER_MAX_LEN, "q=BBKBEGIN|bbk1109|bbk1110|BBKEND");
	memset(&bbk_result, 0, sizeof(BBKRESULT));

	send_request_to_bbk(request_buf);

	::PostMessage(hWndUI, WM_PROGRESS_SYNC_UPDATE, 100, 90);  // show 20% in the UI
	Sleep(1000);
	//::MessageBox(hWndUI, response_buf, _T(""), MB_OK); 

	update_video_database(&bbk_result);

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
//UINT idx;
RECVIDEO *p, *current, *pVNode = NULL;
sqlite3 *db;
sqlite3_stmt *stmt0 = NULL;
sqlite3_stmt *stmt1 = NULL;
//LPCTSTR pcol = NULL;
LPCTSTR pcol0, pcol1;
int rc;
TCHAR path[MAX_PATH] = {0};
TCHAR sql[SQL_STMT_MAX_LEN] = {0};

	hWndUI = (HWND)data;
	if(NULL == hWndUI) return;

	memset(g_tblS, 0, sizeof(RECSERIES) * SERIES_MAX_NUMBERS);

	count = scan_video_files();

	if(0 == count)
	{
		PostMessage(hWndUI, WM_UPDATE_VIDEO_TREE, 0, 0);
		return;  // no video found
	}

	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.basedir);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, _T(DEFAULT_VIDEO_DB));
	
	rc = sqlite3_open16(path, &db);
	if( rc )
	{
		sqlite3_close(db);
		::PostMessage(hWndUI, WM_UPDATE_VIDEO_TREE, 0, 0);
		return;
	}
	_stprintf_s(sql, SQL_STMT_MAX_LEN, _T("SELECT sid, total, tutor, title, summary FROM series ORDER BY 1"));
	if(SQLITE_OK != sqlite3_prepare16_v2(db, sql, -1, &stmt0,NULL))
	{
		sqlite3_finalize(stmt0);
		sqlite3_close(db);
		::PostMessage(hWndUI, WM_UPDATE_VIDEO_TREE, 0, 0);
		return;
	}
	
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
		g_tblS[i].sid = sqlite3_column_int(stmt0,0); // sid
		g_tblS[i].total = sqlite3_column_int(stmt0, 1); // total
		pcol0 = (LPCTSTR)sqlite3_column_text16(stmt0,2); // tutor
		StringCchCat(g_tblS[i].tutor, TUTOR_NAME_MAX_LEN, pcol0);

		pcol1 = (LPCTSTR)sqlite3_column_text16(stmt0,3); // title
		if(0 != g_tblS[i].sid)
		{
			_stprintf_s(g_tblS[i].title, VIDEO_TITLE_MAX_LEN, _T("[系列%d/共%d集]: %s - %s"), g_tblS[i].sid, g_tblS[i].total, pcol0, pcol1);
			_stprintf_s(sql, SQL_STMT_MAX_LEN, _T("SELECT vid,tutor,idx,title FROM video WHERE sid=%d ORDER BY idx"), g_tblS[i].sid);
		}
		else
		{
			_stprintf_s(g_tblS[i].title, VIDEO_TITLE_MAX_LEN, _T("[系列%d]: %s - %s"), g_tblS[i].sid, pcol0, pcol1);
			_stprintf_s(sql, SQL_STMT_MAX_LEN, _T("SELECT vid,tutor,idx,title FROM video WHERE sid=%d ORDER BY vid"), g_tblS[i].sid);
		}

		if(SQLITE_OK != sqlite3_prepare16_v2(db, sql, -1, &stmt1,NULL))
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
			pcol0 = (LPCTSTR)sqlite3_column_text16(stmt1,0);	  // vid

			p = lookup_video(pcol0);  // lookup the video hash table to find this video record
			if(NULL == p) continue;  // not found, go to the next record
			p->db = TRUE;
			pcol1 = (LPCTSTR)sqlite3_column_text16(stmt1,1);	  // tutor
			StringCchCat(p->tutor, TUTOR_NAME_MAX_LEN, pcol1);
			p->idx = sqlite3_column_int(stmt1, 2);  // idx;
			p->sid = g_tblS[i].sid;
			pcol1 = (LPCTSTR)sqlite3_column_text16(stmt1,3);	  // title
			StringCchCat(p->title, VIDEO_TITLE_MAX_LEN, pcol1);
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


#ifdef UNICODE
#define fill_win32_filefunc64U  fill_win32_filefunc64W
#else
#define fill_win32_filefunc64U  fill_win32_filefunc64A
#endif

#define WRITEBUFFERSIZE	8192

static BOOL unzip_video_file(HWND hWndUI, int current, int total, LPCTSTR zipfile)
{
WORD total_percent, current_percent;
zlib_filefunc64_def ffunc;
unzFile uf=NULL;
unz_global_info64 gi;
unz_file_info64 file_info;
uLong i;
int ret;
FILE *fout = NULL;
TCHAR unicode_path[MAX_PATH] = {0};
char filename_inzip[MAX_PATH];
char *p, *filename_withoutpath;
char buffer[WRITEBUFFERSIZE] = {0};

	total_percent = (WORD)((current * 100)/total);

	fill_win32_filefunc64U(&ffunc);
	uf = unzOpen2_64(zipfile, &ffunc);  // this function seems ok in UNICODE mode
	if(NULL == uf) 
	{
		return FALSE;
	}
	ret = unzGetGlobalInfo64(uf, &gi);
	if(UNZ_OK != ret)
	{
		unzClose(uf);
		return FALSE;
	}

	for(i=0; i<gi.number_entry; i++)
	{
		current_percent = (WORD)((i * 100) / gi.number_entry);
		PostMessage(hWndUI, WM_PROGRESS_UNZIP_SHOW, NULL, MAKELPARAM(current_percent, total_percent));
		ret = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
		if(UNZ_OK != ret)
		{
			unzClose(uf);
			return FALSE;
		}
		p = filename_withoutpath = filename_inzip;
		while ((*p) != '\0')
		{
			if (((*p)=='/') || ((*p)=='\\')) filename_withoutpath = p+1;
			p++;
		}
		memset(unicode_path, 0, sizeof(unicode_path));
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)filename_inzip, MAX_PATH, unicode_path, MAX_PATH);
		if ((*filename_withoutpath)=='\0')
		{
			_tmkdir(unicode_path);
		}
		else
		{
			ret = unzOpenCurrentFile(uf);
			if(UNZ_OK == ret)
			{
				_tfopen_s(&fout, unicode_path, _T("wb"));
				if(NULL != fout)
				{
					do
					{
						ret = unzReadCurrentFile(uf, buffer, WRITEBUFFERSIZE);
						if(ret < 0) break;
						if(fwrite(buffer, ret, 1, fout) != 1)
						{
							ret = UNZ_ERRNO;
							break;
						}
					}while (ret>0);

					fclose(fout);
					unzCloseCurrentFile(uf);
				}
			}
		}
		if(i+1 < gi.number_entry)
		{
			ret = unzGoToNextFile(uf);
			if(UNZ_OK != ret)
			{
				unzClose(uf);
				return FALSE;
			}
		}
	}
	unzClose(uf);

	return TRUE;
}

void thread_unzip_videofile(void* data)
{
HWND hWndUI;
int current, total;
WORD total_percent;

	hWndUI = (HWND)data;
	if(NULL == hWndUI) return;

	total = 0;
	for(int i=0; i<VIDEOZIP_MAX_NUMBERS; i++)
	{
		if(FALSE == g_tblVZIP[i].valid) break;
		if(TRUE == g_tblVZIP[i].unzip) continue;
		total++;
	}
	if(0 == total) return;

	SetCurrentDirectory(g_configInfo.videodir);
	PostMessage(hWndUI, WM_PROGRESS_UNZIP_SHOW, NULL, MAKELPARAM(0,0)); // show the progress bars
	current = 0;
	for(int i=0; i<VIDEOZIP_MAX_NUMBERS; i++)
	{
		if(FALSE == g_tblVZIP[i].valid) break;
		if(TRUE == g_tblVZIP[i].unzip) continue;
		
		total_percent = (WORD)(current*100)/total;
		PostMessage(hWndUI, WM_PROGRESS_UNZIP_SHOW, (WPARAM)(g_tblVZIP[i].name), MAKELPARAM(0, total_percent));
		if(unzip_video_file(hWndUI, current, total,g_tblVZIP[i].name))
		{
			current++;
			g_tblVZIP[i].unzip = TRUE;
		}
	}

	PostMessage(hWndUI, WM_PROGRESS_UNZIP_SHOW, NULL, MAKELPARAM(100,100));
}
