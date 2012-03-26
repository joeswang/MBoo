// VideoList.cpp : implementation of the CVideoList class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "VideoWnd.h"
#include "VideoList.h"
#include "VideoData.h"

LRESULT CVideoList::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CProgressBarCtrl pgs = GetDlgItem(IDC_LIST_PGS_SYNC);
	pgs.ShowWindow(SW_HIDE);	
	pgs.SetRange(0, 100);

	::SkinSE_SubclassWindow(m_hWnd, _T("dlg.videolist"));
	::SkinSE_SetLayoutMainFrame(m_hWnd);

	//::SkinSE_SubclassWindow(GetDlgItem(IDC_LIST_TREE_VIDEO), _T("list.tree.video"));
	::SkinSE_SubclassWindow(GetDlgItem(IDC_LIST_BTN_SYNC), _T("list.btn.sync"));
	::SkinSE_SubclassWindow(GetDlgItem(IDC_LIST_PGS_SYNC), _T("list.pgs.sync"));

	CTreeViewCtrlEx treeVideo = GetDlgItem(IDC_LIST_TREE_VIDEO);
	::SetWindowLong(treeVideo, GWL_STYLE, 
		treeVideo.GetStyle() | TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_LINESATROOT | TVS_DISABLEDRAGDROP | TVS_INFOTIP);
/*
	CBitmap bmpLock;
	CImageList imgList;
	bmpLock.LoadBitmap(IDR_BITMAP_LOCK);
	imgList.Create(9, 12, ILC_COLOR24|ILC_MASK, 0, 1);
	imgList.Add(bmpLock,RGB(255,0,255));
	//bmpLock.DeleteObject();
	treeVideo.SetImageList(imgList);
*/
	m_btreeLocked = TRUE;
	_beginthread(thread_query_videoinfo, 0, m_hWnd);
	//populate_videoinfo_thread(NULL);
	//PopulateVideoInfo();

	return TRUE;
}

LRESULT CVideoList::OnLBtnDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostMessage(GetParent(), uMsg, wParam, lParam);
	bHandled = FALSE;
	return 0;
}

LRESULT CVideoList::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT rc;

	GetClientRect(&rc);
	CTreeViewCtrlEx treeVideo = GetDlgItem(IDC_LIST_TREE_VIDEO);
	treeVideo.MoveWindow(0, 0, rc.right - rc.left - 3, rc.bottom -rc.top - DIALOG_PANEL_HEIGHT); 

	return TRUE;
}

LRESULT CVideoList::OnDBLClick()
{
TCHAR path[MAX_PATH] = { 0 };
WIN32_FIND_DATA findata;
HANDLE hVideoFile;
TCHAR msg[MAX_PATH] = {0};

	if(NULL == m_pFlashObject ) return FALSE;

	CTreeViewCtrlEx treeVideo = GetDlgItem(IDC_LIST_TREE_VIDEO);
	CTreeItem ti =	treeVideo.GetSelectedItem();
	if(ti.HasChildren()) return TRUE;

	RECVIDEO* p  = (RECVIDEO*)ti.GetData();
	if(NULL == p) return FALSE;
	//MessageBox(p->name);
	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.videodir);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, p->name);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, _T(DEFAULT_VIDEO_FILE));

	hVideoFile = FindFirstFile(path, &findata);
	if(INVALID_HANDLE_VALUE != hVideoFile)
	{
		::PostMessage(GetParent(), WM_CHANGE_WINDOW_TITLE, 0, (LPARAM)p);
		m_pFlashObject->PlayFlashVideo(path);
		return TRUE;
	}

	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.videodir);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, p->name);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, p->name);
	StringCchCat(path, MAX_PATH, _T(".swf"));
	hVideoFile = FindFirstFile(path, &findata);
	if(INVALID_HANDLE_VALUE != hVideoFile)
	{
		::PostMessage(GetParent(), WM_CHANGE_WINDOW_TITLE, 0, (LPARAM)p);
		m_pFlashObject->PlayFlashVideo(path);
		return TRUE;
	}
	StringCchCat(msg, MAX_PATH, _T("没有找到视频文件 : "));
	//StringCchCat(msg, MAX_PATH*2, path);
	MessageBox(msg);
	return 0;
}

LRESULT CVideoList::OnRClick()
{
POINT pt;
BOOL  bSeries = FALSE;
TCHAR menuTitle[32+1] = {0};

	GetCursorPos(&pt);
	CTreeViewCtrlEx treeVideo = GetDlgItem(IDC_LIST_TREE_VIDEO);
	treeVideo.ScreenToClient(&pt);
	CTreeItem ti =	treeVideo.HitTest(pt, NULL);
	treeVideo.Select(ti,TVGN_CARET);
	if(ti.HasChildren()) bSeries = TRUE;

	GetCursorPos(&pt);
	CMenu popMenu;
	popMenu.CreatePopupMenu();
	if(!bSeries)
	{
		popMenu.AppendMenu(MF_STRING|MF_ENABLED,IDC_LIST_CMD_OPENFOLDER,_T("打开本视频所在文件夹"));
		StringCchCat(menuTitle, 32, _T("查看本视频的详细信息"));
	}
	else
	{
		StringCchCat(menuTitle, 32, _T("查看本系列的详细信息"));
	}
	popMenu.AppendMenu(MF_STRING|MF_ENABLED,IDC_LIST_CMD_DETAILINFO, menuTitle);

	popMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON, pt.x, pt.y, m_hWnd);

	return 0;
}


