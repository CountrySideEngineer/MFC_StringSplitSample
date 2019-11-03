#pragma once
class CPasteCommand
{
public:
	CPasteCommand() {}
	~CPasteCommand() {}

	virtual VOID PrepCommand(INT_PTR MsgId, HWND WndHnd);
	virtual BOOL Execute();

protected:
	virtual VOID SplitByToken();
	virtual CString SplitByToken(CString& Src, BOOL& HasLineChage, INT_PTR& NextStartIndex);
	virtual VOID SplitMsg();
	virtual VOID HandleDQSplitMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg);
	virtual VOID HandleTabSplitMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg);
	virtual VOID HandleCrLfSplitMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg);
	virtual VOID HandleOtherMsg(int& RowIndex, int& ColIndex, int& StartIndex, CString& ExtractedMsg);
	virtual int FindNextSplitWord(int& StartIndex);

protected:
	CString m_ClipBoardMsg;
	INT_PTR m_MsgId;
	HWND	m_WndHnd;	//ウィンドハンドラ
};

