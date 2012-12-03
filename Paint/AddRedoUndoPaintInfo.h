#pragma once
#include "CommonVar.h"

class AddRedoUndoPaintInfo
{
public:
	AddRedoUndoPaintInfo(void);
	~AddRedoUndoPaintInfo(void);
	void AddModifySizeInfo(int ID,POINT posBeg,POINT posEnd);
	void AddModifyPenColorInfo(int penColor);
	void AddModifyPenWidthInfo(int penWidth);
	void AddModifyBgColorInfo(int bgColor);
	void AddDeleteNodeInfo(int ID);
	void AddNewNodeInfo(int ID);
	void AddEditWordNodeInfo(int ID);

};

