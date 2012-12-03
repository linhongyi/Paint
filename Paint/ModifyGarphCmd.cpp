#include "StdAfx.h"
#include "ModifyGarphCmd.h"


ModifyGarphCmd::ModifyGarphCmd(void)
{
}


ModifyGarphCmd::~ModifyGarphCmd(void)
{
}



void ModifyGarphCmd::Execute(RecordObj * recordObj)
{
	gPaintInfo.SetHitID( recordObj->ID);
	
	//塗白舊已已存在貼圖
	gPaintInfo.GetDrawTool()->FillWhileRec(recordObj->ptBeg,recordObj->ptEnd);

	gMyList.visitID_Node(  recordObj->ID );
	gMyList.ModifyCurrNodeInfo(recordObj->nObjType,recordObj->ptNewBeg,recordObj->ptNewEnd,recordObj->iNewPenColor,recordObj->iNewFigBg,recordObj->iNewPenWidth);
	
	gPaintInfo.GetCSelectRecInfo().SetIsShow(true);
	gPaintInfo.GetCSelectRecInfo().SetPosStart(recordObj->ptNewBeg);
	gPaintInfo.GetCSelectRecInfo().SetPosEnd(recordObj->ptNewEnd);
	gPaintInfo.GetScrollBarHandle()->TuneWindowView();

	//四個點，求所成最大方形
	POINT recPt1, recPt2, recPt3, recPt4;
	recPt1 = recordObj->ptBeg;
	recPt2 = recordObj->ptEnd;
	recPt3 = recordObj->ptNewBeg;
	recPt4 = recordObj->ptNewEnd;

	ToolKits::Normalization( recPt1, recPt2 );
	ToolKits::Normalization( recPt3, recPt4 );
	ToolKits::Normalization( recPt1, recPt3 );
	ToolKits::Normalization( recPt2, recPt4 );

	gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recPt1,recPt4 );
	gPaintInfo.GetDrawTool()->MapDCToMonitor();

	gPaintInfo.ColorMenuClickEvent( gPaintInfo.GetMainHwnd(), recordObj->iNewPenColor, 0 );
	gPaintInfo.FigBgMenuClickEvent( gPaintInfo.GetMainHwnd(), recordObj->iNewFigBg, 0 );
	gPaintInfo.PenWidthMenuClickEvent( gPaintInfo.GetMainHwnd(), recordObj->iNewPenWidth, 0 );
	recordObj = NULL;
}
	
void ModifyGarphCmd::UnExecute(RecordObj * recordObj)
{

	gPaintInfo.SetHitID( recordObj->ID);

	
	//塗白舊已已存在貼圖
	gPaintInfo.GetDrawTool()->FillWhileRec(recordObj->ptNewBeg,recordObj->ptNewEnd);

	gMyList.visitID_Node(  recordObj->ID );
	gMyList.ModifyCurrNodeInfo(recordObj->nObjType,recordObj->ptBeg,recordObj->ptEnd,recordObj->iPenColor,recordObj->iFigBg,recordObj->iPenWidth);

	gPaintInfo.GetCSelectRecInfo().SetIsShow(true);
	gPaintInfo.GetCSelectRecInfo().SetPosStart(recordObj->ptBeg);
	gPaintInfo.GetCSelectRecInfo().SetPosEnd(recordObj->ptEnd);
	
	gPaintInfo.GetScrollBarHandle()->TuneWindowView();

	//四個點，求所成最大方形
	POINT recPt1, recPt2, recPt3, recPt4;
	recPt1 = recordObj->ptBeg;
	recPt2 = recordObj->ptEnd;
	recPt3 = recordObj->ptNewBeg;
	recPt4 = recordObj->ptNewEnd;

	ToolKits::Normalization( recPt1, recPt2 );
	ToolKits::Normalization( recPt3, recPt4 );
	ToolKits::Normalization( recPt1, recPt3 );
	ToolKits::Normalization( recPt2, recPt4 );

	gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recPt1,recPt4 );
	gPaintInfo.GetDrawTool()->MapDCToMonitor();

	gPaintInfo.ColorMenuClickEvent( gPaintInfo.GetMainHwnd(), recordObj->iPenColor, 0 );
	gPaintInfo.FigBgMenuClickEvent( gPaintInfo.GetMainHwnd(), recordObj->iFigBg, 0 );
	gPaintInfo.PenWidthMenuClickEvent( gPaintInfo.GetMainHwnd(), recordObj->iPenWidth, 0 );
	recordObj = NULL;

}