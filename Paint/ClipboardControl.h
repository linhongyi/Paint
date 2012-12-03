#pragma once

#include <string>
#include "CGraphClipboardControl.h"
#include "CWordClipboardControl.h"
#include "Command.h"
using namespace std;

class PaintInfo;

class CClipboardControl
{
public:
	CClipboardControl(void);
	~CClipboardControl(void);
	void SetParent(PaintInfo* ptr);
	void CopyControl();
	void PasteControl();
	void CutControl();
	void DeleteControl();
	void SetCGraphClipboardControl( CGraphClipboardControl* ptr );
	void SetCWordClipboardControl( CWordClipboardControl* ptr );
	CWordClipboardControl *GetCWordClipboardControl(){ return _cWordClipboardControl;};
	CGraphClipboardControl *GetCGraphClipboardControl(){ return _cGraphClipboardControl;};
	PaintInfo* GetPaintInfo(){ return _parent;};
private:
	PaintInfo*   _parent;
	CGraphClipboardControl* _cGraphClipboardControl;
	CWordClipboardControl* _cWordClipboardControl;
	void FindTargetGraphLocation(int ID,clipObj *buffer);
};

