#include "pch.h"
#include "CPasteCommand.h"

/**
 *	コマンドの実行準備
 */
VOID CPasteCommand::PrepCommand(INT_PTR MsgId, HWND WndHnd)
{
	this->m_MsgId = MsgId;
	this->m_WndHnd = WndHnd;
}

/**
 *	@brief	貼り付けの実行
 */
BOOL CPasteCommand::Execute()
{
	//クリップボードを開いて、値を取得する
	if (::OpenClipboard(NULL)) {
		HGLOBAL MemHnd = GetClipboardData(CF_UNICODETEXT);
		if (NULL != MemHnd) {
			TCHAR* ClipBoardBuff = (TCHAR*)GlobalLock(MemHnd);
			if (NULL != ClipBoardBuff) {
				this->m_ClipBoardMsg = CString(ClipBoardBuff);
				TRACE(_T("%s\r\n"), ClipBoardBuff);
				GlobalUnlock(MemHnd);
			}
		}
		CloseClipboard();
		this->SplitMsg();

		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 *	@brief	貼り付け文字の分割
 */
VOID CPasteCommand::SplitByToken()
{
	int CurPos = 0;
	CString SplitToken = _T("\t");	//セルの区切り
	CString ResToken = this->m_ClipBoardMsg.Tokenize(SplitToken, CurPos);
	while (ResToken != _T("")) {
		TRACE(_T("%s\r\n"), ResToken);
		ResToken = this->m_ClipBoardMsg.Tokenize(SplitToken, CurPos);
	}
}

/**
 *	文字列を分割する。
 *
 *	@param[in,out]	Src	分割対象の文字列
 *	@param[out]	HasLineChange	分割/抜き出した文字列に
 */
CString CPasteCommand::SplitByToken(CString& Src, BOOL& HasLineChage, INT_PTR& StartIndex)
{
	CString SeparatorCrLf = _T("");
	CString SeparatorTab = _T("");
	CString SeparatorOpt_DQ = _T("");
	TCHAR DQ = _T('\"');
	INT_PTR SeparatorIndexCrLf = 0;
	INT_PTR SeparatorIndexTab = 0;

	//次のセパレータ(改行/タブ)の開始位置を見つける。
	TCHAR* SrcBuffer = Src.GetBuffer();
	if (DQ == SrcBuffer[StartIndex]) {
		SeparatorCrLf = _T("\"\r\n");
		SeparatorTab = _T("\"\t");
		StartIndex++;	//抜き出した文字列が「"」を含まないよう、抜き出し開始位置を1つ進める。
	}
	else {
		SeparatorCrLf = _T("\r\n");
		SeparatorTab = _T("\t");
	}
	SeparatorIndexCrLf = Src.Find(SeparatorCrLf, StartIndex);
	SeparatorIndexTab = Src.Find(SeparatorTab, StartIndex);

	INT_PTR SeparatorLen = 0;
	INT_PTR SeparatorIndex = 0;
	if ((SeparatorIndexCrLf < 0) && (SeparatorIndexTab < 0)) {
		/*
		 *	改行コードもタブも見つからなかった
		 *		→	最後まで抜き出す。
		 */
		SeparatorLen = Src.GetLength() - SeparatorIndex;
		SeparatorIndex = Src.GetLength();
		HasLineChage = FALSE;
	}
	else if ((SeparatorIndexCrLf < 0) && (0 <= SeparatorIndexTab)) {
		//タブだけが見つかった(改行コードなし)
		SeparatorLen = SeparatorTab.GetLength();
		SeparatorIndex = SeparatorIndexTab;
		HasLineChage = FALSE;
	}
	else if ((0 <= SeparatorIndexCrLf) && (SeparatorIndexTab < 0)) {
		//改行コードだけが見つかった(タブなし)
		SeparatorLen = SeparatorCrLf.GetLength();
		SeparatorIndex = SeparatorIndexCrLf;
		HasLineChage = TRUE;
	}
	else {
		/*
		 *	上記以外(改行コードとタブの両方が見つかった
		 *		→	先に見つかったセパレータを採用する
		 */
		if (SeparatorIndexCrLf < SeparatorIndexTab) {
			//改行コードが先に見つかった
			SeparatorLen = SeparatorCrLf.GetLength();
			SeparatorIndex = SeparatorIndexCrLf;
			HasLineChage = TRUE;
		} else if (SeparatorIndexTab < SeparatorIndexCrLf) {
			//タブが先に見つかった
			SeparatorLen = SeparatorTab.GetLength();
			SeparatorIndex = SeparatorIndexTab;
			HasLineChage = FALSE;
		}
		else {
			//改行コード・タブが同じ位置→あり得ない：到達不可能だが、念のため記載。
		}
	}
	INT_PTR Count = SeparatorIndex - StartIndex;
	CString Token = Src.Mid(StartIndex, Count);
	StartIndex = StartIndex + (Count + SeparatorLen);

	return Token;
}


VOID CPasteCommand::SplitMsg()
{
	//文字列の分割の開始
	int StartIndex = 0;
	int EndIndex = this->m_ClipBoardMsg.GetLength();
	CString SplitMsgBuffer[10][10];
	//初期化
	for (INT_PTR BuffRowIndex = 0; BuffRowIndex < 10; BuffRowIndex++) {
		for (INT_PTR BuffColIndex = 0; BuffColIndex < 10; BuffColIndex++) {
			SplitMsgBuffer[BuffRowIndex][BuffColIndex] = _T(" - ");
		}
	}

	INT_PTR RowIndex = 0;
	INT_PTR ColIndex = 0;
	while (StartIndex < EndIndex) {
		BOOL HasLineChange = FALSE;
		CString Token = SplitByToken(this->m_ClipBoardMsg, HasLineChange, StartIndex);
		SplitMsgBuffer[RowIndex][ColIndex] = Token;

		if (HasLineChange) {
			RowIndex++;
			ColIndex = 0;
		}
		else {
			ColIndex++;
		}
	}
}

/**
 *	@brief	「"」で囲まれる文字列を抽出する。
 *
 *	@param[in,out]	RowIndex	行のインデックス
 *	@param[in,out]	ColIndex	列のインデックス
 *	@param[in,out]	StartIndex	走査/抽出の開始位置
 *	@param[out]		ExtractMsg	抽出したメッセージ
 */
VOID CPasteCommand::HandleDQSplitMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg)
{
	int NextDQIndex = this->m_ClipBoardMsg.Find(_T('"'), StartIndex + 1);
	if ((-1) == NextDQIndex) {
		int LeftMsgLen = (this->m_ClipBoardMsg.GetLength() * sizeof(TCHAR)) - StartIndex;
		/*
		 *	走査開始位置は「"」の位置。
		 *	範囲を閉じるための「"」が見つからなかった場合、範囲開始の「"」を抜き出しの開始位置とする。
		 */
		ExtractedMsg = this->m_ClipBoardMsg.Mid(StartIndex, LeftMsgLen);
		StartIndex += LeftMsgLen;
	}
	else {
		int ExtractDataLen = NextDQIndex - StartIndex - 1;
		ExtractedMsg = this->m_ClipBoardMsg.Mid(StartIndex + 1, ExtractDataLen);
		StartIndex = NextDQIndex + 1;
	}
}

/**
 *	@brief	\tを見つけた際の処理を行う。
 *			列を1つ進める。
 *
 *	@param[in,out]	RowIndex	行のインデックス
 *	@param[in,out]	ColIndex	列のインデックス
 *	@param[in,out]	StartIndex	走査/抽出の開始位置
 *	@param[out]		ExtractMsg	抽出したメッセージ
 */
VOID CPasteCommand::HandleTabSplitMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg)
{
	RowIndex++;
	StartIndex++;
}

