#include "wx_pch.h"
#include "MultiTextDialog.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(MultiTextDialog)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(MultiTextDialog)
//*)

//(*IdInit(MultiTextDialog)
const long MultiTextDialog::ID_TEXTCTRL1 = wxNewId();
const long MultiTextDialog::ID_TEXTCTRL2 = wxNewId();
const long MultiTextDialog::ID_TEXTCTRL3 = wxNewId();
const long MultiTextDialog::ID_STATICTEXT1 = wxNewId();
const long MultiTextDialog::ID_STATICTEXT2 = wxNewId();
const long MultiTextDialog::ID_STATICTEXT3 = wxNewId();
const long MultiTextDialog::ID_TEXTCTRL4 = wxNewId();
const long MultiTextDialog::ID_STATICTEXT4 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MultiTextDialog,wxDialog)
	//(*EventTable(MultiTextDialog)
	//*)
END_EVENT_TABLE()

MultiTextDialog::MultiTextDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(MultiTextDialog)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(506,332));
	Move(wxDefaultPosition);
	Button1 = new wxButton(this, wxID_OK, _("OK"), wxPoint(200,216), wxDefaultSize, 0, wxDefaultValidator, _T("wxID_OK"));
	Button2 = new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(328,216), wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxPoint(128,32), wxSize(328,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TextCtrl2 = new wxTextCtrl(this, ID_TEXTCTRL2, _("Text"), wxPoint(128,72), wxSize(328,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	TextCtrl3 = new wxTextCtrl(this, ID_TEXTCTRL3, _("Text"), wxPoint(128,112), wxSize(328,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Website"), wxPoint(32,40), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("URL"), wxPoint(72,80), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Username"), wxPoint(16,112), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	TextCtrl4 = new wxTextCtrl(this, ID_TEXTCTRL4, _("Text"), wxPoint(128,152), wxSize(328,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Password"), wxPoint(24,152), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&MultiTextDialog::OnTextCtrl2Text);
	//*)
}

MultiTextDialog::~MultiTextDialog()
{
	//(*Destroy(MultiTextDialog)
	//*)
}


void MultiTextDialog::OnTextCtrl2Text(wxCommandEvent& event)
{
}

void MultiTextDialog::OnButton1Click(wxCommandEvent& event)
{
}
