#include "pch.h"
#include "CPasteCommand.h"

/**
 *	�R�}���h�̎��s����
 */
VOID CPasteCommand::PrepCommand(INT_PTR MsgId, HWND WndHnd)
{
	this->m_MsgId = MsgId;
	this->m_WndHnd = WndHnd;
}

/**
 *	@brief	�\��t���̎��s
 */
BOOL CPasteCommand::Execute()
{
	//�N���b�v�{�[�h���J���āA�l���擾����
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
 *	@brief	�\��t�������̕���
 */
VOID CPasteCommand::SplitByToken()
{
	int CurPos = 0;
	CString SplitToken = _T("\t");	//�Z���̋�؂�
	CString ResToken = this->m_ClipBoardMsg.Tokenize(SplitToken, CurPos);
	while (ResToken != _T("")) {
		TRACE(_T("%s\r\n"), ResToken);
		ResToken = this->m_ClipBoardMsg.Tokenize(SplitToken, CurPos);
	}
}

/**
 *	������𕪊�����B
 *
 *	@param[in,out]	Src	�����Ώۂ̕�����
 *	@param[out]	HasLineChange	����/�����o�����������
 */
CString CPasteCommand::SplitByToken(CString& Src, BOOL& HasLineChage, INT_PTR& StartIndex)
{
	CString SeparatorCrLf = _T("");
	CString SeparatorTab = _T("");
	CString SeparatorOpt_DQ = _T("");
	TCHAR DQ = _T('\"');
	INT_PTR SeparatorIndexCrLf = 0;
	INT_PTR SeparatorIndexTab = 0;

	//���̃Z�p���[�^(���s/�^�u)�̊J�n�ʒu��������B
	TCHAR* SrcBuffer = Src.GetBuffer();
	if (DQ == SrcBuffer[StartIndex]) {
		SeparatorCrLf = _T("\"\r\n");
		SeparatorTab = _T("\"\t");
		StartIndex++;	//�����o���������񂪁u"�v���܂܂Ȃ��悤�A�����o���J�n�ʒu��1�i�߂�B
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
		 *	���s�R�[�h���^�u��������Ȃ�����
		 *		��	�Ō�܂Ŕ����o���B
		 */
		SeparatorLen = Src.GetLength() - SeparatorIndex;
		SeparatorIndex = Src.GetLength();
		HasLineChage = FALSE;
	}
	else if ((SeparatorIndexCrLf < 0) && (0 <= SeparatorIndexTab)) {
		//�^�u��������������(���s�R�[�h�Ȃ�)
		SeparatorLen = SeparatorTab.GetLength();
		SeparatorIndex = SeparatorIndexTab;
		HasLineChage = FALSE;
	}
	else if ((0 <= SeparatorIndexCrLf) && (SeparatorIndexTab < 0)) {
		//���s�R�[�h��������������(�^�u�Ȃ�)
		SeparatorLen = SeparatorCrLf.GetLength();
		SeparatorIndex = SeparatorIndexCrLf;
		HasLineChage = TRUE;
	}
	else {
		/*
		 *	��L�ȊO(���s�R�[�h�ƃ^�u�̗�������������
		 *		��	��Ɍ��������Z�p���[�^���̗p����
		 */
		if (SeparatorIndexCrLf < SeparatorIndexTab) {
			//���s�R�[�h����Ɍ�������
			SeparatorLen = SeparatorCrLf.GetLength();
			SeparatorIndex = SeparatorIndexCrLf;
			HasLineChage = TRUE;
		} else if (SeparatorIndexTab < SeparatorIndexCrLf) {
			//�^�u����Ɍ�������
			SeparatorLen = SeparatorTab.GetLength();
			SeparatorIndex = SeparatorIndexTab;
			HasLineChage = FALSE;
		}
		else {
			//���s�R�[�h�E�^�u�������ʒu�����蓾�Ȃ��F���B�s�\�����A�O�̂��ߋL�ځB
		}
	}
	INT_PTR Count = SeparatorIndex - StartIndex;
	CString Token = Src.Mid(StartIndex, Count);
	StartIndex = StartIndex + (Count + SeparatorLen);

	return Token;
}


VOID CPasteCommand::SplitMsg()
{
	//������̕����̊J�n
	int StartIndex = 0;
	int EndIndex = this->m_ClipBoardMsg.GetLength();
	CString SplitMsgBuffer[10][10];
	//������
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
 *	@brief	�u"�v�ň͂܂�镶����𒊏o����B
 *
 *	@param[in,out]	RowIndex	�s�̃C���f�b�N�X
 *	@param[in,out]	ColIndex	��̃C���f�b�N�X
 *	@param[in,out]	StartIndex	����/���o�̊J�n�ʒu
 *	@param[out]		ExtractMsg	���o�������b�Z�[�W
 */
VOID CPasteCommand::HandleDQSplitMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg)
{
	int NextDQIndex = this->m_ClipBoardMsg.Find(_T('"'), StartIndex + 1);
	if ((-1) == NextDQIndex) {
		int LeftMsgLen = (this->m_ClipBoardMsg.GetLength() * sizeof(TCHAR)) - StartIndex;
		/*
		 *	�����J�n�ʒu�́u"�v�̈ʒu�B
		 *	�͈͂���邽�߂́u"�v��������Ȃ������ꍇ�A�͈͊J�n�́u"�v�𔲂��o���̊J�n�ʒu�Ƃ���B
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
 *	@brief	\t���������ۂ̏������s���B
 *			���1�i�߂�B
 *
 *	@param[in,out]	RowIndex	�s�̃C���f�b�N�X
 *	@param[in,out]	ColIndex	��̃C���f�b�N�X
 *	@param[in,out]	StartIndex	����/���o�̊J�n�ʒu
 *	@param[out]		ExtractMsg	���o�������b�Z�[�W
 */
VOID CPasteCommand::HandleTabSplitMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg)
{
	RowIndex++;
	StartIndex++;
}

/**
 *	@brief	\t���������ۂ̏������s���B
 *			���1�i�߂�B
 *
 *	@param[in,out]	RowIndex	�s�̃C���f�b�N�X
 *	@param[in,out]	ColIndex	��̃C���f�b�N�X
 *	@param[in,out]	StartIndex	����/���o�̊J�n�ʒu
 *	@param[out]		ExtractMsg	���o�������b�Z�[�W
 */
VOID CPasteCommand::HandleCrLfSplitMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg)
{
	RowIndex = 0;
	ColIndex++;

	StartIndex += 2;
}

/**
 *	@brief	���������ȊO�̕���/��������������ۂ̏������s���B
 *
 *	@param[in,out]	RowIndex	�s�̃C���f�b�N�X
 *	@param[in,out]	ColIndex	��̃C���f�b�N�X
 *	@param[in,out]	StartIndex	����/���o�̊J�n�ʒu
 *	@param[out]		ExtractMsg	���o�������b�Z�[�W
 */
VOID CPasteCommand::HandleOtherMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg)
{
	//���̋�؂蕶���̈ʒu��������B
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
 *	@brief	���������ȊO�̕���/��������������ۂ̏������s���B
 *
 *	@param[in,out]	RowIndex	�s�̃C���f�b�N�X
 *	@param[in,out]	ColIndex	��̃C���f�b�N�X
 *	@param[in,out]	StartIndex	����/���o�̊J�n�ʒu
 *	@param[out]		ExtractMsg	���o�������b�Z�[�W
 *	@return	���̋�؂蕶���̃C���f�b�N�X�B��؂蕶����������Ȃ������ꍇ�ɂ́A-1��Ԃ��B
 */
int CPasteCommand::FindNextSplitWord(int& StartIndex)
{
	//�e��؂蕶���̈ʒu�̑��������{����
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
				//���݂̋�؂蕶���ʒu��Cr�����O�̏ꍇ�ɂ́A�������Ȃ��B
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
				//���݂̋�؂蕶���ʒu��Cr�����O�̏ꍇ�ɂ́A�������Ȃ��B
			}
		}
	}
#endif
	return NextSplitIndex;
}