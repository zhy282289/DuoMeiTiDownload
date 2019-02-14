#include "stdafx.h"
#include "VideoInfo.h"

VideoInfo::VideoInfo(QObject *parent)
	: QObject(parent)
{
}

VideoInfo::~VideoInfo()
{
}

MediaInfo VideoInfo::GetMediaInfo(QString path)
{
	QString ffprobePath = QApplication::applicationDirPath() + "/ffmpeg/ffprobe.exe";
	QString cmd3 = QString("%1 -print_format json -show_format -show_streams %2").arg(ffprobePath).arg(path);
	QProcess process;
	process.start(cmd3);
	process.waitForReadyRead();
	auto json = process.readAllStandardOutput();
	process.waitForFinished();

	MediaInfo info;
	QJsonDocument doc =  QJsonDocument::fromJson(json);
	auto object = doc.object();
	auto streamsArray = object["streams"].toArray();
	for (int i = 0; i < streamsArray.size(); ++i)
	{
		auto streams1 = streamsArray[i].toObject();
		bool bVideo = streams1["codec_type"].toString() == "video";
		if (bVideo)
		{
			VInfo vinfo;

			vinfo.codec_name = streams1["codec_name"].toString().toLatin1().data();
			vinfo.width = streams1["width"].toInt();
			vinfo.height = streams1["height"].toInt();
			QStringList ratios = streams1["display_aspect_ratio"].toString().split(":");
			if (ratios.size() == 2)
				vinfo.display_aspect_ratio = 1.0f*ratios[0].toInt() / ratios[1].toInt();
			vinfo.duration = streams1["duration"].toString().toFloat();

			QStringList rates = streams1["r_frame_rate"].toString().split("/");
			if (rates.size() == 2)
				vinfo.r_frame_rate = 1.0f*rates[0].toInt() / rates[1].toInt();

			info.vinfo = vinfo;
		}
		else
		{
			AInfo aInfo;
			aInfo.codec_name = streams1["codec_name"].toString().toLatin1().data();
			aInfo.sample_rate = streams1["sample_rate"].toString().toInt();
			aInfo.channels = streams1["channels"].toInt();
			aInfo.duration = streams1["duration"].toString().toFloat();

			info.ainfo = aInfo;

		}


	}

	auto formatObject = object["format"].toObject();
	if (!formatObject.isEmpty())
	{
		info.filename = formatObject["filename"].toString().toLocal8Bit().data();
		info.nb_streams = formatObject["nb_streams"].toInt();
		info.duration = formatObject["duration"].toString().toFloat();
		info.size = formatObject["size"].toString().toInt();
		info.bit_rate = formatObject["bit_rate"].toString().toInt();


	}

	return info;
}

QString VideoInfo::SecondToQString(int second)
{
	int hour = second / (60*60);
	second = second % (60*60);
	int minute = second / (60);
	second = second % 60;

	return QString("%1:%2:%3").arg(hour,2,10,QChar('0')).arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0'));
}
