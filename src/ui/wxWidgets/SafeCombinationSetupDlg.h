/*
 * Copyright (c) 2003-2024 Rony Shapiro <ronys@pwsafe.org>.
 * All rights reserved. Use of the code is allowed under the
 * Artistic License 2.0 terms, as specified in the LICENSE file
 * distributed with this code, or available from
 * http://www.opensource.org/licenses/artistic-license-2.0.php
 */

/** \file SafeCombinationSetupDlg.h
 * 
 */

// Generated by DialogBlocks, Wed 21 Jan 2009 09:07:57 PM IST

#ifndef _SAFECOMBINATIONSETUPDLG_H_
#define _SAFECOMBINATIONSETUPDLG_H_

/*!
 * Includes
 */

////@begin includes
#include "wx/valgen.h"
////@end includes

#ifndef NO_YUBI
#include "YubiMixin.h"
#endif

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations
class wxTimer;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_SAFECOMBINATIONSETUPDLG 10007
#define ID_PASSWORD 10008
#define ID_VERIFY 10009
#if WXWIN_COMPATIBILITY_2_6
#define SYMBOL_SAFECOMBINATIONSETUPDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL|wxTAB_TRAVERSAL
#else
#define SYMBOL_SAFECOMBINATIONSETUPDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#endif
#define ID_YUBIBTN 10229
#define ID_YUBISTATUS 10230
#define SYMBOL_SAFECOMBINATIONSETUPDLG_TITLE _("Safe Combination Setup")
#define SYMBOL_SAFECOMBINATIONSETUPDLG_IDNAME ID_SAFECOMBINATIONSETUPDLG
#define SYMBOL_SAFECOMBINATIONSETUPDLG_SIZE wxSize(400, 300)
#define SYMBOL_SAFECOMBINATIONSETUPDLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * SafeCombinationSetupDlg class declaration
 */

#ifndef NO_YUBI
class SafeCombinationSetupDlg : public wxDialog, private YubiMixin
#else
class SafeCombinationSetupDlg : public wxDialog
#endif
{
  DECLARE_DYNAMIC_CLASS( SafeCombinationSetupDlg )
  DECLARE_EVENT_TABLE()

public:
  static SafeCombinationSetupDlg* Create(wxWindow *parent, wxWindowID id = SYMBOL_SAFECOMBINATIONSETUPDLG_IDNAME, const wxString& caption = SYMBOL_SAFECOMBINATIONSETUPDLG_TITLE, const wxPoint& pos = SYMBOL_SAFECOMBINATIONSETUPDLG_POSITION, const wxSize& size = SYMBOL_SAFECOMBINATIONSETUPDLG_SIZE, long style = SYMBOL_SAFECOMBINATIONSETUPDLG_STYLE );

  /// Destructor
  ~SafeCombinationSetupDlg();
  wxString GetPassword() const {return m_password;}
protected:
  /// Constructors
  SafeCombinationSetupDlg(wxWindow *parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style);
  SafeCombinationSetupDlg() = default;

  /// Creates the controls and sizers
  void CreateControls();

  ////@begin SafeCombinationSetupDlg event handler declarations

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_YUBIBTN
  void OnYubibtnClick( wxCommandEvent& event );

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
  void OnOkClick( wxCommandEvent& event );

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
  void OnCancelClick( wxCommandEvent& event );

////@end SafeCombinationSetupDlg event handler declarations
  void OnPollingTimer(wxTimerEvent& timerEvent);

////@begin SafeCombinationSetupDlg member function declarations

  /// Retrieves bitmap resources
  wxBitmap GetBitmapResource( const wxString& name );

  /// Retrieves icon resources
  wxIcon GetIconResource( const wxString& name );
////@end SafeCombinationSetupDlg member function declarations

  /// Should we show tooltips?
  static bool ShowToolTips();

////@begin SafeCombinationSetupDlg member variables
#ifndef NO_YUBI
  wxBitmapButton* m_YubiBtn = nullptr;
  wxStaticText* m_yubiStatusCtrl = nullptr;
#endif
////@end SafeCombinationSetupDlg member variables
 private:
  wxString m_password;
  wxString m_verify;
};

#endif // _SAFECOMBINATIONSETUPDLG_H_
