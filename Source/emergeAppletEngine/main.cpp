//----  --------------------------------------------------------------------------------------------------------
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
//----  --------------------------------------------------------------------------------------------------------

#include "main.h"

//----  --------------------------------------------------------------------------------------------------------
// Function:    EAEHitTest
// Requires:    HWND hwnd - window handle
//              int x - x co-ordinate of mouse
//              int y - y co-ordinate of mouse
// Returns:     LRESULT
// Purpose:     Determine the mouse pointer type for the hit test
//----  --------------------------------------------------------------------------------------------------------
LRESULT EAEHitTest(HWND hwnd, int guiBorder, bool autoSize, int x, int y)
{
  RECT r;
  int dragBorder = guiBorder;

  if (dragBorder < 5)
  {
    dragBorder = 5;
  }

  r = ELGetWindowRect(hwnd);
  InflateRect(&r, -dragBorder, -dragBorder);

  // For some reason the bottom and right check seem to be thinner than the top
  // and the left, so take an extra 2 pixels off the bottom and right edge to
  // compensate for it.
  r.bottom -= 2;
  r.right -= 2;

  if (ELIsKeyDown(VK_CONTROL))
  {
    return HTCAPTION;
  }
  else if (ELIsKeyDown(VK_MENU) && !autoSize)
  {
    if ((x <= r.left) && (y <= r.top))
    {
      return HTTOPLEFT;
    }
    else if ((x <= r.left) && (y >= r.bottom))
    {
      return HTBOTTOMLEFT;
    }
    else if (x <= r.left)
    {
      return HTLEFT;
    }
    else if ((x >= r.right) && (y <= r.top))
    {
      return HTTOPRIGHT;
    }
    else if ((x >= r.right) && (y >= r.bottom))
    {
      return HTBOTTOMRIGHT;
    }
    else if (x >= r.right)
    {
      return HTRIGHT;
    }
    else if (y <= r.top)
    {
      return HTTOP;
    }
    else if (y >= r.bottom)
    {
      return HTBOTTOM;
    }
  }

  return HTCLIENT;
}

//----  --------------------------------------------------------------------------------------------------------
// Function:    EAEUpdateGUI
// Requires:    HWND hwnd - window handle
//              bool shadow - window shadow indicator
//              std::wstring zposition - z-order setting for window
// Returns:     bool
// Purpose:     Set the window shadow and z-order
//----  --------------------------------------------------------------------------------------------------------
HWND EAEUpdateGUI(HWND hwnd, bool shadow, std::wstring zposition)
{
  if (shadow)
  {
    SetClassLongPtr(hwnd, GCL_STYLE, GetClassLongPtr(hwnd, GCL_STYLE) | CS_DROPSHADOW);
  }
  else
  {
    SetClassLongPtr(hwnd, GCL_STYLE, GetClassLongPtr(hwnd, GCL_STYLE) & ~CS_DROPSHADOW);
  }

  // Set window z-order
  if (ELToLower(zposition) == ELToLower(TEXT("Top")))
  {
    return HWND_TOPMOST;
  }

  return HWND_NOTOPMOST;
}

HWND EAEInitializeAppletWindow(HINSTANCE appletInstance, WNDPROC windowProcedure, LPVOID lpParam)
{
  WNDCLASSEX wcx;
  HWND appletWindow;

  ZeroMemory(&wcx, sizeof(WNDCLASSEX));

  // Register the window class
  wcx.hInstance = appletInstance;
  wcx.lpszClassName = appletClass;
  wcx.lpfnWndProc = windowProcedure;
  wcx.cbSize = sizeof (WNDCLASSEX);
  wcx.style = CS_DBLCLKS;
  wcx.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  wcx.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
  wcx.hCursor = LoadCursor (NULL, IDC_ARROW);

  // Register the window class, and if it fails quit the program
  if (!RegisterClassEx (&wcx))
  {
    return NULL;
  }

  // The class is registered, let's create the window
  appletWindow = CreateWindowEx (
                   WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST,
                   appletClass,
                   NULL,
                   WS_POPUP,
                   0, 0,
                   0, 0,
                   NULL,
                   NULL,
                   appletInstance,
                   lpParam
                 );

  return appletWindow;
}

