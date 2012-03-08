// VideoList.cpp : implementation of the CVideoList class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "VideoList.h"
#include "VideoWnd.h"

#define VIDEO_BUFFER_MAX_LEN	102400
static TCHAR request_buf[VIDEO_BUFFER_MAX_LEN];
static TCHAR response_buf[VIDEO_BUFFER_MAX_LEN];

void get_videoinfo_thread(void* data)
{
HWND hWndUI;
int i;
	hWndUI = (HWND)data;

	if(NULL == hWndUI) return;

	for(i=1; i<=10; i++)
	{
		PostMessage(hWndUI, WM_PROGRESS_SYNC_SHOW, 100, i*10);
		Sleep(1000);
	}
	PostMessage(hWndUI, WM_PROGRESS_SYNC_HIDE, 0, 0);
}

LRESULT CVideoList::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CProgressBarCtrl pgs = GetDlgItem(IDC_LIST_PGS_SYNC);
	pgs.ShowWindow(SW_HIDE);	
	pgs.SetRange(0, 100);

	::SkinSE_SubclassWindow(m_hWnd, _T("dlg.videolist"));
	::SkinSE_SetLayoutMainFrame(m_hWnd);

	::SkinSE_SubclassWindow(GetDlgItem(IDC_LIST_TREE_VIDEO), _T("list.tree.video"));
	::SkinSE_SubclassWindow(GetDlgItem(IDC_LIST_BTN_SYNC), _T("list.btn.sync"));
	::SkinSE_SubclassWindow(GetDlgItem(IDC_LIST_PGS_SYNC).m_hWnd, _T("list.pgs.sync"));

	CTreeViewCtrlEx treeVideo = GetDlgItem(IDC_LIST_TREE_VIDEO);
	CTreeItem ti1, ti2;
	ti1 = treeVideo.InsertItem(_T("Oracle培训视频"), TVI_ROOT, TVI_LAST);
	ti2 = ti1.AddTail(_T("[小布老师]Oracle 12C Fundamental培训视频"), 0);
	ti2.AddTail(_T("[bbk1109]第一集 : Oracle 12C在Linux下的安装"), 0);
	ti2.AddTail(_T("[bbk1312]第二集 : Oracle体系结构概述"), 0);
	ti2.AddTail(_T("[bbk1322]第三集 : 使用DBCA和手工建库"), 0);
	ti2.AddTail(_T("[bbk2341]第四集 : 表空间和数据文件"), 0);
	ti2.AddTail(_T("[bbk3018]第五集 : 联机Redo文件和归档文件"), 0);
	ti2 = ti1.AddTail(_T("[海明老师]菜鸟RMAN备份不犯愁培训视频"), 0);
	ti2.AddTail(_T("[bbk1359]第一集 : Oracle备份概述"), 0);
	ti2.AddTail(_T("[bbk1360]第二集 : RMAN的基本知识"), 0);
	ti2.AddTail(_T("[bbk1361]第三集 : 如何做Oracle数据库的全备"), 0);
	ti2.AddTail(_T("[bbk1362]第四集 : 如何做Oracle增量备份"), 0);
	ti2.AddTail(_T("[bbk1363]第五集 : 菜鸟高枕无忧之使用脚本自动化备份"), 0);
	ti1 = treeVideo.InsertItem(_T("Linux培训视频"), TVI_ROOT, TVI_LAST);
	ti2 = ti1.AddTail(_T("[海明老师]Ubuntu Linux菜鸟不犯愁培训视频"), 0);
	ti2.AddTail(_T("[bbk1239]第一集 : Linux概述和发展历史"), 0);
	ti2.AddTail(_T("[bbk1240]第二集 : Ubuntu Linux的基本知识"), 0);
	ti2.AddTail(_T("[bbk5401]第三集 : Ubuntu Linux的安装"), 0);
	ti2.AddTail(_T("[bbk5402]第四集 : Linux基本命令入门"), 0);
	ti2.AddTail(_T("[bbk5403]第五集 : 保卫你的桌面，保卫你的胃"), 0);
	ti1 = treeVideo.InsertItem(_T("Java培训视频"), TVI_ROOT, TVI_LAST);
	ti2 = ti1.AddTail(_T("[黑弓老师]Java从入门到精通培训视频"), 0);
	ti2.AddTail(_T("[bbk5239]第一集 : Java概述和发展历史"), 0);
	ti2.AddTail(_T("[bbk5240]第二集 : 搭建你的开发环境"), 0);
	ti2.AddTail(_T("[bbk5501]第三集 : 一切皆是对象，OOP入门"), 0);
	ti2.AddTail(_T("[bbk5502]第四集 : 工欲善其事必先利其器，Eclips宝典"), 0);
	ti2.AddTail(_T("[bbk5503]第五集 : 保卫你的工作，保卫你的胃，使用Git保护你的代码"), 0);

	return TRUE;
}

LRESULT CVideoList::OnProgressShow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CProgressBarCtrl pgs = GetDlgItem(IDC_LIST_PGS_SYNC);
	pgs.SetPos(lParam);
	return 0;
}

LRESULT CVideoList::OnProgressHide(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CProgressBarCtrl pgs = GetDlgItem(IDC_LIST_PGS_SYNC);
	pgs.ShowWindow(SW_HIDE);
	return 0;
}

BOOL CVideoList::WindowMove(UINT x, UINT y, UINT cx, UINT cy)
{
	if(NULL == m_hWnd) return FALSE;

	return (::SetWindowPos(m_hWnd, NULL, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE));
}

LRESULT CVideoList::OnBtnSyncVideo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
/*
	MessageBox(_T("从播布客网站数据库查询视频目录信息，更新本地视频目录！"));
	return 0;
	sqlite3 *db;
    char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open("video.db", &db);
	if( rc )
	{
		MessageBox(_T("Cannot Open DB"));
		sqlite3_close(db);
		return 0;
	}

	rc = sqlite3_exec(db, "insert into t values(8)", callback, 0, &zErrMsg);
	if(SQLITE_OK != rc)
	{
		MessageBox(_T("INSERT err!"));
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
	MessageBox(_T("successfully!"));
*/
	CProgressBarCtrl pgs = GetDlgItem(IDC_LIST_PGS_SYNC);
	pgs.ShowWindow(SW_SHOW);	
	pgs.SetRange(0, 100);

	_beginthread(get_videoinfo_thread, 0, m_hWnd);
	return 0;
}

