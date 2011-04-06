/*
* Copyright (c) 2003-2011 Rony Shapiro <ronys@users.sourceforge.net>.
* All rights reserved. Use of the code is allowed under the
* Artistic License 2.0 terms, as specified in the LICENSE file
* distributed with this code, or available from
* http://www.opensource.org/licenses/artistic-license-2.0.php
*/
/// \file AboutDlg.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "AboutDlg.h"
// following 2 includes to allow querying
// the current version details
#include "PasswordSafe.h"
#include "ThisMfcApp.h"
#include "GeneralMsgBox.h"
#include "RichEditCtrlExtn.h"
#include "version.h"
#include "DumpSelect.h"

#include "core/UTF8Conv.h"
#include "core/SysInfo.h"

#include "resource.h"
#include "resource3.h"

// for fetching xml file:
#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAboutDlg::CAboutDlg(CWnd* pParent)
  : CPWDialog(CAboutDlg::IDD, pParent), 
  m_nMajor(0), m_nMinor(0), m_nBuild(0)
{
  CString verstat;
  // Following since text in quotes is not to be translated
  verstat.Format(IDS_LATEST_VERSION, L"[check_version]");
  m_newVerStatus = verstat;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CPWDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_NEWVER_STATUS, m_RECExNewVerStatus);
  DDX_Control(pDX, IDC_VISIT_WEBSITE, m_RECExWebSite);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CPWDialog)
  ON_BN_CLICKED(IDC_TAKETESTDUMP, OnTakeTestdump)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
  CPWDialog::OnInitDialog();

  DWORD dwMajorMinor = app.GetFileVersionMajorMinor();
  DWORD dwBuildRevision = app.GetFileVersionBuildRevision();

  if (dwMajorMinor > 0) {
    m_nMajor = HIWORD(dwMajorMinor);
    m_nMinor = LOWORD(dwMajorMinor);
    m_nBuild = HIWORD(dwBuildRevision);
  }

  // revision is either a number or a number with '+',
  // so we need to get it from the file version string
  // which is of the form "MM, NN, BB, rev"
  CString csFileVersionString, csRevision;
  csFileVersionString = app.GetFileVersionString();
  int revIndex = csFileVersionString.ReverseFind(L',');
  if (revIndex >= 0) {
    int len = csFileVersionString.GetLength();
    csRevision = csFileVersionString.Right(len - revIndex - 1);
    csRevision.Trim();
  }
  const CString cs2go = SysInfo::IsUnderPw2go() ? L"2go " : L" ";
  if (m_nBuild == 0) { // hide build # if zero (formal release)
    m_appversion.Format(L"%s%sV%d.%02d (%s)%s", AfxGetAppName(), cs2go,
                        m_nMajor, m_nMinor, csRevision, SPECIAL_BUILD);
  } else {
    m_appversion.Format(L"%s%sV%d.%02d.%02d (%s)%s", AfxGetAppName(), cs2go,
                        m_nMajor, m_nMinor, m_nBuild, csRevision, SPECIAL_BUILD);
  }
#ifdef _DEBUG
  m_appversion += L" [Debug]";
#endif
  m_appcopyright = app.GetCopyrightString();

#ifdef DEMO
  m_appversion += L" " + CString(MAKEINTRESOURCE(IDS_DEMO));
#endif

  GetDlgItem(IDC_APPVERSION)->SetWindowText(m_appversion);
  GetDlgItem(IDC_APPCOPYRIGHT)->SetWindowText(m_appcopyright);

  m_RECExNewVerStatus.SetWindowText(m_newVerStatus);
  m_RECExNewVerStatus.RegisterOnLink(OnCheckVersion, (LPARAM)this);

  m_RECExWebSite.SetWindowText(CString(MAKEINTRESOURCE(IDS_VISIT_WEBSITE)));

  if (app.PermitTestdump()) {
    GetDlgItem(IDC_TAKETESTDUMP)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_TAKETESTDUMP)->EnableWindow();
  }

  return TRUE;
}

bool CAboutDlg::OnCheckVersion(const CString &URL, const CString & /* lpszFName */, LPARAM instance)
{
  if (URL == L"[check_version]") {
    CAboutDlg *self = (CAboutDlg *)instance;
    self->CheckNewVer();
    return true;
  } else
    return false;
}

