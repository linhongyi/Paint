#pragma once
#include "CommonVar.h"


class Memento;
class Originator
{
public:
	Originator(void);
	~Originator(void);
	void SetRecordObj(RecordObj *recordObj);
	Memento* CreateMemento();
	void SetMementoState(Memento *pMem);
	void SetRedoUndoFlag(int val);
private:
	RecordObj *_recordObj;
	int _iRedoUndoFlag;
};