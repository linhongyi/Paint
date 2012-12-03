#include "StdAfx.h"
#include "PenSetting.h"
#include "Resource.h"
#include "CommonVar.h"

PenSetting::PenSetting(void)
{
}

PenSetting::~PenSetting(void)
{
}

/** id顏色定義轉rgb*/
COLORREF PenSetting::TransferIDToPenColor(int penColor)
{
    COLORREF myPenColor;

    switch( penColor )
    {
        case IDM_GREEN:
            myPenColor = RGB(0,255,0);
            break;
        case IDM_RED:
            myPenColor = RGB(255,0,0);
            break;
        case IDM_YELLOW:
            myPenColor = RGB(255,255,0);
            break;
        case IDM_BLUE:
             myPenColor = RGB(0,0,255);
            break;
        case IDM_PINK:
            myPenColor = RGB(255,128,192);
            break;
        case IDM_BLACK:
              myPenColor = RGB(0,0,0);
            break;
        case IDM_GRAY:
            myPenColor  = RGB(192,192,192);
            break;
        case IDM_SKYBLUE:
             myPenColor = RGB(64,128,128);
            break;
		case IDM_WHITE:
			myPenColor = RGB(255,255,255);
            break;
        default:
            myPenColor = RGB(0,0,0);
            break;
    }

    return myPenColor;
}
/** 筆寬id 轉筆寬*/
int PenSetting::TransferIDToPenWidth(int penWidth)
{
	int width;

    switch( penWidth )
    {
        case IDM_WIDTH_1:
            width = 1;
            break;
        case IDM_WIDTH_2:
            width = 2;
            break;
        case IDM_WIDTH_3:
            width = 3;
            break;
        case IDM_WIDTH_4:
            width = 4;
            break;
        case IDM_WIDTH_5:
            width = 5;
            break;
        default:
            width = 1;
            break;
    }

	return width;
}

/** id顏色定義轉rgb*/
COLORREF PenSetting::TransferIDToBg(int BgColor)
{
    COLORREF myBgColor;

    switch( BgColor )
    {
		//透明
	    case IDM_BG_TRANSPARENT:
			myBgColor = CommonVar::BG_TRANSPARENT;
            break;
        case IDM_BG_GREEN:
            myBgColor = RGB(0,255,0);
            break;
        case IDM_BG_RED:
            myBgColor = RGB(255,0,0);
            break;
        case IDM_BG_YELLOW:
            myBgColor = RGB(255,255,0);
            break;
        case IDM_BG_BLUE:
            myBgColor = RGB(0,0,255);
            break;
        case IDM_BG_PINK:
            myBgColor = RGB(255,128,192);
            break;
        case IDM_BG_BLACK:
            myBgColor = RGB(0,0,0);
            break;
        case IDM_BG_GRAY:
            myBgColor = RGB(192,192,192);
            break;
        case IDM_BG_SKYBLUE:
            myBgColor = RGB(64,128,128);
            break;
        default:
            myBgColor = RGB(0,0,0);
            break;
    }

    return myBgColor;
}