LRESULT EAEDisplayChange(HMONITOR appletMonitor, RECT* appletRect, RECT* oldDeskRect)
{
  RECT deskRect;
  int width, height;
  int middle, centre;
  MONITORINFO appletMonitorInfo;
  appletMonitorInfo.cbSize = sizeof(MONITORINFO);
  //  float xdelta, ydelta;

  if (!GetMonitorInfo(appletMonitor, &appletMonitorInfo))
  {
    ELGetDesktopRect(appletRect, &deskRect);
  }
  else
  {
    CopyRect(&deskRect, &appletMonitorInfo.rcMonitor);
  }

  width = appletRect->right - appletRect->left;
  height = appletRect->bottom - appletRect->top;
  middle = appletRect->left + (width / 2);
  centre = appletRect->top + (height / 2);

  if (!EqualRect(&deskRect, oldDeskRect))
  {
    int newX, newY;
    /*      xdelta = (float)middle / (float)(oldDeskRect->right - oldDeskRect->left);
            newX = xdelta * 100000;
            xdelta = (float)newX / 100000;
            newX = ((deskRect.right - deskRect.left) * xdelta) - (width / 2);

            ydelta = (float)centre / (float)(oldDeskRect->bottom - oldDeskRect->top);
            newY = ydelta * 100000;
            ydelta = (float)newY / 100000;
            newY = ((deskRect.bottom - deskRect.top) * ydelta) - (height / 2);*/

    // Horizontal Positioning
    int leftDelta = appletRect->left - oldDeskRect->left;
    int rightDelta = oldDeskRect->right - appletRect->right;
    int midDelta = ELMid(oldDeskRect->right, oldDeskRect->left) - middle;

    if ((abs(midDelta) < abs(leftDelta)) && (abs(midDelta) < abs(rightDelta)))
    {
      newX = ELMid(deskRect.right, deskRect.left) - midDelta - (width / 2);
    }
    else if (abs(rightDelta) < abs(leftDelta))
    {
      newX = deskRect.right - rightDelta - width;
    }
    else
    {
      newX = deskRect.left + leftDelta;
    }

    // Vertical Positioning
    int topDelta = appletRect->top - oldDeskRect->top;
    int bottomDelta = oldDeskRect->bottom - appletRect->bottom;
    int centreDelta = ELMid(oldDeskRect->bottom, oldDeskRect->top) - centre;

    if ((abs(centreDelta) < abs(topDelta)) && (abs(centreDelta) < abs(bottomDelta)))
    {
      newY = ELMid(deskRect.bottom, deskRect.top) - centreDelta - (height / 2);
    }
    else if (abs(bottomDelta) < abs(topDelta))
    {
      newY = deskRect.bottom - bottomDelta - height;
    }
    else
    {
      newY = deskRect.top + topDelta;
    }

    appletRect->left = newX;
    appletRect->right = appletRect->left + width;
    appletRect->top = newY;
    appletRect->bottom = appletRect->top + height;
  }

  return 0;
}

