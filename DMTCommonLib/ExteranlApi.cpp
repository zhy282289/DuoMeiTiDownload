#include "ExteranlApi.h"

#include <QDir>

void gMakeDirectory(QString path)
{
	QDir dir(path);
	if (!dir.exists(path))
	{
		dir.mkpath(path);
	}
}
