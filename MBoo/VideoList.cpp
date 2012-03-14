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
	::SkinSE_SubclassWindow(GetDlgItem(IDC_LIST_PGS_SYNC).m_hWnd, _T("list.pgs.sync"));

	CTreeViewCtrlEx treeVideo = GetDlgItem(IDC_LIST_TREE_VIDEO);
	::SetWindowLong(treeVideo, GWL_STYLE, 
		treeVideo.GetStyle() | TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_LINESATROOT | TVS_DISABLEDRAGDROP);
/*
	CBitmap bmpLock;
	CImageList imgList;
	bmpLock.LoadBitmap(IDR_BITMAP_LOCK);
	imgList.Create(9, 12, ILC_COLOR24|ILC_MASK, 0, 1);
	imgList.Add(bmpLock,RGB(255,0,255));
	//bmpLock.DeleteObject();
	treeVideo.SetImageList(imgList);
*/
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

LRESULT CVideoList::OnVideoSelected(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
TCHAR path[MAX_PATH] = { 0 };
WIN32_FIND_DATA findata;
HANDLE hVideoFile;
TCHAR msg[MAX_PATH] = {0};

	if(NM_DBLCLK != pnmh->code) return FALSE;
	//MessageBox(_T("选中一个视频！"));
	CTreeViewCtrlEx treeVideo = GetDlgItem(IDC_LIST_TREE_VIDEO);
	CTreeItem ti =	treeVideo.GetSelectedItem();
	if(ti.HasChildren()) return TRUE;

	if(NULL == m_pFlashObject ) return FALSE;

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
		//if(NULL != g_pMainWnd) g_pMainWnd->SetWindowText(path);
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
		//if(NULL != g_pMainWnd) g_pMainWnd->SetWindowText(path);
		m_pFlashObject->PlayFlashVideo(path);
		return TRUE;
	}
	StringCchCat(msg, MAX_PATH, _T("没有找到视频文件 : "));
	//StringCchCat(msg, MAX_PATH*2, path);
	MessageBox(msg);
	return FALSE;
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

	_beginthread(thread_update_videoinfo, 0, m_hWnd);
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
			_stprintf_s(msg, VIDEO_TITLE_MAX_LEN*2, _T("[第%d集] %s"), p->idx, p->title);
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

	return 0;

}
