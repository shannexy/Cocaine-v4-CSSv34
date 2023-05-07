#pragma once

#include <windows.h>

#include <memory>

namespace Input
{
	class Win32
	{
	public:
		Win32();
		~Win32();

		bool					Capture();
		bool					Release();

		const HWND				GetTarget() const;

	private:
		static LRESULT WINAPI	Proxy( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	private:
		HWND					m_hTarget;
		WNDPROC					m_pProcedure;
	};
}