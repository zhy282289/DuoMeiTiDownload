#include "stdafx.h"
#include "MouseKeyboardForbid.h"

static HHOOK m_hook = nullptr;

MouseKeyboardForbid::MouseKeyboardForbid()
{
}

MouseKeyboardForbid::~MouseKeyboardForbid()
{
}

MouseKeyboardForbid* MouseKeyboardForbid::GetInstance()
{
	static MouseKeyboardForbid instance;
	return &instance;
}

void MouseKeyboardForbid::SetEnabled(bool enabled)
{
	if (enabled)
	{
		m_hook = SetWindowsHookEx(WH_JOURNALPLAYBACK, JournalPlaybackProc, (HINSTANCE)GetModuleHandle(L"DMTCommonLibD.dll"), 0);
		auto err = GetLastError();
		LOG(QString("MouseKeyboardForbid::SetEnabled:%1").arg((int)m_hook));

	}
	else
	{
		if (m_hook)
			UnhookWindowsHookEx(m_hook);
		m_hook = nullptr;
	}
}

LRESULT CALLBACK MouseKeyboardForbid::JournalPlaybackProc(int code, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(m_hook, code, wParam, lParam);
}