/**
 *	@brief	\tを見つけた際の処理を行う。
 *			列を1つ進める。
 *
 *	@param[in,out]	RowIndex	行のインデックス
 *	@param[in,out]	ColIndex	列のインデックス
 *	@param[in,out]	StartIndex	走査/抽出の開始位置
 *	@param[out]		ExtractMsg	抽出したメッセージ
 */
VOID CPasteCommand::HandleCrLfSplitMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg)
{
	RowIndex = 0;
	ColIndex++;

	StartIndex += 2;
}

/**
 *	@brief	分割文字以外の文字/文字列を見つけた際の処理を行う。
 *
 *	@param[in,out]	RowIndex	行のインデックス
 *	@param[in,out]	ColIndex	列のインデックス
 *	@param[in,out]	StartIndex	走査/抽出の開始位置
 *	@param[out]		ExtractMsg	抽出したメッセージ
 */
VOID CPasteCommand::HandleOtherMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg)
{
	//次の区切り文字の位置を見つける。
}

#define	UPDATA_NEXT_SPLIT_INDEX(CompIndex, NextSplitIndex)		\
	do {														\
		if ((-1) != CompIndex) {								\
			if ((-1) == NextSplitIndex) {						\
				NextSplitIndex = CompIndex;						\
			}													\
			else {												\
				if (CompIndex < NextSplitIndex) {				\
					NextSplitIndex = CompIndex;					\
				}												\
			}													\
		}														\
	} while(0)


