
// MFC_CopyPasteSampleDlg.h : ヘッダー ファイル
//

#pragma once


// CMFCCopyPasteSampleDlg ダイアログ
class CMFCCopyPasteSampleDlg : public CDialogEx
{
// コンストラクション
public:
	CMFCCopyPasteSampleDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_COPYPASTESAMPLE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPasteButton();
	afx_msg void OnBnClickedPasteButton2();
};
