#include "pch.h"
#include "CPasteCommandExtend.h"

/**
 *	������𕪊�����B
 */
VOID CPasteCommandExtend::SplitMsg()
{
	CString StringSeparator = _T("\t");
	CString	SplitToken = _T("");

	CString SplitMsgBuffer[10][10] = { _T("-") };
	//������
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
			 *	�擪�Ɩ����̕������u"�v	��	���ɉ��s���܂ޕ�����
			 *							��	�u"�v������
			 */
			SplitToken = SplitToken.TrimLeft(1);
			SplitToken = SplitToken.Right(SplitToken.GetLength() - 2);
		}
		SplitMsgBuffer[RowIndex][ColIndex] = SplitToken;
		if (_T("\r\n") == SplitToken.Right(2)) {
			//����2���������s�R�[�h�̏ꍇ�ɂ́A�o�b�t�@�̈ʒu��ύX����B
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