/**
 *	@brief	分割文字以外の文字/文字列を見つけた際の処理を行う。
 *
 *	@param[in,out]	RowIndex	行のインデックス
 *	@param[in,out]	ColIndex	列のインデックス
 *	@param[in,out]	StartIndex	走査/抽出の開始位置
 *	@param[out]		ExtractMsg	抽出したメッセージ
 *	@return	次の区切り文字のインデックス。区切り文字が見つからなかった場合には、-1を返す。
 */
int CPasteCommand::FindNextSplitWord(int& StartIndex)
{
	//各区切り文字の位置の走査を実施する
	int NextTabIndex = this->m_ClipBoardMsg.Find(_T('\t'), StartIndex);
	int NextCrIndex = this->m_ClipBoardMsg.Find(_T('\r'), StartIndex);
	int NextLfIndex = this->m_ClipBoardMsg.Find(_T('\n'), StartIndex);
	int NextDQIndex = this->m_ClipBoardMsg.Find(_T('"'), StartIndex);

	int NextSplitIndex = -1;
	UPDATA_NEXT_SPLIT_INDEX(NextTabIndex, NextSplitIndex);
	UPDATA_NEXT_SPLIT_INDEX(NextCrIndex, NextSplitIndex);
	UPDATA_NEXT_SPLIT_INDEX(NextLfIndex, NextSplitIndex);
	UPDATA_NEXT_SPLIT_INDEX(NextDQIndex, NextSplitIndex);
#if 0
	if ((-1) != NextTabIndex) {
		NextSplitIndex = NextTabIndex;
	}
	if ((-1) != NextCrIndex) {
		if ((-1) == NextSplitIndex) {
			NextSplitIndex = NextCrIndex;
		}
		else {
			if (NextCrIndex < NextSplitIndex) {
				NextSplitIndex = NextCrIndex;
			}
			else {
				//現在の区切り文字位置がCrよりも前の場合には、何もしない。
			}
		}
	}
	if ((-1) != NextLfIndex) {
		if ((-1) == NextSplitIndex) {
			NextSplitIndex = NextLfIndex;
		}
		else {
			if (NextLfIndex < NextSplitIndex) {
				NextSplitIndex = NextLfIndex;
			}
			else {
				//現在の区切り文字位置がCrよりも前の場合には、何もしない。
			}
		}
	}
#endif
	return NextSplitIndex;
}