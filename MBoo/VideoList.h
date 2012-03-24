// VideoList.h : interface of the CVideoList class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define IDC_LIST_CMD_OPENFOLDER	0x12
#define IDC_LIST_CMD_DETAILINFO	0x13

class CVideoWnd;

class CVideoList : public CDialogImpl<CVideoList>
{
public:
    enum { IDD = IDD_DLG_LIST };

	CVideoList() { m_pFlashObject = NULL; m_btreeLocked = TRUE;}
	~CVideoList() {}

private:
	CVideoWnd*  m_pFlashObject;

public: 
	void SetFlashObject(CVideoWnd* pflashObj) { m_pFlashObject = pflashObj; }
	//void UpdateUI();
	BOOL WindowMove(UINT, UINT, UINT, UINT);
	
    BEGIN_MSG_MAP(CVideoList)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLBtnDown)
		NOTIFY_ID_HANDLER(IDC_LIST_TREE_VIDEO, OnVideoSelected)
		MESSAGE_HANDLER(WM_UPDATE_VIDEO_TREE, OnUpdateVideoTree)
		MESSAGE_HANDLER(WM_PROGRESS_SYNC_SHOW, OnProgressShow)
		MESSAGE_HANDLER(WM_PROGRESS_SYNC_UPDATE, OnProgressUpdate)
		COMMAND_ID_HANDLER(IDC_LIST_BTN_SYNC, OnBtnSyncVideo)
		COMMAND_ID_HANDLER(IDC_LIST_CMD_OPENFOLDER, OnOpenVideoFolder)
		COMMAND_ID_HANDLER(IDC_LIST_CMD_DETAILINFO, OnDetailedInfo)
    END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLBtnDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUpdateVideoTree(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnProgressShow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnProgressUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBtnSyncVideo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOpenVideoFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDetailedInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnVideoSelected(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

private:
	//void PopulateVideoInfo();
	BOOL	m_btreeLocked;
	LRESULT OnDBLClick();
	LRESULT OnRClick();
	LRESULT OnShowTips(LPNMTVGETINFOTIP);
};
