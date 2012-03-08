// VideoList.h : interface of the CVideoList class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define WM_PROGRESS_SYNC_SHOW	(WM_USER + 100)
#define WM_PROGRESS_SYNC_HIDE	(WM_USER + 101)

class CVideoWnd;

class CVideoList : public CDialogImpl<CVideoList>
{
public:
    enum { IDD = IDD_DLG_LIST };

	CVideoList() { m_pFlashObject = NULL; }
	~CVideoList() {}

private:
	CVideoWnd*  m_pFlashObject;

public: 
	void SetFlashObject(CVideoWnd* pflashObj) { m_pFlashObject = pflashObj; }
	//void UpdateUI();
	BOOL WindowMove(UINT, UINT, UINT, UINT);

    BEGIN_MSG_MAP(CVideoList)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PROGRESS_SYNC_SHOW, OnProgressShow)
		MESSAGE_HANDLER(WM_PROGRESS_SYNC_HIDE, OnProgressHide)
		COMMAND_ID_HANDLER(IDC_LIST_BTN_SYNC, OnBtnSyncVideo)
    END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnProgressShow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnProgressHide(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBtnSyncVideo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

};
