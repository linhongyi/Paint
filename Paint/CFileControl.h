#pragma once

#include <CommDlg.h>

#define IDM_ENCRYPT 100
#define UNTITLED _T("Untitled")
class PaintInfo;
class CFileControl
{
public:
	CFileControl(void);
	~CFileControl(void);
	void PopFileInitialize(HWND hWnd);
	void SetParent(PaintInfo *ptr);
	void FileCreate(HWND hWnd);
	void SaveFile(HWND hWnd);
	void SaveFileAs(HWND hWnd);
	void OpenFile(HWND hWnd);
	void CloseProgram(HWND hWnd);
	void newChangeWithFile(BOOL foo);
	void ClickClearMenu(HWND hWnd);
	void ResetFileNameInfo(HWND hWnd);
private:
	PaintInfo *_parent;
	OPENFILENAME ofn;

	TCHAR szFileName[MAX_PATH];
	TCHAR szTitleName[MAX_PATH];
	TCHAR szAppName[MAX_PATH];
    BOOL _bNeedSave;
	BOOL PopFileSaveDlg(HWND hWnd, PTSTR pstrFileName, PTSTR pstrTitleName);
	BOOL PopFileOpenDlg(HWND hWnd, PTSTR pstrFileName, PTSTR pstrTitleName);
	BOOL PopFileRead(HWND hWndEdit, PTSTR pstrFileName);
	BOOL PopFileWrite(HWND hWndEdit, PTSTR pstrFileName);
	void DoCaption(HWND hWnd, TCHAR* szTitleName,  bool bShowStar );
	void WriteQueueNodeToFile(HANDLE &hFile, DWORD &dwBytesWritten);
	BOOL ReadFileToQueueNode(HANDLE &hFile, DWORD &dwBytesRead);
	short AskAboutSave(HWND hWnd, TCHAR* szTitleName);
	short AskAboutExit(HWND hWnd);
	void OkMessage(HWND hWnd, TCHAR* szMessage, TCHAR* sTitleName );



};