bool EAEAutoSize(AUTOSIZEINFO autoSizeInfo)
{
  UINT width, height, series = 1;
  RECT newRect;
  UINT divider = autoSizeInfo.visibleIconCount;

  if (autoSizeInfo.limit > 0)
  {
    series = autoSizeInfo.visibleIconCount / autoSizeInfo.limit;
    if (autoSizeInfo.visibleIconCount % autoSizeInfo.limit > 0)
    {
      series++;
    }
    divider = MIN(autoSizeInfo.visibleIconCount, autoSizeInfo.limit);
  }

  if (autoSizeInfo.orientation == ASI_VERTICAL)
  {
    height = (2 * autoSizeInfo.dragBorder) +
             (divider * autoSizeInfo.iconSpacing) +
             (divider * autoSizeInfo.iconSize);
    if (divider > 0)
    {
      height -= autoSizeInfo.iconSpacing;
    }

    width = (2 * autoSizeInfo.dragBorder) +
            (series * autoSizeInfo.iconSpacing) +
            (series * autoSizeInfo.iconSize);
    if (series > 0)
    {
      width -= autoSizeInfo.iconSpacing;
    }
  }
  else
  {
    width = (2 * autoSizeInfo.dragBorder) +
            (divider * autoSizeInfo.iconSpacing) +
            (divider * autoSizeInfo.iconSize);
    if (divider > 0)
    {
      width -= autoSizeInfo.iconSpacing;
    }

    height = (2 * autoSizeInfo.dragBorder) +
             (series * autoSizeInfo.iconSpacing) +
             (series * autoSizeInfo.iconSize);
    if (series > 0)
    {
      height -= autoSizeInfo.iconSpacing;
    }
  }

  if (autoSizeInfo.verticalDirection == ASI_UP)
  {
    newRect.top = autoSizeInfo.rect->bottom;
    newRect.top -= height;
  }
  else if (autoSizeInfo.verticalDirection == ASI_CENTER)
  {
    newRect.top = autoSizeInfo.rect->top + ((autoSizeInfo.rect->bottom - autoSizeInfo.rect->top) / 2);
    newRect.top -= height / 2;
  }
  else
  {
    newRect.top = autoSizeInfo.rect->top;
  }


  if (autoSizeInfo.horizontalDirection == ASI_LEFT)
  {
    newRect.left = autoSizeInfo.rect->right;
    newRect.left -= width;
  }
  else if (autoSizeInfo.horizontalDirection == ASI_MIDDLE)
  {
    newRect.left = autoSizeInfo.rect->left + ((autoSizeInfo.rect->right - autoSizeInfo.rect->left) / 2);
    newRect.left -= width / 2;
  }
  else
  {
    newRect.left = autoSizeInfo.rect->left;
  }

  newRect.right = newRect.left + width;
  newRect.bottom = newRect.top + height;

  if (!IsRectEmpty(&autoSizeInfo.titleBarRect))
  {
    newRect.bottom = newRect.bottom + (autoSizeInfo.titleBarRect.bottom - autoSizeInfo.titleBarRect.top);

    if ((autoSizeInfo.titleBarRect.right - autoSizeInfo.titleBarRect.left) > (newRect.right - newRect.left))
    {
      newRect.right = newRect.left + (autoSizeInfo.titleBarRect.right - autoSizeInfo.titleBarRect.left);
    }
  }

  CopyRect(autoSizeInfo.rect, &newRect);

  return true;
}

