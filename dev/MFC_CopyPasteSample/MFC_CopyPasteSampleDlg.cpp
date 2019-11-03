﻿
// MFC_CopyPasteSampleDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "MFC_CopyPasteSample.h"
#include "MFC_CopyPasteSampleDlg.h"
#include "afxdialogex.h"
#include "CPasteCommand.h"
#include "CPasteCommandExtend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCCopyPasteSampleDlg ダイアログ



CMFCCopyPasteSampleDlg::CMFCCopyPasteSampleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_COPYPASTESAMPLE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCCopyPasteSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCCopyPasteSampleDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PASTE_BUTTON, &CMFCCopyPasteSampleDlg::OnBnClickedPasteButton)
	ON_BN_CLICKED(IDC_PASTE_BUTTON2, &CMFCCopyPasteSampleDlg::OnBnClickedPasteButton2)
END_MESSAGE_MAP()


// CMFCCopyPasteSampleDlg メッセージ ハンドラー

BOOL CMFCCopyPasteSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CMFCCopyPasteSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CMFCCopyPasteSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCCopyPasteSampleDlg::OnBnClickedPasteButton()
{
	CPasteCommand Command;
	Command.PrepCommand(0, this->m_hWnd);
	Command.Execute();

}


void CMFCCopyPasteSampleDlg::OnBnClickedPasteButton2()
{
	CPasteCommandExtend Command;
	Command.PrepCommand(0, this->m_hWnd);
	Command.Execute();
}
