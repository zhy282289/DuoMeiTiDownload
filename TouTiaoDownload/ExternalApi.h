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

void gWebViewScrollBottom(QWebEnginePage *page, std::function<void(QVariant)> callback);
void gWebViewScrollTop(QWebEnginePage *page, std::function<void(QVariant)> callback);

void gInitPythonPath();


#define MOUSEKEYBOARD_FORBID_DISABLED	BlockInput(false);
#define MOUSEKEYBOARD_FORBID_ENABLED	BlockInput(true);
