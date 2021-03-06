//---
//
//  This file is part of Emerge Desktop.
//  Copyright (C) 2004-2013  The Emerge Desktop Development Team
//
//  Emerge Desktop is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  Emerge Desktop is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//---

#include "Settings.h"

Settings::Settings()
  : BaseSettings(false)
{
  xmlFile = TEXT("%EmergeDir%\\files\\emergeVWM.xml");
  rows = 2;
  columns = 2;
  hideSticky = false;
}

Settings::~Settings()
{
  // Clear the stickyList vector
  stickyList.clear();
}

void Settings::DoReadSettings(IOHelper& helper)
{
  BaseSettings::DoReadSettings(helper);
  rows = helper.ReadInt(TEXT("DesktopRows"), 2);
  columns = helper.ReadInt(TEXT("DesktopColumns"), 2);
  hideSticky = helper.ReadBool(TEXT("HideSticky"), false);
}

void Settings::DoWriteSettings(IOHelper& helper)
{
  BaseSettings::DoWriteSettings(helper);
  helper.WriteInt(TEXT("DesktopRows"), rows);
  helper.WriteInt(TEXT("DesktopColumns"), columns);
  helper.WriteBool(TEXT("HideSticky"), hideSticky);
}

void Settings::ResetDefaults()
{
  BaseSettings::ResetDefaults();
  rows = 2;
  columns = 2;
  hideSticky = false;
  x = 216;
}

bool Settings::GetHideSticky()
{
  return hideSticky;
}

int Settings::GetDesktopRows()
{
  return (int)rows;
}

int Settings::GetDesktopColumns()
{
  return (int)columns;
}

bool Settings::SetHideSticky(bool hideSticky)
{
  if (this->hideSticky != hideSticky)
  {
    this->hideSticky = hideSticky;
    SetModified();
  }
  return true;
}

bool Settings::SetDesktopRows(int rows)
{
  if (this->rows != rows)
  {
    this->rows = rows;
    SetModified();
  }
  return true;
}

bool Settings::SetDesktopColumns(int columns)
{
  if (this->columns != columns)
  {
    this->columns = columns;
    SetModified();
  }
  return true;
}

//-----
// Function:	BuildStickyList
// Required:	Nothing
// Returns:	Nothing
// Purpose:	Enumerates the Sticky key and builds the stickyList vector
//-----
void Settings::BuildStickyList()
{
  std::tr1::shared_ptr<TiXmlDocument> configXML = ELOpenXMLConfig(xmlFile, false);
  TiXmlElement* section;
  std::wstring data;

  if (configXML)
  {
    // Clear the stickyList vector
    stickyList.clear();
    section = ELGetXMLSection(configXML.get(), TEXT("Sticky"), false);

    if (section)
    {
      IOHelper userIO(section);

      while (userIO.GetElement())
      {
        data = userIO.ReadString(TEXT("Application"), TEXT(""));
        if (!data.empty())
        {
          stickyList.push_back(data);
        }
      }
    }
  }
}

//-----
// Function:	CheckSticky
// Required:	HWND wnd - window handle to check
// Returns:	bool
// Purpose:	Checks to see if the window should be sticky
//-----
bool Settings::CheckSticky(std::wstring appName)
{
  UINT i;
  bool titleMatch = false;
  std::wstring tmp = ELToLower(appName);

  for (i = 0; i < stickyList.size(); i++)
  {
    if (appName == ELToLower(stickyList[i]))
    {
      titleMatch = true;
    }
  }

  return titleMatch;
}

UINT Settings::GetStickyListSize()
{
  return (UINT)stickyList.size();
}

WCHAR* Settings::GetStickyListItem(UINT item)
{
  return (WCHAR*)stickyList[item].c_str();
}

void Settings::DeleteStickyListItem(UINT item)
{
  stickyList.erase(stickyList.begin() + item);
}

void Settings::AddStickyListItem(WCHAR* item)
{
  stickyList.push_back(item);
}

void Settings::WriteStickyList()
{
  std::tr1::shared_ptr<TiXmlDocument> configXML = ELOpenXMLConfig(xmlFile, true);
  TiXmlElement* section;

  if (configXML)
  {
    section = ELGetXMLSection(configXML.get(), TEXT("Sticky"), true);

    if (section)
    {
      IOHelper userIO(section);

      userIO.Clear();
      for (UINT i = 0; i < stickyList.size(); i++)
      {
        if (userIO.SetElement(TEXT("item")))
        {
          userIO.WriteString(TEXT("Application"), stickyList[i]);
        }
      }

    }

    ELWriteXMLConfig(configXML.get());
  }
}
