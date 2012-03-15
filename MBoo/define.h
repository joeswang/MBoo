#pragma once

#define SCREEN_MIN_WIDTH	1024
#define SCREEN_MIN_HEIGHT	768

#define MBOO_MIN_WIDTH		1024
#define MBOO_MIN_HEIGHT		690

#define VIDEO_MIN_WIDTH		800
#define VIDEO_MIN_HEIGHT	600

#define VIDEO_MAX_WIDTH		1024
#define VIDEO_MAX_HEIGHT	768

#define DIALOG_PANEL_HEIGHT		 25

#define DIALOG_LIST_MIN_WIDTH	200

#define PARAM_QUERY_URL		0
#define PARAM_VIDEO_DIR		1
#define PARAM_UPDATE_MODE	2

#define REQUEST_BUFFER_MAX_LEN	102400

#define DEFAULT_QUERY_URL	"http://www.boobooke.com/qv.php"
#define DEFAULT_VIDEO_DIR   "videos"
#define DEFAULT_VIDEO_DB	"video.db"
#define DEFAULT_VIDEO_FILE	"video.swf"

#define SQL_STMT_MAX_LEN	1024
#define UI_MESSAGE_MAX_LEN	1024

#define WM_UPDATE_VIDEO_TREE	(WM_USER + 100)
#define WM_PROGRESS_SYNC_SHOW	(WM_USER + 101)
#define WM_PROGRESS_SYNC_UPDATE	(WM_USER + 102)
#define WM_CHANGE_WINDOW_TITLE	(WM_USER + 103)

#define	MBOO_OK				0
#define MBOO_SCREEN_ERR		1
#define MBOO_FLASH_ERR		2
#define MBOO_CURL_ERR		3

//////////////////////////////////////////////////
#define VIDEO_FILENAME_MAX_LEN	13	// bbk1234567890
#define VIDEO_TITLE_MAX_LEN		256
#define	TUTOR_NAME_MAX_LEN		32
#define VIDEO_HASHTBL_SIZE		1009
#define SERIES_MAX_NUMBERS		1000

typedef struct _RECVIDEO RECVIDEO;
struct _RECVIDEO
{
	RECVIDEO* next;		// hash next
	RECVIDEO* nextVideo;
	BYTE	db;			// if this video is in the database ,then db=1;
	//BYTE	size;		// 800 X 600,  or 1024 X 768?
	UINT	sid;
	UINT	idx;
	TCHAR	name[VIDEO_FILENAME_MAX_LEN + 1];
	TCHAR	title[VIDEO_TITLE_MAX_LEN + 1];
	TCHAR	tutor[TUTOR_NAME_MAX_LEN + 1];
};

typedef struct _RECSERIES RECSERIES;
struct _RECSERIES
{
	RECVIDEO* firstVideo;
	int		valid;
	int		sid;
	int		total;
	TCHAR	tutor[TUTOR_NAME_MAX_LEN + 1];
	TCHAR	title[VIDEO_TITLE_MAX_LEN + 1];
};

extern RECVIDEO* g_ptblV[VIDEO_HASHTBL_SIZE];
extern RECSERIES g_tblS[SERIES_MAX_NUMBERS];

#define URL_MAX_LEN			256

typedef struct _CONFIGINFO
{
	TCHAR	basedir[MAX_PATH];
	TCHAR	videodir[MAX_PATH];
	TCHAR	dbfile[MAX_PATH];
	TCHAR	logfile[MAX_PATH];
	TCHAR	url[URL_MAX_LEN];
	int		update_mode;
	int		volume;		// 0 ~ 100
} CONFIGINFO;

extern CONFIGINFO g_configInfo;

extern HFPC g_hFPC;

#define TITLE_MAX_LEN			256

typedef struct _VIDEOINFO
{
	long	totalframes;
	int		volume;
	TCHAR	title[TITLE_MAX_LEN];
} VIDEOINFO;

extern VIDEOINFO	g_videoInfo;

