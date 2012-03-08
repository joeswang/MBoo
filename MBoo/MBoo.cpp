// MBoo.cpp : main source file for MBoo.exe
//

#include "stdafx.h"

#include "resource.h"
#include "define.h"

#include "MBooView.h"
#include "aboutdlg.h"
#include "MainFrm.h"

// the global variables
HFPC g_hFPC = NULL;
VIDEOINFO	g_videoInfo;

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	RECT rc = {0, 0, SCREEN_MIN_WIDTH,SCREEN_MIN_HEIGHT};
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
	g_hFPC = FPC_LoadRegisteredOCX();
	if (NULL == g_hFPC)
	{
		::MessageBox(NULL, _T("你的系统尚未安装Flash播放器插件！"), _T("未检测到Flash插件"),MB_OK);
		return 0;
	}

	::SkinSE_LoadSkinResourceFromFolder(_T("skin"));

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

	if (NULL != g_hFPC)
	{
		FPC_UnloadCode(g_hFPC);
		g_hFPC = NULL;
	}

	return nRet;
}
