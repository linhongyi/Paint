#pragma once

#include "Originator.h"
#include "CommonVar.h"
#include <vector>
using namespace std;

class Memento;
class RedoUndoController
{
public:
	RedoUndoController(Originator* originator);
	~RedoUndoController(void);
	
	void Record();
	void Redo();
	void Undo();
	void Reset();
	void PopFinalWord();
	int  GetFinalID();
	int  GetLastCmd();
	void TraceAllRecord();

	BOOL IsRedo();
	BOOL IsUndo();
private:
	int _index;
    BOOL _IsUndo;
    Originator* _originator; 
    vector<Memento*> _memoList; 
 
};

