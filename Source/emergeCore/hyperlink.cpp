/*
 * Module ID: hyperlink.cpp
 * Title    : CHyperLink definition.
 *
 * Author   : Olivier Langlois <olanglois@sympatico.ca>
 * Date     : November 15, 2005
 *
 * To read the article describing this class, visit
 * http://www3.sympatico.ca/olanglois/hyperlinkdemo.htm
 *
 * Note: Strongly inspired by Neal Stublen code
 *       Minor ideas come from Chris Maunder and Paul DiLascia code
 *
 * Revision :
 *
 * 002        15-Dec-2011 - Chris Sutcliffe
 *            - Updated to play nice with GCC
 *            - switched to 64-bit capable calls
 *
 * 001        26-Nov-2005 - Olivier Langlois
 *            - Added changes to make CHyperLink compatible with UNICODE
 *            - Use dynamic memory allocation for the URL string
 *            - Use of the MAKEINTATOM macro
 */

#include "hyperlink.h"

/*
 * Defines
 */
#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

/*
 * typedefs
 */
class CGlobalAtom
{
public:
  CGlobalAtom(void)
  {
    atom = GlobalAddAtom(TEXT("_Hyperlink_Object_Pointer_")
                         TEXT("\\{AFEED740-CC6D-47c5-831D-9848FD916EEF}"));
  }
  ~CGlobalAtom(void)
  {
    DeleteAtom(atom);
  }

  ATOM atom;
};

/*
 * Local variables
 */
static CGlobalAtom ga;

/*
 * Static members initilisation
 */
COLORREF CHyperLink::g_crLinkColor     = RGB(  0,   0, 255);   // Blue;
COLORREF CHyperLink::g_crVisitedColor  = RGB( 128,  0, 128);   // Purple;
HCURSOR  CHyperLink::g_hLinkCursor     = NULL;
HFONT    CHyperLink::g_UnderlineFont   = NULL;
int      CHyperLink::g_counter         = 0;

/*
 * Macros and inline functions
 */
inline bool PTINRECT( LPCRECT r, const POINT& pt )
{
  return  ( pt.x >= r->left && pt.x < r->right && pt.y >= r->top && pt.y < r->bottom );
}
inline void INFLATERECT( PRECT r, int dx, int dy )
{
  r->left -= dx;
  r->right += dx;
  r->top -= dy;
  r->bottom += dy;
}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink

CHyperLink::CHyperLink(void)
{
  m_bOverControl      = FALSE;                // Cursor not yet over control
  m_bVisited          = FALSE;                // Hasn't been visited yet.
  m_StdFont           = NULL;
  m_pfnOrigCtlProc    = NULL;
  m_strURL            = NULL;
}

CHyperLink::~CHyperLink(void)
{
  delete [] m_strURL;
}

/*-----------------------------------------------------------------------------
 * Public functions
 */

/*
 * Function CHyperLink::ConvertStaticToHyperlink
 */
BOOL CHyperLink::ConvertStaticToHyperlink(HWND hwndCtl, LPCTSTR strURL)
{
  if( !(setURL(strURL)) )
  {
    return FALSE;
  }

  // Subclass the parent so we can color the controls as we desire.

  HWND hwndParent = GetParent(hwndCtl);
  if (NULL != hwndParent)
  {
    WNDPROC pfnOrigProc = (WNDPROC) GetWindowLongPtr(hwndParent, GWLP_WNDPROC);
    if (pfnOrigProc != _HyperlinkParentProc)
    {
      SetWindowLongPtr( hwndParent, GWLP_USERDATA, (LONG_PTR)pfnOrigProc);
      SetWindowLongPtr( hwndParent, GWLP_WNDPROC, (LONG_PTR)_HyperlinkParentProc);
    }
  }

  // Make sure the control will send notifications.
  DWORD dwStyle = GetWindowLongPtr(hwndCtl, GWL_STYLE);
  SetWindowLongPtr(hwndCtl, GWL_STYLE, dwStyle | SS_NOTIFY);

  // Create an updated font by adding an underline.
  m_StdFont = (HFONT) SendMessage(hwndCtl, WM_GETFONT, 0, 0);

  if( g_counter++ == 0 )
  {
    createGlobalResources();
  }

  // Subclass the existing control.
  m_pfnOrigCtlProc = (WNDPROC) GetWindowLongPtr(hwndCtl, GWLP_WNDPROC);
  SetWindowLongPtr(hwndCtl, GWLP_USERDATA, (LONG_PTR)this);
  SetWindowLongPtr(hwndCtl, GWLP_WNDPROC, (LONG_PTR) _HyperlinkProc);

  return TRUE;
}

