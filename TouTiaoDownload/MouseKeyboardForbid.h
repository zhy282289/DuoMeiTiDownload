#pragma once

#include <QObject>

class MouseKeyboardForbid 
{
public:
	MouseKeyboardForbid();
	~MouseKeyboardForbid();

	static MouseKeyboardForbid* GetInstance();

	void SetEnabled(bool enabled);
private:
	static LRESULT CALLBACK JournalPlaybackProc(int code, WPARAM wParam, LPARAM lParam);

private:
};


#define MOUSEKEYBOARD_FORBID MouseKeyboardForbid::GetInstance()
#define MOUSEKEYBOARD_FORBID_ENABLED MouseKeyboardForbid::GetInstance()->SetEnabled(true);
#define MOUSEKEYBOARD_FORBID_DISABLED MouseKeyboardForbid::GetInstance()->SetEnabled(false);
