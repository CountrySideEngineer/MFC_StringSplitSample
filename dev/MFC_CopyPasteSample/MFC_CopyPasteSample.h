
// MFC_CopyPasteSample.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CMFCCopyPasteSampleApp:
// このクラスの実装については、MFC_CopyPasteSample.cpp を参照してください
//

class CMFCCopyPasteSampleApp : public CWinApp
{
public:
	CMFCCopyPasteSampleApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMFCCopyPasteSampleApp theApp;
