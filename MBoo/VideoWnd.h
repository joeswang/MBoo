// VideoWnd.h : interface of the CVideoWnd class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CVideoPanel;

class CVideoWnd
{
public:

	CVideoWnd() 
	{ 
		m_hWnd = NULL; 
		m_pdlgPanel = NULL;
		m_videoWidth = VIDEO_MIN_WIDTH; 
		m_videoHeight = VIDEO_MIN_HEIGHT; 
		m_havingMoive = FALSE;
	};
	~CVideoWnd() {};

	UINT GetVideoWidth() { return m_videoWidth; }
	UINT GetVideoHeight() { return m_videoHeight; }

	HWND Create(HWND hwndParent);
	BOOL WindowMove(UINT, UINT, UINT, UINT);
	BOOL ShowWindow(int);
	BOOL PlayFlashVideo(LPCTSTR lpszURL);
	BOOL HavingMovie() { return m_havingMoive; }
	UINT IsPlaying();  // 0 - Not Playing, 1 - Playing, 2 - Error
	BOOL Pause();
	BOOL Resume();
	BOOL Stop();
	BOOL EnableSound(BOOL);
	HRESULT SetSoundVolume(int, int, int);
	HRESULT SetProgress(int, int, int);
	long GetTotalFrames();
	long GetCurrentFrame();
	void SetPanelHandle(CVideoPanel* pdlgPanel) { m_pdlgPanel = pdlgPanel; }

private:
	HWND m_hWnd;
	UINT m_videoWidth;
	UINT m_videoHeight;
	BOOL m_havingMoive;
	CVideoPanel*  m_pdlgPanel;

};