// VideoWnd.cpp : implementation of the CVideoWnd class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "VideoWnd.h"
#include "VideoPanel.h"

HWND CVideoWnd::Create(HWND hwndParent)
{
	RECT rc = {0,0,10,10};

	if(NULL == hwndParent) return NULL;
	if(NULL != m_hWnd) return m_hWnd;

	m_hWnd = FPC_CreateWindow(g_hFPC,
						 0,
						 NULL, 
						 WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 
						 rc.left, 
						 rc.top, 
						 rc.right - rc.left, 
						 rc.bottom - rc.top, 
						 hwndParent, 
						 NULL, 
						 NULL, 
						 NULL);
	return m_hWnd;
}

BOOL CVideoWnd::ShowWindow(int nCmdShow)
{
	if(NULL == m_hWnd) return FALSE;
	return (::ShowWindow(m_hWnd, nCmdShow));
}

BOOL CVideoWnd::WindowMove(UINT x, UINT y, UINT cx, UINT cy)
{
	if(m_hWnd != NULL)
	{
		::SetWindowPos(m_hWnd, NULL, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	return TRUE;
}

BOOL CVideoWnd::PlayFlashVideo(LPCTSTR lpszURL)
{

	SFPCPutMovie FPCPutMovie;
	SFPCPutStandardMenu FPCPutStandardMenu;

	if(NULL == m_hWnd) return FALSE;
	if(NULL == lpszURL) return FALSE;
	if(NULL == m_pdlgPanel) return FALSE;

	::ShowWindow(m_hWnd, SW_SHOW);

	FPCPutMovie.lpszBuffer = lpszURL;

	::SendMessage(m_hWnd, FPCM_PUT_MOVIE, 0, (LPARAM)&FPCPutMovie);
	
	FPC_SetSoundVolume(g_hFPC, g_configInfo.volume * DEF_MAX_FLASH_AUDIO_VOLUME / 100);
	
	//::Sleep(500);

	FPC_Play(m_hWnd);

	FPCPutStandardMenu.StandardMenu = TRUE;
	::SendMessage(m_hWnd, FPCM_PUT_STANDARD_MENU, 0, (LPARAM)&FPCPutStandardMenu);
	
	m_havingMoive = TRUE;
	
	g_videoInfo.totalframes = GetTotalFrames();
	m_pdlgPanel->UpdateVideoUI(&g_videoInfo);
	
	return TRUE;
}

UINT CVideoWnd::IsPlaying()
{
	SFPCIsPlaying info;

	if(NULL == m_hWnd) return 2;

	::SendMessage(m_hWnd, FPCM_ISPLAYING, 0, (LPARAM)&info);

	if(FAILED(info.hr))
	{
		// error
		//MessageBox(_T("IsPlaying Operation Failed!"));
		return 2;
	}
	return info.Result? 1 : 0;
}

BOOL CVideoWnd::Pause()
{
	SFPCStop info;

	if(NULL == m_hWnd) return FALSE;

	::SendMessage(m_hWnd, FPCM_STOP, 0, (LPARAM)&info);

	if(FAILED(info.hr))
	{
		// error
		//MessageBox(_T("Stop Operation Failed!"));
		return FALSE;
	}

	return TRUE;
}

BOOL CVideoWnd::Resume()
{
	SFPCPlay info;

	if(NULL == m_hWnd) return FALSE;
	::SendMessage(m_hWnd, FPCM_PLAY, 0, (LPARAM)&info);

	if(FAILED(info.hr))
	{
		// error
		//MessageBox(_T("Play Operation Failed!"));
		return FALSE;
	}

	return TRUE;

}

BOOL CVideoWnd::Stop()
{
	SFPCRewind info;

	if(NULL == m_hWnd) return FALSE;
	
	::SendMessage(m_hWnd, FPCM_REWIND, 0, (LPARAM)&info);

	if(FAILED(info.hr))
	{
		// error
		//MessageBox(_T("Stop Operation Failed!"));
		return FALSE;
	}
	return TRUE;
}

long CVideoWnd::GetTotalFrames()
{
	SFPCGetTotalFrames info;

	if(NULL == m_hWnd) return FALSE;
	
	::SendMessage(m_hWnd, FPCM_GET_TOTALFRAMES, 0, (LPARAM)&info);

	if(FAILED(info.hr))
	{
		// error
		//MessageBox(_T("Stop Operation Failed!"));
		return -1;
	}
	return info.TotalFrames;
}

long CVideoWnd::GetCurrentFrame()
{
	SFPCCurrentFrame info;

	if(NULL == m_hWnd) return FALSE;
	
	::SendMessage(m_hWnd, FPCM_CURRENTFRAME, 0, (LPARAM)&info);

	if(FAILED(info.hr))
	{
		// error
		//MessageBox(_T("Stop Operation Failed!"));
		return -1;
	}
	return info.Result;
}

BOOL CVideoWnd::EnableSound(BOOL enabled)
{
	FPC_EnableSound(g_hFPC, enabled);
	return TRUE;
}

HRESULT CVideoWnd::SetSoundVolume(int min, int max, int pos)
{
	return (FPC_SetSoundVolume(g_hFPC, pos * DEF_MAX_FLASH_AUDIO_VOLUME / max));
}

HRESULT CVideoWnd::SetProgress(int min, int max, int pos)
{
BOOL bIsPlaying = FALSE;
	SFPCGotoFrame info;

	if(NULL == m_hWnd) return FALSE;
	
	if(1 == IsPlaying()) bIsPlaying = TRUE;

	info.FrameNum = pos;
	::SendMessage(m_hWnd, FPCM_GOTOFRAME, 0, (LPARAM)&info);

	if(FAILED(info.hr))
	{
		// error
		//MessageBox(_T("Stop Operation Failed!"));
		return -1;
	}

	if(bIsPlaying)
	{
		FPC_Play(m_hWnd);
	}

	return 0;
}
