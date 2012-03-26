// VideoPanel.h : interface of the CVideoPanel class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CVideoWnd;

class CVideoPanel : public CDialogImpl<CVideoPanel>
{
public:
    enum { IDD = IDD_DLG_PANEL };

	CVideoPanel() { m_pFlashObject = NULL; }
	~CVideoPanel() {}

	void SetFlashObject(CVideoWnd* pflashObj) { m_pFlashObject = pflashObj; }
	void UpdateVideoUI(VIDEOINFO*);
	BOOL WindowMove(UINT, UINT, UINT, UINT);

    BEGIN_MSG_MAP(CVideoPanel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLBtnDown)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(IDC_PANEL_CHK_AUDIO, OnCheckAudio)
		COMMAND_ID_HANDLER(IDC_PANEL_BTN_STARTPAUSE, OnStartPause)
		COMMAND_ID_HANDLER(IDC_PANEL_BTN_STOP, OnStop)
		COMMAND_ID_HANDLER(IDC_PANEL_STC_TIMEFRAME, OnTimeFrameMode)
    END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLBtnDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHotKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCheckAudio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStartPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTimeFrameMode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	CVideoWnd*  m_pFlashObject;

};
