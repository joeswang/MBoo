#pragma once

#define SCREEN_MIN_WIDTH	1024
#define SCREEN_MIN_HEIGHT	690

#define VIDEO_MIN_WIDTH		800
#define VIDEO_MIN_HEIGHT	600

#define VIDEO_MAX_WIDTH		1024
#define VIDEO_MAX_HEIGHT	768

#define DIALOG_PANEL_HEIGHT		 25

#define DIALOG_LIST_MIN_WIDTH	200

#define PARAM_QUERY_URL		0
#define PARAM_VIDEO_DIR		1
#define DEFAULT_QUERY_URL	"http://www.boobooke.com/qv.php"
#define DEFAULT_VIDEO_DIR   "videos"
#define DEFAULT_VIDEO_DB	"video.db"

#define SQL_STMT_MAX_LEN	1024

//////////////////////////////////////////////////
#define VIDEO_FILENAME_MAX_LEN	14	// bbk1234567890
#define VIDEO_TITLE_MAX_LEN		256
#define	TUTOR_NAME_MAX_LEN		128
#define VIDEO_MAX_NUMBERS		1000

typedef struct _VIDEONODE
{
	int		child;		// -1 : NULL
	int		sibling;	// -1 : NULL
	TCHAR	title[VIDEO_TITLE_MAX_LEN];
	UINT	total;
	TCHAR   file[VIDEO_FILENAME_MAX_LEN];
	TCHAR	tutor[TUTOR_NAME_MAX_LEN];
} VIDEONODE;

extern VIDEONODE g_videoTree[VIDEO_MAX_NUMBERS];
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

