#pragma once

#include <QObject>

#define ITEM_BIG_HEIGHT 140
#define ITEM_SMALL_HEIGHT 24

#define SAFE_DELETE(p) do{if (p) delete p; p=nullptr;}while(false)
