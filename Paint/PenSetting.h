#pragma once

/** µeµ§³]©w*/
class PenSetting
{
public:
    PenSetting(void);
    ~PenSetting(void);
    
    static COLORREF TransferIDToPenColor(int penColor);
	static int      TransferIDToPenWidth(int penWidth);
	static COLORREF TransferIDToBg(int bgColor);
};