/*
 * Function CHyperLink::ConvertStaticToHyperlink
 */
BOOL CHyperLink::ConvertStaticToHyperlink(HWND hwndParent, UINT uiCtlId,
    LPCTSTR strURL)
{
  return ConvertStaticToHyperlink(GetDlgItem(hwndParent, uiCtlId), strURL);
}

/*
 * Function CHyperLink::setURL
 */
BOOL CHyperLink::setURL(LPCTSTR strURL)
{
  if( m_strURL )
  {
    delete [] m_strURL;
  }
  if( !(m_strURL = new TCHAR[lstrlen(strURL) + 1]) )
  {
    return FALSE;
  }

  lstrcpy(m_strURL, strURL);

  return TRUE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Private functions
 */

/*
 * Function CHyperLink::_HyperlinkParentProc
 */
LRESULT CALLBACK CHyperLink::_HyperlinkParentProc(HWND hwnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
  WNDPROC pfnOrigProc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA);

  switch (message)
  {
  case WM_CTLCOLORSTATIC:
    {
      HDC hdc = (HDC) wParam;
      HWND hwndCtl = (HWND) lParam;
      CHyperLink* pHyperLink = reinterpret_cast< CHyperLink* >(GetWindowLongPtr(hwndCtl, GWLP_USERDATA));

      if(pHyperLink)
      {
        LRESULT lr = CallWindowProc(pfnOrigProc, hwnd, message,
                                    wParam, lParam);
        if (!pHyperLink->m_bVisited)
        {
          // This is the most common case for static branch prediction
          // optimization
          SetTextColor(hdc, CHyperLink::g_crLinkColor);
        }
        else
        {
          SetTextColor(hdc, CHyperLink::g_crVisitedColor);
        }
        return lr;
      }
      break;
    }
  case WM_DESTROY:
    {
      SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR) pfnOrigProc);
      break;
    }
  }
  return CallWindowProc(pfnOrigProc, hwnd, message, wParam, lParam);
}

/*
 * Function CHyperLink::Navigate
 */
inline void CHyperLink::Navigate(void)
{
  ELExecuteFileOrCommand(m_strURL);
  m_bVisited = TRUE;
}

/*
 * Function CHyperLink::DrawFocusRect
 */
inline void CHyperLink::DrawFocusRect(HWND hwnd)
{
  HWND hwndParent = ::GetParent(hwnd);

  if( hwndParent )
  {
    // calculate where to draw focus rectangle, in screen coords
    RECT rc;
    GetWindowRect(hwnd, &rc);

    INFLATERECT(&rc, 1, 1);					 // add one pixel all around
    // convert to parent window client coords
    ::ScreenToClient(hwndParent, (LPPOINT)&rc);
    ::ScreenToClient(hwndParent, ((LPPOINT)&rc) + 1);
    HDC dcParent = GetDC(hwndParent);		 // parent window's DC
    ::DrawFocusRect(dcParent, &rc);			 // draw it!
    ReleaseDC(hwndParent, dcParent);
  }
}

