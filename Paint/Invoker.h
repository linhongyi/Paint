#pragma once
#include "Command.h"
#include "AddNewGraphCmd.h"
#include "DeleteGraphCmd.h"
#include "ModifyGarphCmd.h"
#include "ChangeWordBgColorCommand.h"
#include "ChangeWordColorCommand.h"
#include "ChangeWordRecScopeCommand.h"
#include "NewTarGetGraphSelectCmd.h"
#include "WarningCmd.h"
#include "AddWordCmd.h"
#include "Command.h"
#include "DeleteWordCmd.h"
#include "ModifyEditWordCmd.h"

class CInvoker
{
public:
	CInvoker(void);
	~CInvoker(void);
	Command& GetRedoUndoCmd(RedoUndoCmd cmd);
private:
	AddNewGraphCmd _addNewGarphCmd;
	DeleteGraphCmd _deleteGraphCmd;
	ModifyGarphCmd _modifyGarphCmd;

	ChangeWordBgColorCommand  _changeWordBgColorCommand;
	ChangeWordColorCommand    _changeWordColorCommand;
	ChangeWordRecScopeCommand _changeWordRecScopeCommand;
	NewTarGetGraphSelectCmd   _newTarGetGraphSelectCmd;
	WarningCmd                _warningCmd;
	AddWordCmd                _addWordCmd;

	DeleteWordCmd             _deleteWordCmd;
	ModifyEditWordCmd         _modifyEditWordCmd;

};

