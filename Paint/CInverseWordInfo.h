#pragma once
#include <vector>
using namespace std;
class CInverseWordInfo
{
public:
	CInverseWordInfo(void);
	~CInverseWordInfo(void);
	void ResetInverseWordScope();
	void SetCaretPt1( POINT pt );
	void SetCaretPt2( POINT pt );

    POINT GetCaretPt1();
	POINT GetCaretPt2();
private:
	
	POINT _caretPt1, _caretPt2;

};

