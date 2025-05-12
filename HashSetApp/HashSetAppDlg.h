#pragma once
#include "HashSet.h"
#include <afxwin.h>
#include <string>
#include <atomic>

class CHashSetAppDlg : public CDialogEx {
public:
    CHashSetAppDlg(CWnd* pParent = nullptr);

    enum { IDD = IDD_HASHSETAPP_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    HICON m_hIcon;
    CProgressCtrl m_progress;
    CButton m_btnLoad;
    CButton m_btnProcess;
    CListBox m_listAllWords;
    CListBox m_listShortWords;
    CListBox m_listPalindromes;

    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedButtonLoad();
    afx_msg void OnBnClickedButtonProcess();

    DECLARE_MESSAGE_MAP()

private:
    HashSet m_wordSet;
    std::atomic<bool> m_isLoading;
    CString m_filePathToLoad;

    void EnableControls(bool enable);
    static UINT LoadFileThread(LPVOID pParam);
    afx_msg LRESULT OnLoadProgress(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLoadComplete(WPARAM wParam, LPARAM lParam);
};