/*
 * Function CHyperLink::_HyperlinkProc
 *
 * Note: Processed messages are not passed back to the static control
 *       procedure. It does work fine but be aware that it could cause
 *       some problems if the static control is already subclassed.
 *       Consider the example where the static control would be already
 *       subclassed with the ToolTip control that needs to process mouse
 *       messages. In that situation, the ToolTip control would not work
 *       as expected.
 */
LRESULT CALLBACK CHyperLink::_HyperlinkProc(HWND hwnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
  CHyperLink* pHyperLink = reinterpret_cast< CHyperLink* >(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  switch (message)
  {
  case WM_MOUSEMOVE:
    {
      if ( pHyperLink->m_bOverControl )
      {
        // This is the most common case for static branch prediction
        // optimization
        RECT rect;
        GetClientRect(hwnd, &rect);

        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        if (!PTINRECT(&rect, pt))
        {
          ReleaseCapture();
        }
      }
      else
      {
        pHyperLink->m_bOverControl = TRUE;
        SendMessage(hwnd, WM_SETFONT,
                    (WPARAM)CHyperLink::g_UnderlineFont, FALSE);
        InvalidateRect(hwnd, NULL, FALSE);
        pHyperLink->OnSelect();
        SetCapture(hwnd);
      }
      return 0;
    }
  case WM_SETCURSOR:
    {
      SetCursor(CHyperLink::g_hLinkCursor);
      return TRUE;
    }
  case WM_CAPTURECHANGED:
    {
      pHyperLink->m_bOverControl = FALSE;
      pHyperLink->OnDeselect();
      SendMessage(hwnd, WM_SETFONT,
                  (WPARAM)pHyperLink->m_StdFont, FALSE);
      InvalidateRect(hwnd, NULL, FALSE);
      return 0;
    }
  case WM_KEYUP:
    {
      if( wParam != VK_SPACE )
      {
        break;
      }
    }
    // Fall through
  case WM_LBUTTONUP:
    {
      pHyperLink->Navigate();
      return 0;
    }
  case WM_SETFOCUS:	// Fall through
  case WM_KILLFOCUS:
    {
      if( message == WM_SETFOCUS )
      {
        pHyperLink->OnSelect();
      }
      else		// WM_KILLFOCUS
      {
        pHyperLink->OnDeselect();
      }
      CHyperLink::DrawFocusRect(hwnd);
      return 0;
    }
  case WM_DESTROY:
    {
      SetWindowLongPtr(hwnd, GWLP_WNDPROC,
                       (LONG_PTR) pHyperLink->m_pfnOrigCtlProc);

      SendMessage(hwnd, WM_SETFONT, (WPARAM) pHyperLink->m_StdFont, 0);

      if( --CHyperLink::g_counter <= 0 )
      {
        destroyGlobalResources();
      }

      break;
    }
  }

  return CallWindowProc(pHyperLink->m_pfnOrigCtlProc, hwnd, message,
                        wParam, lParam);
}

/*
 * Function CHyperLink::createUnderlineFont
 */
void CHyperLink::createUnderlineFont(void)
{
  LOGFONT lf;
  GetObject(m_StdFont, sizeof(lf), &lf);
  lf.lfUnderline = TRUE;

  g_UnderlineFont = CreateFontIndirect(&lf);
}

/*
 * Function CHyperLink::createLinkCursor
 */
void CHyperLink::createLinkCursor(void)
{
  g_hLinkCursor = ::LoadCursor(NULL, IDC_HAND); // Load Windows' hand cursor
  if( !g_hLinkCursor )    // if not available, use the standard Arrow cursor
  {
    /*
     * There exist an alternative way to get the IDC_HAND by loading winhlp32.exe but I
     * estimated that it didn't worth the trouble as IDC_HAND is supported since Win98.
     * I consider that if a user is happy with 10 years old OS, he won't bother to have
     * an arrow cursor.
     */
    g_hLinkCursor = ::LoadCursor(NULL, IDC_ARROW);
  }
}
