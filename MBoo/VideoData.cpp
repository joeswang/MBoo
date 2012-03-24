#include "stdafx.h"

#include "resource.h"
#include "define.h"
#include "swflib/swflib.h"

typedef struct _BBKRESULT
{
	LPTSTR result_buf;	
	UINT len;
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

BOOL is_valid_bbk_video(LPCTSTR dirname, LPCTSTR filepath)
{
int len, i;
int ret;
SWF swf;
WIN32_FIND_DATA findata;
HANDLE hVideoFile;
TCHAR vf[MAX_PATH] = { 0 };

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

	//vf[0] = 0;
	memset(vf, 0, sizeof(vf));
	StringCchCat(vf, MAX_PATH, filepath);
	StringCchCat(vf, MAX_PATH, _T("\\"));
	StringCchCat(vf, MAX_PATH, _T(DEFAULT_VIDEO_FILE));
	hVideoFile = FindFirstFile(vf, &findata);
	if(INVALID_HANDLE_VALUE == hVideoFile)
	{
		memset(vf, 0, MAX_PATH * sizeof(TCHAR));
		StringCchCat(vf, MAX_PATH, filepath);
		StringCchCat(vf, MAX_PATH, _T("\\"));
		StringCchCat(vf, MAX_PATH, dirname);
		StringCchCat(vf, MAX_PATH, _T(".swf"));
		hVideoFile = FindFirstFile(vf, &findata);
		if(INVALID_HANDLE_VALUE == hVideoFile)	return FALSE;
	}

	//if(FILE_ATTRIBUTE_DIRECTORY != videodata.dwFileAttributes) return FALSE;
	memset(&swf, 0, sizeof(SWF));
	ret = swf_ReadSWFInfo(vf, &swf);
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
RECVIDEO *p, *pVNode = NULL;
int count = 0;
UINT idx;
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
		memset(path, 0, sizeof(path));
		StringCchCat(path, MAX_PATH, g_configInfo.videodir);
		StringCchCat(path, MAX_PATH, _T("\\"));
		StringCchCat(path, MAX_PATH, findata.cFileName);
		if(is_valid_bbk_video(findata.cFileName, path))
		{
			idx = get_hash_index(findata.cFileName);
			pVNode = (RECVIDEO*)malloc(sizeof(RECVIDEO));
			if(NULL == pVNode) 
			{
				return 0;
			}
			memset(pVNode, 0, sizeof(RECVIDEO));
			g_ptblV[idx] = pVNode;
			StringCchCat(pVNode->name, VIDEO_FILENAME_MAX_LEN, findata.cFileName);
			count++;
		}
	}

	while(FindNextFile(hFind, &findata))
	{
		if(0 == (FILE_ATTRIBUTE_DIRECTORY & findata.dwFileAttributes)) continue;
		//if(FILE_ATTRIBUTE_DIRECTORY != findata.dwFileAttributes) continue;

		memset(path, 0, sizeof(path));
		StringCchCat(path, MAX_PATH, g_configInfo.videodir);
		StringCchCat(path, MAX_PATH, _T("\\"));
		StringCchCat(path, MAX_PATH, findata.cFileName);
		if(!is_valid_bbk_video(findata.cFileName, path)) continue;

		pVNode = (RECVIDEO*)malloc(sizeof(RECVIDEO));
		memset(pVNode, 0, sizeof(RECVIDEO));
		if(NULL == pVNode) continue;
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
int idx = 0;
RECVIDEO* p = NULL;
sqlite3 *db;
sqlite3_stmt *stmt = NULL;
int rc;
TCHAR sql[SQL_STMT_MAX_LEN] = { 0 };
TCHAR tmpbuf[VIDEO_TITLE_MAX_LEN + 1] = { 0 };

	if(NULL == result) return FALSE;
	
	memset(g_tblS, 0, sizeof(RECSERIES) * SERIES_MAX_NUMBERS);

	i=0; len = result->len;
	while(i < len)
	{
		left = right = 0;
		if(0 == state)
		{
			while(i < len && _T('|') != result->result_buf[i]) { i++; }
			if(i >= len) break; 
			left = i; state = 1;
			i++;
		}
		if(1 == state)
		{
			while(i < len && _T('|') != result->result_buf[i]) { i++; }
			if(i >= len) break; 
			right = i; state = 0;
			//i++;
		}
		if(left >0 && right>0 && (right - left > 3)) // seem to get a good record
		{
			if(_T('S') == result->result_buf[left+1])  // series record
			{
				//process_series_record(result->result_buf, left+1, right-1);
				j = left + 2;
				L = j;
				while(j<right && _T(':') != result->result_buf[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) tmpbuf[k-L] = result->result_buf[k];
				g_tblS[idx].valid = TRUE;
				g_tblS[idx].sid = _tstoi(tmpbuf);
				
				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->result_buf[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) tmpbuf[k-L] = result->result_buf[k];
				g_tblS[idx].total = _tstoi(tmpbuf);

				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->result_buf[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				//memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) g_tblS[idx].title[k-L] = result->result_buf[k];

				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->result_buf[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > TUTOR_NAME_MAX_LEN) continue; // too long
				//memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) g_tblS[idx].tutor[k-L] = result->result_buf[k];

				idx++;
			}
			if( _T('b') == result->result_buf[left+1] &&
				_T('b') == result->result_buf[left+2] &&
				_T('k') == result->result_buf[left+3] ) // video record
			{
				j = left + 1;
				L = j;
				while(j<right && _T(':') != result->result_buf[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_FILENAME_MAX_LEN) continue; // too long
				memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) tmpbuf[k-L] = result->result_buf[k];  // vid
				p = lookup_video(tmpbuf);
				if(NULL == p) continue;  // cannot find the video node in the hash table
				
				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->result_buf[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) tmpbuf[k-L] = result->result_buf[k]; // sid
				p->sid = _tstoi(tmpbuf);

				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->result_buf[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) tmpbuf[k-L] = result->result_buf[k]; // idx
				p->idx = _tstoi(tmpbuf);

				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->result_buf[j]) { j++; }
				if(j==right) continue;
				R = j;
				if(R - L > VIDEO_TITLE_MAX_LEN) continue; // too long
				//memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) p->title[k-L] = result->result_buf[k];

				j = R + 1; 
				L = j;
				while(j<right && _T(':') != result->result_buf[j]) { j++; }
				//if(j==right) continue;
				R = j;
				if(R - L > TUTOR_NAME_MAX_LEN) continue; // too long
				//memset(tmpbuf, 0, VIDEO_TITLE_MAX_LEN + 1);
				for(k=L; k<R; k++) p->tutor[k-L] = result->result_buf[k];
			}
		}
	}

	if(NULL != result->result_buf)
	{
		free(result->result_buf);
		result->result_buf = NULL;
		result->len = 0;
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

	for(i=0; i<idx; i++)
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
	//CA2T szResponse((char*)ptr, CP_UTF8);
	bbk_result.result_buf = (LPTSTR)malloc(sizeof(TCHAR)* size * nmemb);
	bbk_result.len = size * nmemb;
	memset(bbk_result.result_buf, 0, bbk_result.len);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)ptr, size * nmemb, bbk_result.result_buf, bbk_result.len);
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
	// test
	//memset(request_buf, 0, sizeof(request_buf));
	//strcat_s(request_buf, REQUEST_BUFFER_MAX_LEN, "q=BBKBEGIN|bbk1109|bbk1110|BBKEND");
	bbk_result.len = 0;
	bbk_result.result_buf = NULL;

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
UINT idx;
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

	count = scan_video_files();

	if(0 == count) return;  // no video found

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
		g_tblS[i].sid = sqlite3_column_int(stmt0,0); // sid
		g_tblS[i].total = sqlite3_column_int(stmt0, 1); // total
		pcol0 = (LPCTSTR)sqlite3_column_text16(stmt0,2); // tutor
		StringCchCat(g_tblS[i].tutor, TUTOR_NAME_MAX_LEN, pcol0);

		pcol1 = (LPCTSTR)sqlite3_column_text16(stmt0,3); // title
		_stprintf_s(g_tblS[i].title, VIDEO_TITLE_MAX_LEN, _T("[系列%d/共%d集]: %s - %s"), g_tblS[i].sid, g_tblS[i].total, pcol0, pcol1);

		_stprintf_s(sql, SQL_STMT_MAX_LEN, _T("SELECT vid, idx, title FROM video WHERE sid=%d ORDER BY idx"), g_tblS[i].sid);
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
			//CA2T szVID(pcol, CP_ACP);
			// lookup the video hash table to find this video record
			idx = get_hash_index(pcol0);
			p = g_ptblV[idx];
			while(NULL != p)
			{
				if(0 == _tcscmp(p->name, pcol0))
				{
					p->db = TRUE;
					p->idx = sqlite3_column_int(stmt1, 1);  // idx;
					pcol1 = (LPCTSTR)sqlite3_column_text16(stmt1,2);	  // title
					//CA2T szT(pcol, CP_ACP);
					StringCchCat(p->title, VIDEO_TITLE_MAX_LEN, pcol1);
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

