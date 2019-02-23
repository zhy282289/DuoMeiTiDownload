#pragma once

#include <QObject>

#define ITEM_BIG_HEIGHT 140
#define ITEM_SMALL_HEIGHT 24

#define SAFE_DELETE(p) do{if (p) delete p; p=nullptr;}while(false)


#define ERROR_CODE_OK 0 //
#define ERROR_CODE_NETWORK_ERROR 1 //
#define ERROR_CODE_CONVERT_ERROR 2 //
#define ERROR_CODE_REMOTEPATH_NOT_EXIST_ERROR 3 //


enum VideoType
{
	TuiJian,
	YingShi,
	YinYue,
	ShiShang,
	WenHua,
	MeiShi,
};