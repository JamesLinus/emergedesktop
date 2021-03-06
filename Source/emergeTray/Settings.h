/*!
  @file Settings.h
  @brief header for emergeTray
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

#ifndef __GUARD_d1fed683_9609_4406_ae88_15e14a42bf14
#define __GUARD_d1fed683_9609_4406_ae88_15e14a42bf14

#define UNICODE 1

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1 //suppress warnings about old versions of wcscpy, wcscat, etc.
#endif

#include <string>
#include <vector>
#include "../emergeBaseClasses/BaseSettings.h"

class Settings : public BaseSettings
{
public:
  Settings(LPARAM lParam);
  bool GetUnhideIcons();
  bool SetUnhideIcons(bool unhideIcons);
  UINT GetHideListSize();
  WCHAR* GetHideListItem(UINT item);
  void DeleteHideListItem(WCHAR* itemText);
  void ModifyHideListItem(WCHAR* oldText, WCHAR* newText);
  bool CheckHide(WCHAR* iconTip);
  void AddHideListItem(WCHAR* item);
  void WriteHideList();
  void BuildHideList();
  LOGFONT* GetInfoFont();
  LOGFONT* GetInfoTitleFont();
  bool SetInfoFont(LOGFONT* infoLogFont);
  bool SetInfoTitleFont(LOGFONT* infoTitleLogFont);
  COLORREF GetTextColor();
  bool SetTextColor(COLORREF textColor);
  COLORREF GetBorderColor();
  bool SetBorderColor(COLORREF borderColor);
  COLORREF GetGradientFrom();
  bool SetGradientFrom(COLORREF gradientFrom);
  bool SetGradientTo(COLORREF gradientTo);
  COLORREF GetGradientTo();
  int GetAlpha();
  bool SetAlpha(int alpha);
  std::wstring GetGradientMethod();
  bool SetGradientMethod(std::wstring gradientMethod);

protected:
  virtual void DoReadSettings(IOHelper& helper);
  virtual void DoWriteSettings(IOHelper& helper);
  virtual void ResetDefaults();
  virtual void DoInitialize();

private:
  LPARAM lParam;
  std::vector<std::wstring> hideList;
  std::wstring xmlFile;
  bool unhideIcons;
  LOGFONT infoLogFont, infoTitleLogFont;
  std::wstring infoFontString, infoTitleFontString, gradientMethod;
  COLORREF borderColour, textColour, gradientTo, gradientFrom;
  int alpha;
};

#endif