void CAboutDlg::CheckNewVer()
{
  // Get the latest.xml file from our site, compare to version,
  // and notify the user
  // First, make sure database is closed: Sensitive data with an
  // open socket makes me uneasy...
  DboxMain *pDbx = static_cast<DboxMain *>(GetParent());
  CGeneralMsgBox gmb;

  if (pDbx->GetNumEntries() != 0) {
    const CString cs_txt(MAKEINTRESOURCE(IDS_CLOSE_B4_CHECK));
    const CString cs_title(MAKEINTRESOURCE(IDS_CONFIRM_CLOSE));
    INT_PTR rc = gmb.MessageBox(cs_txt, cs_title,
                        (MB_OKCANCEL | MB_ICONQUESTION));
    if (rc == IDCANCEL)
      return; // no hard feelings
    // Close database, prompt for save if changed
    pDbx->SendMessage(WM_COMMAND, ID_MENUITEM_CLOSE);
    // User could have cancelled save, need to check if really closed:
    if (pDbx->GetNumEntries() != 0)
      return;
  }
  pDbx->UpdateWindow(); // show user that we closed database
  ASSERT(pDbx->GetNumEntries() == 0);
  // safe to open external connection
  m_newVerStatus.LoadString(IDS_TRYING2CONTACT_SERVER);
  UpdateData(FALSE);
  std::wstring latest;
  wchar_t *html_redfont = L"<b><font color=\"red\">";
  wchar_t *html_greenfont = L"<b><font color=\"green\">";
  wchar_t *html_endfont = L"</font></b>";
  switch (CheckLatestVersion(latest)) {
    case CheckVersion::CANT_CONNECT:
      m_newVerStatus.Format(IDS_CANT_CONTACT_SERVER, html_redfont, html_endfont);
      break;
    case CheckVersion::UP2DATE:
      m_newVerStatus.Format(IDS_UP2DATE, html_greenfont, html_endfont);
      break;
    case CheckVersion::NEWER_AVAILABLE:
      {
      CString newer;
      newer.Format(SysInfo::IsUnderU3() ? IDS_NEWER_AVAILABLE_U3 : IDS_NEWER_AVAILABLE,
                   m_appversion, latest.c_str());
      m_newVerStatus.Format(IDS_NEWER_AVAILABLE_SHORT, html_redfont, html_endfont);
      gmb.MessageBox(newer, CString(MAKEINTRESOURCE(IDS_NEWER_CAPTION)), MB_ICONEXCLAMATION);
      break;
      }
    case CheckVersion::CANT_READ:
      m_newVerStatus.Format(IDS_CANT_READ_VERINFO, html_redfont, html_endfont);
      break;
    default:
      break;
  }

  m_RECExNewVerStatus.SetFont(GetFont());
  m_RECExNewVerStatus.SetWindowText(m_newVerStatus);
  m_RECExNewVerStatus.Invalidate();
  UpdateData(FALSE);
  GetDlgItem(IDOK)->SetFocus();
}

CheckVersion::CheckStatus CAboutDlg::CheckLatestVersion(std::wstring &latest)
{
  CInternetSession session(L"PasswordSafe Version Check");
  CStdioFile *fh;
  // Put up hourglass...this might take a while
  CWaitCursor waitCursor;
  try {
    // Loading the file as binary since we're treating it as UTF-8
    fh = session.OpenURL(L"http://pwsafe.org/latest.xml",
                         1, (INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD));
  } catch (CInternetException *) {
    // throw;
    return CheckVersion::CANT_CONNECT;
  }
  ASSERT(fh != NULL);
  CString latest_xml;
  unsigned char buff[BUFSIZ + 1];
  StringX chunk;
  UINT nRead;
  CUTF8Conv conv;
  while ((nRead = fh->Read(buff, BUFSIZ)) != 0) {
    buff[nRead] = '\0';
    // change to widechar representation
    if (!conv.FromUTF8(buff, nRead, chunk)) {
      fh->Close();
      delete fh;
      session.Close();
      return CheckVersion::CANT_READ;
    } else
      latest_xml += chunk.c_str();
  }
  fh->Close();
  delete fh;
  session.Close();
  waitCursor.Restore(); // restore normal cursor
  CheckVersion vh(m_nMajor, m_nMinor, m_nBuild);
  return vh.CheckLatestVersion(LPCWSTR(latest_xml), latest);
}

// Take a test dump
void CAboutDlg::OnTakeTestdump()
{
  CDumpSelect dmpslct;
  dmpslct.DoModal();
}

