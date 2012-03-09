#pragma once

#define SCREEN_MIN_WIDTH	1024
#define SCREEN_MIN_HEIGHT	700

#define VIDEO_MIN_WIDTH		800
#define VIDEO_MIN_HEIGHT	600

#define VIDEO_MAX_WIDTH		1024
#define VIDEO_MAX_HEIGHT	768

#define DIALOG_PANEL_MIN_HEIGHT	 36
#define DIALOG_PANEL_MAX_HEIGHT	 36

#define DIALOG_LIST_MIN_WIDTH	100

#define DEFAULT_QUERY_URL	_T("http://www.boobooke.com/qv.php")
#define DEFAULT_VIDEO_DIR   _T("videos")
#define DEFAULT_VIDEO_DB	_T("video.db")

extern HFPC g_hFPC;
extern TCHAR g_MainDirectory[MAX_PATH];

#define TITLE_MAX_LEN			256

typedef struct _VIDEOINFO
{
	long	totalframes;
	int		volume;
	TCHAR	title[TITLE_MAX_LEN];
} VIDEOINFO;

extern VIDEOINFO	g_videoInfo;

