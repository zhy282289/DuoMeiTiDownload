#pragma once

#include <QObject>

#include <string>

struct VInfo
{
	std::string codec_name;
	int width;
	int height;
	float display_aspect_ratio;
	float duration;
	float r_frame_rate;

	VInfo() {
		width = 0;
		height = 0;
		display_aspect_ratio = 0.0f;
		duration = 0.0f;
		r_frame_rate = 0.0f;
	}
};
struct AInfo
{
	std::string codec_name;
	int sample_rate;
	int channels;
	float duration;

	AInfo() {
		sample_rate = 0;
		channels = 0;
		duration = 0.0f;
	}
};

struct MediaInfo
{
	std::string filename;
	int nb_streams;
	float duration;
	int size;
	int bit_rate;

	VInfo vinfo;
	AInfo ainfo;
};


class VideoInfo : public QObject
{
	Q_OBJECT

public:
	VideoInfo(QObject *parent);
	~VideoInfo();

	static MediaInfo GetMediaInfo(QString path);

	static QString SecondToQString(int second);
};


