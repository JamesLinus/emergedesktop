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

#include "Config.h"
#include "Applet.h"

INT_PTR CALLBACK Config::ConfigDlgProc(HWND hwndDlg, UINT message, WPARAM wParam UNUSED, LPARAM lParam UNUSED)
{
  static Config* pConfig = NULL;

  switch (message)
  {
  case WM_INITDIALOG:
    pConfig = reinterpret_cast<Config*>(lParam);
    if (!pConfig)
    {
      break;
    }
    return pConfig->DoInitDialog(hwndDlg);
  }

  return 0;
}

Config::Config(HINSTANCE hInstance, HWND mainWnd, WCHAR* instanceName, std::tr1::shared_ptr<Settings> pSettings)
{
  this->hInstance = hInstance;
  this->mainWnd = mainWnd;
  this->pSettings = pSettings;

  pConfigPage = std::tr1::shared_ptr<ConfigPage>(new ConfigPage(pSettings));
  pIconHidePage = std::tr1::shared_ptr<IconHidePage>(new IconHidePage(hInstance, pSettings));
  pPositionPage = std::tr1::shared_ptr<BasePositionPage>
                  (new BasePositionPage(pSettings, BPP_ZORDER | BPP_HORIZONTAL | BPP_ORIENTATION | BPP_VERTICAL));
  pStyleEditor = std::tr1::shared_ptr<StyleEditor>(new StyleEditor(mainWnd, instanceName));
  pBalloonPage = std::tr1::shared_ptr<BalloonPage>(new BalloonPage(pSettings));
}

Config::~Config()
{
}

int Config::Show()
{
  return (int)DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CONFIG), mainWnd, (DLGPROC)ConfigDlgProc, (LPARAM)this);
}

INT_PTR Config::DoInitDialog(HWND hwndDlg)
{
  int ret;
  PROPSHEETPAGE psp[5];
  PROPSHEETHEADER psh;

  ELStealFocus(hwndDlg);
  SetWindowPos(hwndDlg, HWND_TOPMOST, 0, 0, 0, 0,  SWP_NOSIZE | SWP_NOMOVE);

  psp[0].dwSize = sizeof(PROPSHEETPAGE);
  psp[0].dwFlags = PSP_USETITLE;
  psp[0].hInstance = hInstance;
  psp[0].pszTemplate = MAKEINTRESOURCE(IDD_CONFIG_PAGE);
  psp[0].pfnDlgProc = pConfigPage->ConfigPageDlgProc;
  psp[0].pszTitle = TEXT("General");
  psp[0].lParam = reinterpret_cast<LPARAM>(pConfigPage.get());
  psp[0].pfnCallback = NULL;

  psp[1].dwSize = sizeof(PROPSHEETPAGE);
  psp[1].dwFlags = PSP_USETITLE;
  psp[1].hInstance = hInstance;
  psp[1].pszTemplate = MAKEINTRESOURCE(IDD_ICONHIDE_PAGE);
  psp[1].pfnDlgProc = pIconHidePage->IconHidePageDlgProc;
  psp[1].pszTitle = TEXT("Hidden Icons");
  psp[1].lParam = reinterpret_cast<LPARAM>(pIconHidePage.get());
  psp[1].pfnCallback = NULL;

  psp[2].dwSize = sizeof(PROPSHEETPAGE);
  psp[2].dwFlags = PSP_USETITLE;
  psp[2].hInstance = hInstance;
  psp[2].pszTemplate = MAKEINTRESOURCE(IDD_BALLOON_PAGE);
  psp[2].pfnDlgProc = pBalloonPage->BalloonPageDlgProc;
  psp[2].pszTitle = TEXT("Balloon tips");
  psp[2].lParam = reinterpret_cast<LPARAM>(pBalloonPage.get());
  psp[2].pfnCallback = NULL;

  psp[3].dwSize = sizeof(PROPSHEETPAGE);
  psp[3].dwFlags = PSP_USETITLE;
  psp[3].hInstance = ELGetEmergeLibrary(TEXT("emergeBaseClasses.dll"));
  psp[3].pszTemplate = pPositionPage->GetTemplate();
  psp[3].pfnDlgProc = pPositionPage->BasePositionPageDlgProc;
  psp[3].pszTitle = TEXT("Position");
  psp[3].lParam = reinterpret_cast<LPARAM>(pPositionPage.get());
  psp[3].pfnCallback = NULL;

  pStyleEditor->Edit(pSettings->GetStyleFile());
  psp[4].dwSize = sizeof(PROPSHEETPAGE);
  psp[4].dwFlags = PSP_USETITLE;
  psp[4].hInstance = ELGetEmergeLibrary(TEXT("emergeStyleEngine.dll"));
  psp[4].pszTemplate = pStyleEditor->GetTemplate();
  psp[4].pfnDlgProc = pStyleEditor->StyleEditorDlgProc;
  psp[4].pszTitle = TEXT("Style Editor");
  psp[4].lParam = reinterpret_cast<LPARAM>(pStyleEditor.get());
  psp[4].pfnCallback = NULL;

  psh.dwSize = sizeof(PROPSHEETHEADER);
  psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
  psh.hwndParent = hwndDlg;
  psh.hInstance = hInstance;
  psh.pszCaption = TEXT("emergeTray Configuration");
  psh.nPages = sizeof(psp) /
               sizeof(PROPSHEETPAGE);
  psh.nStartPage = 0;
  psh.ppsp = (LPCPROPSHEETPAGE) &psp;
  psh.pfnCallback = NULL;

  ret = PropertySheet(&psh);

  if (ret >= 1)
  {
    pSettings->SetStyleFile(ESEGetStyle());
    pSettings->WriteSettings();
    EndDialog(hwndDlg, IDOK);
  }
  if (ret <= 0)
  {
    EndDialog(hwndDlg, IDCANCEL);
  }

  return 1;
}