HWND EAEInitializeTooltipWindow(HINSTANCE appletInstance)
{
  HWND tooltipWindow = NULL;

  // Initialize common controls for the tooltip
  INITCOMMONCONTROLSEX ic;
  ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
  ic.dwICC = ICC_BAR_CLASSES;

  if (!InitCommonControlsEx(&ic))
  {
    return NULL;
  }

  // Create the tooltip window
  tooltipWindow = CreateWindowEx(
                    0,
                    TOOLTIPS_CLASS,
                    NULL,
                    TTS_ALWAYSTIP | WS_POPUP | TTS_NOPREFIX,
                    CW_USEDEFAULT, CW_USEDEFAULT,
                    CW_USEDEFAULT, CW_USEDEFAULT,
                    NULL,
                    NULL,
                    appletInstance,
                    NULL
                  );

  // If the tooltip window was created successfully, extend it for 2K/XP icon tips
  if (tooltipWindow)
  {
    SetWindowPos(tooltipWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SendMessage(tooltipWindow, TTM_SETMAXTIPWIDTH, 0, TIP_SIZE);
  }

  return tooltipWindow;
}

//----  --------------------------------------------------------------------------------------------------------
// Function:	EAEDisplayMenu
// Requires:	HWND callingWnd - Handle of calling Window
// Returns:	Nothing
// Purpose:	Builds task menu
//----  --------------------------------------------------------------------------------------------------------
int EAEDisplayMenu(HWND callingWnd, HWND taskWnd)
{
  // Get cursor position
  POINT pt;
  GetCursorPos(&pt);
  WINDOWPLACEMENT taskWndPlacement;
  HMENU systemMenu;
  int command = 0;

  if (ELIsModal(taskWnd))
  {
    return command;
  }

  // Create menu
  systemMenu = GetSystemMenu(taskWnd, false);

  taskWndPlacement.length = sizeof(WINDOWPLACEMENT);
  GetWindowPlacement(taskWnd, &taskWndPlacement);

  if (GetClassLongPtr(taskWnd, GCL_STYLE) & CS_NOCLOSE)
  {
    DeleteMenu(systemMenu, SC_CLOSE, MF_BYCOMMAND);
  }

  // restore is enabled only if minimized or maximized (not normal)
  EnableMenuItem(systemMenu, SC_RESTORE, MF_BYCOMMAND |
                 (taskWndPlacement.showCmd != SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));

  // move is enabled only if normal
  EnableMenuItem(systemMenu, SC_MOVE, MF_BYCOMMAND |
                 (taskWndPlacement.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));

  // size is enabled only if normal
  EnableMenuItem(systemMenu, SC_SIZE, MF_BYCOMMAND |
                 (taskWndPlacement.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));

  // minimize is enabled only if not minimized
  EnableMenuItem(systemMenu, SC_MINIMIZE, MF_BYCOMMAND |
                 (taskWndPlacement.showCmd != SW_SHOWMINIMIZED ? MF_ENABLED : MF_GRAYED));

  // maximize is enabled only if not maximized
  EnableMenuItem(systemMenu, SC_MAXIMIZE, MF_BYCOMMAND |
                 (taskWndPlacement.showCmd != SW_SHOWMAXIMIZED ? MF_ENABLED : MF_GRAYED));

  // let application modify menu
  SendMessage(taskWnd, WM_INITMENUPOPUP, (WPARAM)systemMenu, MAKELPARAM(0, TRUE));
  SendMessage(taskWnd, WM_INITMENU, (WPARAM)systemMenu, 0);

  // Display menu
  command = TrackPopupMenuEx(systemMenu,
                             TPM_RETURNCMD | TPM_RECURSE,
                             pt.x, pt.y,
                             callingWnd,
                             NULL);

  // Reset the menu - disabled, seems to conflict with some apps like Servant Salamander
  //  GetSystemMenu(taskWnd, true);

  return command;
}

HRESULT AddContextMenu(WCHAR* file)
{
  HRESULT hr = E_UNEXPECTED;
  LPVOID lpVoid;
  HMENU fileMenu = CreatePopupMenu();
  IShellFolder* deskFolder = NULL;
  IShellFolder* appObject = NULL;
  LPITEMIDLIST pidlLocal = NULL;
  LPITEMIDLIST pidlRelative = NULL;
  IContextMenu* contextMenu = NULL;
  CONTEXTINFO contextInfo;

  wcscpy(contextInfo.value, file);

  hr = SHGetDesktopFolder(&deskFolder);
  if (FAILED(hr))
  {
    return hr;
  }

  hr = deskFolder->ParseDisplayName(NULL, NULL, file, NULL, &pidlLocal, NULL);
  if (FAILED(hr))
  {
    deskFolder->Release();
    return hr;
  }

  pidlRelative = ILClone(ILFindLastID(pidlLocal));
  ILRemoveLastID(pidlLocal);

  hr = deskFolder->BindToObject(pidlLocal, NULL, IID_IShellFolder, &lpVoid);
  if (FAILED(hr))
  {
    deskFolder->Release();
    ILFree(pidlLocal);
    return hr;
  }
  appObject = reinterpret_cast <IShellFolder*> (lpVoid);

  deskFolder->Release();
  ILFree(pidlLocal);

  hr = appObject->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&pidlRelative,
                                IID_IContextMenu, NULL, &lpVoid);
  if (FAILED(hr))
  {
    appObject->Release();
    ILFree(pidlRelative);
    return hr;
  }
  contextMenu = reinterpret_cast <IContextMenu*> (lpVoid);

  ILFree(pidlRelative);
  appObject->Release();

  contextMenu->QueryInterface(IID_IContextMenu2, &lpVoid);
  if (FAILED(hr))
  {
    contextMenu->Release();
    return hr;
  }
  contextInfo.ic2 = reinterpret_cast <IContextMenu2*> (lpVoid);

  contextMenu->Release();

  hr = contextInfo.ic2->QueryContextMenu(fileMenu, 0, SYSMENUMIN, SYSMENUMAX, CMF_NORMAL);
  if (FAILED(hr))
  {
    contextInfo.ic2->Release();
    return hr;
  }

  if (!ELIsDirectory(file))
  {
    AppendMenu(fileMenu, MF_SEPARATOR, 0x8000, NULL);
    AppendMenu(fileMenu, MF_STRING, 0x8001, TEXT("Open Folder"));
  }

  contextMap.insert(std::pair<HMENU, CONTEXTINFO>(fileMenu, contextInfo));

  return hr;
}

