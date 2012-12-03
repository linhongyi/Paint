#pragma once
#include <Windows.h>
class ToolKits
{
public:
	ToolKits(void);
	~ToolKits(void);

	static void IniDLL();
	static void ReleaseDLL();
	static void Swap(int &x,int &y );
	static void Swap(long &x, long &y);
	static void Normalization( POINT &iniPt, POINT &endPos);
	static void FindMaxScopeWithFourPoint( POINT &recPt1, POINT &recPt2, POINT &recPt3, POINT &recPt4);
	static void TunePtsWithPosY(POINT *ptrPt1, POINT *ptrPt2);
	static void InverseWordNormalization( POINT &iniPt, POINT &endPt);
	
private:
	static BOOL IsIniDLL();
	static void WelcomeUseThisDLL();
	//HINSTANCE _hLibrary;
};
