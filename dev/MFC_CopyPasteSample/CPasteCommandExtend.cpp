#include "pch.h"
#include "CPasteCommandExtend.h"

/**
 *	文字列を分割する。
 */
VOID CPasteCommandExtend::SplitMsg()
{
	CString StringSeparator = _T("\t");
	CString	SplitToken = _T("");

	CString SplitMsgBuffer[10][10] = { _T("-") };
	//初期化
	for (INT_PTR BuffRowIndex = 0; BuffRowIndex < 10; BuffRowIndex++) {
		for (INT_PTR BuffColIndex = 0; BuffColIndex < 10; BuffColIndex++) {
			SplitMsgBuffer[BuffRowIndex][BuffColIndex] = _T(" - ");
		}
	}

	INT_PTR SplitPos = 0;
	INT_PTR RowIndex = 0;
	INT_PTR ColIndex = 0;
	SplitToken = this->m_ClipBoardMsg.Tokenize(StringSeparator, SplitPos);
	while (!SplitToken.IsEmpty()) {
		if ((_T('\"') == SplitToken.Left(1)) && (_T('\"') == SplitToken.Right(1))) {
			/*
			 *	先頭と末尾の文字が「"」	→	中に改行を含む文字列
			 *							→	「"」を除去
			 */
			SplitToken = SplitToken.TrimLeft(1);
			SplitToken = SplitToken.Right(SplitToken.GetLength() - 2);
		}
		SplitMsgBuffer[RowIndex][ColIndex] = SplitToken;
		if (_T("\r\n") == SplitToken.Right(2)) {
			//末尾2文字が改行コードの場合には、バッファの位置を変更する。
			RowIndex++;
			ColIndex = 0;
		}
		else {
			ColIndex++;
		}
		SplitToken = this->m_ClipBoardMsg.Tokenize(StringSeparator, SplitPos);
	}

	for (INT_PTR Row = 0; Row < RowIndex; Row++) {
		for (INT_PTR Col = 0; Col < ColIndex; Col++) {
			TRACE(_T("(%d, %d) : %s\n"), SplitMsgBuffer[Row][Col]);
		}
	}
}