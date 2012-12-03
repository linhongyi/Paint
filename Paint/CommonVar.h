#pragma once

#include <vector>
using namespace std;


/** 復原、重做命令集定義*/
enum RedoUndoCmd
{
 COMMAND_DRAW_NEW_GRAPH =  0,
 COMMAND_FIX_GRAPH      =  1,
 COMMAND_DELETE_GRAPH   =  2,

 COMMAND_ADD_WORD       =  3,
 COMMAND_FIX_COLOR_WORD =  4,
 COMMAND_FIX_BGCOLOR_WORD =5,
 COMMAND_CHANGE_WORD_REC = 6,
 COMMAND_DELETE_WORD     = 7,
 COMMAND_NEW_TRAGET_GRAPH_SEL = 8,

 COMMAND_EDIT_WORD         = 9,

};

/**插入節點時，插在何處的定義*/
enum InsertAction
{
	InsertNone = 4,
	InsertHead = 0,
	InsertTail = 1,
	InsertMid  = 2
};

/**滑鼠鼠標形狀定義*/
enum cursorShape
{
	CursorDirection_NW = 0,
	CursorDirection_SE = 1,
	CursorDirection_SW = 2,
	CursorDirection_NE = 3,
	CursorDirection_N  = 4,
	CursorDirection_W  = 5,
	CursorDirection_E  = 6,
	CursorDirection_S  = 7,

	CursorDirection_ARROW = 8,
	CursorDirection_SIZEALL =9,
	CursorDirection_IBEAM   = 10
};


/******************
* 共用變數定義集
********************/
class CommonVar
{
public:
	static const COLORREF BG_TRANSPARENT = RGB(125,125,125);
	static const COLORREF COLOR_ORANGE   = RGB(255,140,0);
	static const COLORREF COLOR_BLACK    = RGB(0,0,0);
	static const COLORREF COLOR_PURPLE   = RGB(128,128,255);

	static const int DRAW_MAP_WIDTH  = 5000;  //畫布最大寬度
	static const int DRAW_MAP_HEIGHT = 5000;  //畫布最大高度


	static const int SMALL_RECT_BUFFER = 3;   //選取方框項點小正方形寬度
	static const int SHIFT_PIXEL     = 30;    //碰邊界時，一次移動的像素

	static const int WORD_REC_MARGIN = 4;     //文字方框與文字間的間格
	static const int WORD_GAP        = 1;     //文字間格
	static const int CARET_WIDTH     = 1;     //打字顯示符號寬度
	static const int MAX_WORD        = 5000;  //最大文字個字定義
	static const int OUT_WINDOW_CONSTANT = 60000;  //滑鼠移出視窗，邊界值定義
	static const int MAX_PEN_WIDTH = 5;            //最大筆寬
	CommonVar(void);
	~CommonVar(void);
};

/**剪貼簿物件*/
struct clipObj
{
	unsigned short nObjType;
	POINT ptBeg;
	POINT ptEnd;

    unsigned short iPenColor;
	unsigned short iFigBg;
    unsigned short iPenWidth;

	POINT wordIniPt;
	BOOL IsSegmentWord;
};

/**檔案物件*/
struct FileObj
{
	unsigned short nObjType;
	POINT ptBeg;
	POINT ptEnd;

    unsigned short iPenColor;
	unsigned short iFigBg;
    unsigned short iPenWidth;
	unsigned short iCharacterNum;

};

/** RedoUndo物件*/
struct RecordObj
{
	RedoUndoCmd Command;
	int ID;
	unsigned short nObjType;

	POINT ptBeg;
	POINT ptEnd;

	POINT ptNewBeg;
	POINT ptNewEnd;

    unsigned short iPenColor;
	unsigned short iFigBg;
    unsigned short iPenWidth;

	unsigned short iNewPenColor;
	unsigned short iNewFigBg;
	unsigned short iNewPenWidth;
	vector<TCHAR>vecOriCharacater;
	vector<TCHAR>vecCharacter;
};