// MBoo.cpp : main source file for MBoo.exe
//

#include "stdafx.h"

#include "resource.h"
#include "define.h"

#include "MBooView.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "VideoData.h"
#include "DlgWizard.h"

// the global variables
HFPC g_hFPC = NULL;
VIDEOINFO	g_videoInfo;
CONFIGINFO	g_configInfo;
RECVIDEO* g_ptblV[VIDEO_HASHTBL_SIZE] = { 0 };
RECSERIES g_tblS[SERIES_MAX_NUMBERS] = { 0 };

CAppModule _Module;

BOOL GetConfigInfo();

BOOL InitInstance()
{
CURLcode res;
int sw, sh;
TCHAR msg[UI_MESSAGE_MAX_LEN] = {0};

	sw = GetSystemMetrics(SM_CXSCREEN);
	sh = GetSystemMetrics(SM_CYSCREEN);
	if(sw < SCREEN_MIN_WIDTH || sh < SCREEN_MIN_HEIGHT)
	{
		_stprintf_s(msg, UI_MESSAGE_MAX_LEN, _T("你的电脑屏幕分辨率必须至少为 %d X %d 或者以上，才能够运行本软件！"), SCREEN_MIN_WIDTH, SCREEN_MIN_HEIGHT);
		::MessageBox(NULL, msg, _T("系统分辨率太低"),MB_OK);
		return FALSE;
	}
	g_hFPC = FPC_LoadRegisteredOCX();
	if (NULL == g_hFPC)
	{
		::MessageBox(NULL, _T("你的系统尚未安装Flash播放器插件！"), _T("未检测到Flash插件"),MB_OK);
		return FALSE;
	}

	res = curl_global_init(CURL_GLOBAL_ALL);
	if(CURLE_OK != res)
	{
		::MessageBox(NULL, _T("加载Libcurl库出错！"), _T("系统错误"),MB_OK);
		return FALSE;
	}
	
	if(FALSE == GetConfigInfo())
	{
		//::MessageBox(NULL, _T("读取系统配置文件出错！"), _T("系统错误"),MB_OK);
		return FALSE;
	}
	::SkinSE_LoadSkinResourceFromFolder(_T("skin"));

	//memset(msg, 0, UI_MESSAGE_MAX_LEN);
	//StringCchCat(msg, UI_MESSAGE_MAX_LEN, g_configInfo.basedir);
	//StringCchCat(msg, UI_MESSAGE_MAX_LEN, _T("\\skin.zip"));

	//::SkinSE_LoadSkinResourceFromZIP(msg, NULL);

	memset(g_ptblV, 0, sizeof(g_ptblV));
	memset(g_tblS, 0, sizeof(g_tblS));

	return TRUE;
}

int ExitInstance()
{
	//----------------- Our Cleanup Job---------------------
	FreeVideoHashTbl(g_ptblV);

	curl_global_cleanup();

	if (NULL != g_hFPC)
	{
		FPC_UnloadCode(g_hFPC);
		g_hFPC = NULL;
	}

	return 0;
}

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

	wndMain.CenterWindow();
	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	if(FALSE == InitInstance()) return 0;
	
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

	ExitInstance();

	return nRet;
}

BOOL GetConfigInfo()
{
BOOL ret;
WIN32_FIND_DATA findata;
HANDLE hFile;
sqlite3 *db;
sqlite3_stmt *stmt = NULL;
LPCTSTR pcol0 = NULL;
int rc;
TCHAR sql[SQL_STMT_MAX_LEN] = {0};
TCHAR msg[UI_MESSAGE_MAX_LEN] = {0};

	memset(&g_configInfo, 0, sizeof(CONFIGINFO));
	g_configInfo.update_mode = 0;
	g_configInfo.volume = 100;

	if(0 == ::GetCurrentDirectory(MAX_PATH, g_configInfo.basedir))
	{
		return FALSE;
	}

	StringCchCat(g_configInfo.dbfile, MAX_PATH, g_configInfo.basedir);
	StringCchCat(g_configInfo.dbfile, MAX_PATH, _T("\\"));
	StringCchCat(g_configInfo.dbfile, MAX_PATH, _T(DEFAULT_VIDEO_DB));

	StringCchCat(g_configInfo.url, URL_MAX_LEN, _T(DEFAULT_QUERY_URL));

	StringCchCat(g_configInfo.videodir, MAX_PATH, g_configInfo.basedir);
	StringCchCat(g_configInfo.videodir, MAX_PATH, _T("\\"));
	StringCchCat(g_configInfo.videodir, MAX_PATH, _T(DEFAULT_VIDEO_DIR));

	hFile = FindFirstFile(g_configInfo.dbfile, &findata);
	if(INVALID_HANDLE_VALUE == hFile) // if we cannot find the video.db database, then we will create a fresh one.
	{
		CDlgWizard dlgWZD;
		if(IDOK != dlgWZD.DoModal()) return FALSE;  // will change the g_configInfo.videodir here
		ret = initilize_video_database(g_configInfo.dbfile, g_configInfo.videodir);
		hFile = FindFirstFile(g_configInfo.dbfile, &findata); // find the datbase file again!
		if(!ret || INVALID_HANDLE_VALUE == hFile) // still cannot find this database file
		{
			_stprintf_s(msg, UI_MESSAGE_MAX_LEN, _T("无法创建数据库文件: %s"), g_configInfo.dbfile);
			::MessageBox(NULL, msg, _T("系统错误"),MB_OK);
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	rc = sqlite3_open16(g_configInfo.dbfile, &db);
	if( rc )
	{
		sqlite3_close(db);
		return TRUE;
	}

	memset(sql, 0, SQL_STMT_MAX_LEN);
	_stprintf_s(sql, SQL_STMT_MAX_LEN, _T("SELECT charval FROM config WHERE param = %d"), PARAM_QUERY_URL);
	if(SQLITE_OK == sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW == rc)
		{
			pcol0 = (LPCTSTR)sqlite3_column_text16(stmt,0);
			memset(g_configInfo.url, 0, URL_MAX_LEN);
			StringCchCat(g_configInfo.url, URL_MAX_LEN, pcol0);
		}
	}
	sqlite3_finalize(stmt);

	memset(sql, 0, SQL_STMT_MAX_LEN);
	_stprintf_s(sql, SQL_STMT_MAX_LEN, _T("SELECT charval FROM config WHERE param = %d"), PARAM_VIDEO_DIR);
	if(SQLITE_OK == sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW == rc)
		{
			pcol0 = (LPCTSTR)sqlite3_column_text16(stmt,0);
			memset(g_configInfo.videodir, 0, MAX_PATH);
			StringCchCat(g_configInfo.videodir, MAX_PATH, pcol0);
		}
	}
	sqlite3_finalize(stmt);

	memset(sql, 0, SQL_STMT_MAX_LEN);
	_stprintf_s(sql, SQL_STMT_MAX_LEN, _T("SELECT charval FROM config WHERE param = %d"), PARAM_UPDATE_MODE);
	if(SQLITE_OK == sqlite3_prepare16_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW == rc)
		{
			g_configInfo.update_mode = sqlite3_column_int(stmt,0);
		}
	}
	sqlite3_finalize(stmt);

	sqlite3_close(db);

	return TRUE;
}