LRESULT CVideoList::OnShowTips(LPNMTVGETINFOTIP lpGetInfoTip)
{
	CTreeViewCtrlEx treeVideo = GetDlgItem(IDC_LIST_TREE_VIDEO);
	//treeVideo.Set
	CTreeItem ti(lpGetInfoTip->hItem, &treeVideo);
	if(ti.HasChildren())
	{
		lpGetInfoTip->pszText = _T("这里是系列视频的介绍资料");
	}
	else
	{
		lpGetInfoTip->pszText = _T("这里是单个视频的介绍资料");
	}
	return 0;
}

LRESULT CVideoList::OnVideoSelected(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	if(m_btreeLocked) return FALSE;

	switch(pnmh->code)
	{
	case NM_DBLCLK:
		return OnDBLClick();
	case NM_RCLICK:
		return OnRClick();
	case TVN_GETINFOTIP:
		return OnShowTips((LPNMTVGETINFOTIP)pnmh);
	default:
		return 0;
	}
}


LRESULT CVideoList::OnProgressUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CProgressBarCtrl pgs = GetDlgItem(IDC_LIST_PGS_SYNC);
	pgs.SetPos(lParam);
	if(100 == lParam)
	{
		_beginthread(thread_query_videoinfo, 0, m_hWnd);
	}
	return 0;
}

LRESULT CVideoList::OnProgressShow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	BOOL bShow = (BOOL)lParam;
	CProgressBarCtrl pgs = GetDlgItem(IDC_LIST_PGS_SYNC);
	CButton btnSync = GetDlgItem(IDC_LIST_BTN_SYNC);
	if(bShow)
	{
		pgs.SetRange(0, 100);
		pgs.SetPos(0);
		pgs.ShowWindow(SW_SHOW);	
		btnSync.EnableWindow(FALSE);
	}
	else
	{
		pgs.ShowWindow(SW_HIDE);
		btnSync.EnableWindow(TRUE);
	}

	return 0;
}

BOOL CVideoList::WindowMove(UINT x, UINT y, UINT cx, UINT cy)
{
	if(NULL == m_hWnd) return FALSE;

	return (::SetWindowPos(m_hWnd, NULL, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE));
}

LRESULT CVideoList::OnBtnSyncVideo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	CButton btnSync = GetDlgItem(IDC_LIST_BTN_SYNC);
	btnSync.EnableWindow(FALSE);

	m_btreeLocked = TRUE;
	_beginthread(thread_update_videoinfo, 0, m_hWnd);
	return 0;
}

LRESULT CVideoList::OnDetailedInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MessageBox(_T("这个功能暂没有开发，抱歉 :-)\n会弹出一个对话框，显示本视频/系列的详细信息 :-P"));
	return 0;
}

LRESULT CVideoList::OnOpenVideoFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
TCHAR path[MAX_PATH] = { 0 };

	CTreeViewCtrlEx treeVideo = GetDlgItem(IDC_LIST_TREE_VIDEO);
	CTreeItem ti =	treeVideo.GetSelectedItem();
	if(ti.HasChildren()) return TRUE;

	RECVIDEO* p  = (RECVIDEO*)ti.GetData();
	if(NULL == p) return FALSE;
	//MessageBox(p->name);
	memset(path, 0, sizeof(path));
	StringCchCat(path, MAX_PATH, g_configInfo.videodir);
	StringCchCat(path, MAX_PATH, _T("\\"));
	StringCchCat(path, MAX_PATH, p->name);
	StringCchCat(path, MAX_PATH, _T("\\"));
	//StringCchCat(path, MAX_PATH, _T(DEFAULT_VIDEO_FILE));

	::ShellExecute(NULL, _T("explore"), path, NULL, NULL, SW_SHOWDEFAULT);
	return 0;
}

LRESULT CVideoList::OnUpdateVideoTree(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
int i;
RECVIDEO *p;
BOOL bFirst;
CTreeViewCtrlEx treeVideo = GetDlgItem(IDC_LIST_TREE_VIDEO);
CTreeItem ti1, ti2;
TCHAR msg[VIDEO_TITLE_MAX_LEN * 2];

	treeVideo.DeleteAllItems();

	for(i=0; i<SERIES_MAX_NUMBERS; i++)
	{
		if(FALSE == g_tblS[i].valid) break;
		p = g_tblS[i].firstVideo;
		if(NULL == p) continue;  // there is no video in this series

		ti1 = treeVideo.InsertItem(g_tblS[i].title, TVI_ROOT, TVI_LAST);
		while(NULL != p)
		{
			memset(msg, 0, VIDEO_TITLE_MAX_LEN*2);
			if(0 != p->sid)
				_stprintf_s(msg, VIDEO_TITLE_MAX_LEN*2, _T("[%s]:第%d集 - %s"), p->name, p->idx, p->title);
			else
				_stprintf_s(msg, VIDEO_TITLE_MAX_LEN*2, _T("[%s]: %s"), p->name, p->title);

			ti2 = ti1.AddTail(msg, 0);
			ti2.SetData((DWORD_PTR)p);
			p = p->nextVideo;
		}
	}
	bFirst = TRUE;
	for(i=0; i<VIDEO_HASHTBL_SIZE; i++)
	{
		if(NULL == g_ptblV[i]) continue;
		p = g_ptblV[i];
		while(NULL != p)
		{
			if(FALSE == p->db)
			{
				if(bFirst)
				{
					bFirst = FALSE;
					ti1 = treeVideo.InsertItem(_T("需要同步目录信息的视频"), TVI_ROOT, TVI_LAST);
					ti2 = ti1.AddTail(p->name, 0);
				}
				else
				{
					ti2 = ti1.AddTail(p->name, 0);
				}
				ti2.SetData((DWORD_PTR)p);
			}
			p = p->next;
		}
	}

	m_btreeLocked = FALSE;
	return 0;

}
