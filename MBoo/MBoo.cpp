// MBoo.cpp : main source file for MBoo.exe
//

#include "stdafx.h"

#include "resource.h"
#include "define.h"

#include "MBooView.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "VideoData.h"

// the global variables
HFPC g_hFPC = NULL;
VIDEOINFO	g_videoInfo;
CONFIGINFO	g_configInfo;
RECVIDEO* g_ptblV[VIDEO_HASHTBL_SIZE] = { 0 };
RECSERIES g_tblS[SERIES_MAX_NUMBERS] = { 0 };
CMainFrame* g_pMainWnd = NULL;

CAppModule _Module;

BOOL GetConfigInfo(CONFIGINFO*);


int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	RECT rc = {0, 0, MBOO_MIN_WIDTH,MBOO_MIN_HEIGHT};
	CMainFrame wndMain;
	
	if(wndMain.CreateEx(NULL, rc) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}
	g_pMainWnd = &wndMain;
	wndMain.CenterWindow();
	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
CURLcode res;
int sw, sh;
	
	sw = GetSystemMetrics(SM_CXSCREEN);
	sh = GetSystemMetrics(SM_CYSCREEN);
	if(sw < SCREEN_MIN_WIDTH || sh < SCREEN_MIN_HEIGHT)
	{
		::MessageBox(NULL, _T("你的电脑屏幕分辨率必须至少为1024X768或者以上，才能够运行本软件！"), _T("系统分辨率太低"),MB_OK);
		return 0;
	}
	g_hFPC = FPC_LoadRegisteredOCX();
	if (NULL == g_hFPC)
	{
		::MessageBox(NULL, _T("你的系统尚未安装Flash播放器插件！"), _T("未检测到Flash插件"),MB_OK);
		return 0;
	}

	res = curl_global_init(CURL_GLOBAL_ALL);
	if(CURLE_OK != res)
	{
		::MessageBox(NULL, _T("加载Libcurl库出错！"), _T("系统错误"),MB_OK);
		return 0;
	}
	
	memset(g_ptblV, 0, sizeof(g_ptblV));
	memset(g_tblS, 0, sizeof(g_tblS));

	if(FALSE == GetConfigInfo(&g_configInfo))
	{
		::MessageBox(NULL, _T("读取系统配置文件出错！"), _T("系统错误"),MB_OK);
		return 0;
	}

	::SkinSE_LoadSkinResourceFromFolder(_T("skin"));
	//----------------- Our Initialized Job Done!------------------

	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	//----------------- Our Cleanup Job---------------------
	release_tblV(g_ptblV);

	curl_global_cleanup();

	if (NULL != g_hFPC)
	{
		FPC_UnloadCode(g_hFPC);
		g_hFPC = NULL;
	}

	return nRet;
}



BOOL GetConfigInfo(CONFIGINFO* pcfgInfo)
{
WIN32_FIND_DATA findata;
HANDLE hFile;
sqlite3 *db;
sqlite3_stmt *stmt = NULL;
char *pcol = 0;
int rc;
char sql[SQL_STMT_MAX_LEN] = {0};
TCHAR path[MAX_PATH] = {0};

	if(NULL == pcfgInfo) return FALSE;

	memset(pcfgInfo, 0, sizeof(CONFIGINFO));

	if(0 == ::GetCurrentDirectory(MAX_PATH, pcfgInfo->maindir))
	{
		//::MessageBox(NULL, _T("无法获得可执行文件的目录！"), _T("系统错误"),MB_OK);
		return FALSE;
	}
	//if(_tcslen(pcfgInfo->maindir) == 0) return FALSE;
	/*
	StringCchCat(pcfgInfo->url, URL_MAX_LEN, _T(DEFAULT_QUERY_URL));
	StringCchCat(pcfgInfo->videodir, MAX_PATH, pcfgInfo->maindir);
	StringCchCat(pcfgInfo->videodir, MAX_PATH, _T("\\"));
	StringCchCat(pcfgInfo->videodir, MAX_PATH, _T(DEFAULT_VIDEO_DIR));
	pcfgInfo->update_mode = 0;
	*/
	StringCchCat(path, MAX_PATH, pcfgInfo->maindir);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, _T(DEFAULT_VIDEO_DB));
	StringCchCat(pcfgInfo->dbfile, MAX_PATH, path);

	hFile = FindFirstFile(path, &findata);
	if(INVALID_HANDLE_VALUE == hFile) // if we cannot find the video.db database, then we will create a fresh one.
	{
		initilize_video_database(path);
		// return TRUE;
	}

	rc = sqlite3_open(CT2A(path), &db);
	if( rc )
	{
		sqlite3_close(db);
		StringCchCat(pcfgInfo->url, URL_MAX_LEN, _T(DEFAULT_QUERY_URL));
		StringCchCat(pcfgInfo->videodir, MAX_PATH, pcfgInfo->maindir);
		StringCchCat(pcfgInfo->videodir, MAX_PATH, _T("\\"));
		StringCchCat(pcfgInfo->videodir, MAX_PATH, _T(DEFAULT_VIDEO_DIR));
		pcfgInfo->update_mode = 0;
		return TRUE;
	}

	sprintf_s(sql, SQL_STMT_MAX_LEN, "SELECT charval FROM config WHERE param = %d", PARAM_QUERY_URL);
	if(SQLITE_OK == sqlite3_prepare_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW == rc)
		{
			pcol = (char*)sqlite3_column_text(stmt,0);
			CA2T szURL(pcol, CP_ACP);
			pcfgInfo->url[0] = 0x00;
			StringCchCat(pcfgInfo->url, URL_MAX_LEN, szURL);
		}
	}
	sqlite3_finalize(stmt);

	sprintf_s(sql, SQL_STMT_MAX_LEN, "SELECT charval FROM config WHERE param = %d", PARAM_VIDEO_DIR);
	if(SQLITE_OK == sqlite3_prepare_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW == rc)
		{
			pcol = (char*)sqlite3_column_text(stmt,0);
			CA2T szVDIR(pcol, CP_ACP);
			pcfgInfo->videodir[0] = 0x00;
			StringCchCat(pcfgInfo->videodir, MAX_PATH, szVDIR);
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	if(0 == _tcslen(pcfgInfo->url))
	{
		StringCchCat(pcfgInfo->url, URL_MAX_LEN, _T(DEFAULT_QUERY_URL));
	}

	if(0 == _tcslen(pcfgInfo->videodir))
	{
		StringCchCat(pcfgInfo->videodir, MAX_PATH, pcfgInfo->maindir);
		StringCchCat(pcfgInfo->videodir, MAX_PATH, _T("\\"));
		StringCchCat(pcfgInfo->videodir, MAX_PATH, _T(DEFAULT_VIDEO_DIR));
	}

	pcfgInfo->update_mode = 1;

	return TRUE;
}