int EAEDisplayFileMenu(WCHAR* file, HWND callingWnd)
{
  int command = 0;
  POINT pt;
  GetCursorPos(&pt);
  WCHAR expandedFile[MAX_PATH];
  WCHAR directory[MAX_PATH];
  ContextMap::iterator iter;
  CMINVOKECOMMANDINFO commandInfo;
  WCHAR tmp[MAX_PATH];
  WCHAR delimiter = '|';
  UINT i = 0, j = 0;
  HMENU contextMenu;
  MENUITEMINFO itemInfo;

  ZeroMemory(&commandInfo, sizeof(commandInfo));

  while (i < wcslen(file))
  {
    if (file[i] == delimiter)
    {
      tmp[j] = '\0';
      ExpandEnvironmentStrings((LPCTSTR)tmp, (LPTSTR)expandedFile, MAX_PATH);
      AddContextMenu(expandedFile);

      j = 0;
    }
    else
    {
      tmp[j] = file[i];
      j++;
    }

    i++;
  }

  tmp[j] = '\0';
  ExpandEnvironmentStrings((LPCTSTR)tmp, (LPTSTR)expandedFile, MAX_PATH);
  AddContextMenu(expandedFile);

  if (contextMap.size() == 1)
  {
    contextMenu = contextMap.begin()->first;
  }
  else
  {
    contextMenu = CreatePopupMenu();
    ZeroMemory(&itemInfo, sizeof(itemInfo));
    itemInfo.fMask = MIIM_STRING | MIIM_SUBMENU;
    itemInfo.cbSize = sizeof(MENUITEMINFO);
    i = 0;

    iter = contextMap.begin();
    while (iter != contextMap.end())
    {
      itemInfo.dwTypeData = iter->second.value;
      itemInfo.hSubMenu = iter->first;
      InsertMenuItem(contextMenu, i, TRUE, &itemInfo);
      i++;
      iter++;
    }
  }

  // Subclass wndproc to handle messages
  oldWndProc = (WNDPROC)SetWindowLongPtr(callingWnd, GWLP_WNDPROC, (LONG_PTR)MenuProc);
  // Display menu
  command = TrackPopupMenuEx(contextMenu,
                             TPM_RETURNCMD | TPM_RECURSE,
                             pt.x, pt.y,
                             callingWnd,
                             NULL);
  if (oldWndProc)
  {
    SetWindowLongPtr(callingWnd, GWLP_WNDPROC, (LONG_PTR)oldWndProc);
    oldWndProc = NULL;
  }

  if ((command >= SYSMENUMIN) && (command <= SYSMENUMAX))
  {
    iter = contextMap.find(selectedMenu);
    if (iter == contextMap.end())
    {
      return 0;
    }

    int offset = command - SYSMENUMIN;
    commandInfo.cbSize = sizeof(commandInfo);
    commandInfo.hwnd = callingWnd;
    commandInfo.lpVerb = (LPCSTR)MAKEINTRESOURCE(offset);
    commandInfo.nShow = SW_SHOW;

    iter->second.ic2->InvokeCommand(&commandInfo);
  }
  else if (command == 0x8001)
  {
    wcscpy(directory, file);
    PathRemoveFileSpec(directory);
    ELExecuteFileOrCommand(directory, directory);
  }

  while (!contextMap.empty())
  {
    iter = contextMap.begin();
    DestroyMenu(iter->first);
    iter->second.ic2->Release();
    contextMap.erase(iter);
  }
  DestroyMenu(contextMenu);

  return command;
}

LRESULT CALLBACK MenuProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  ContextMap::iterator iter;

  switch (message)
  {
  case WM_MENUSELECT:
    {
      HMENU menu = (HMENU)lParam;
      if (menu)
      {
        iter = contextMap.find(menu);
        if (iter != contextMap.end())
        {
          selectedMenu = (HMENU)lParam;
          return 0;
        }
        else
        {
          return CallWindowProc(oldWndProc, hwnd, message, wParam, lParam);
        }
      }
    }
    break;
  case WM_DRAWITEM:
  case WM_MEASUREITEM:
    if (wParam)
    {
      break;
    }
  case WM_INITMENUPOPUP:
    iter = contextMap.find(selectedMenu);
    if (iter == contextMap.end())
    {
      return 0;
    }
    iter->second.ic2->HandleMenuMsg(message, wParam, lParam);
    return (message == WM_INITMENUPOPUP ? 0 : TRUE);
  default:
    break;
  }

  // for all untreated messages, call the original wndproc
  return DefWindowProc(hwnd, message, wParam, lParam);
}
