#pragma once

#include <QObject>
#include <string>

QString gGetTouTiaoResource();
QString gGetTouTiaoTodayPath();
QString gGetTouTiaoTodaySourcePath();
QString gGetTouTiaoTodayDestePath(QString src);



QComboBox* gCreateVideoTypeComboBox(QWidget *parent);
QString gGetVideoTypeString(int videoType);

void gKeybdEvent(char key);
void gKeybdEvent_CTL(char key);

void gMoveCursorAndClick(QPoint point);