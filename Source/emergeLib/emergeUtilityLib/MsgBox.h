/*!
  @file MsgBox.h
  @brief internal header for emergeLib
  @author The Emerge Desktop Development Team

  @attention This file is part of Emerge Desktop.
  @attention Copyright (C) 2004-2013  The Emerge Desktop Development Team

  @attention Emerge Desktop is free software; you can redistribute it and/or
  modify  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  @attention Emerge Desktop is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  @attention You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */

#ifndef __GUARD_299e40a7_b9bb_484f_a701_1836da2c2251
#define __GUARD_299e40a7_b9bb_484f_a701_1836da2c2251

#define UNICODE 1

#define MAX_LINE_LENGTH 4096

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1 //suppress warnings about old versions of wcscpy, wcscat, etc.
#endif

#undef _WIN32_IE
#define _WIN32_IE	0x501

#undef _WIN32_WINNT
#define _WIN32_WINNT	0x600

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

#ifdef __GNUC__
#include <tr1/memory>
#include <tr1/shared_ptr.h>
#else
#include <memory>
#endif

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <map>
#include <string>
#include "../emergeUtilityLib.h"
#include "../emergeWindowLib.h"
#include "../resource.h"

typedef std::map<HWND, HHOOK> HookMap;
static HookMap hookMap;

class MsgBox
{
public:
  MsgBox(HINSTANCE hInstance, HWND mainWnd, std::wstring messageText, std::wstring messageTitle, DWORD iconType, DWORD buttonType);
  ~MsgBox();
  int Show(bool modal);
  LRESULT DoInitDialog(HWND hwndDlg);
  LRESULT DoCommand(HWND hwndDlg, WPARAM wParam, LPARAM lParam);
  bool IsHookMessage(LPMSG lpMsg);

private:
  HINSTANCE hInstance;
  HWND mainWnd, toolWnd;
  HICON msgIcon;
  DWORD msgButtons; // make this global for now, for some reason it causes a crash when defined as a class variable
  WCHAR* sound;
  bool modal;
  WCHAR msgText[MAX_LINE_LENGTH], msgTitle[MAX_LINE_LENGTH];
  static BOOL CALLBACK MsgBoxDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif
