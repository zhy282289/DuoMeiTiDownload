#pragma once

#include <QObject>

QString gGetTouTiaoResource();
QString gGetTouTiaoTodayPath();
QString gGetTouTiaoTodaySourcePath();
QString gGetTouTiaoTodayDestePath(QString src);



QComboBox* gCreateVideoTypeComboBox(QWidget *parent);
QString gGetVideoTypeString(int videoType);