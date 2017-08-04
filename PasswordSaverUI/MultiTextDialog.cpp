#include "wx_pch.h"
#include "MultiTextDialog.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(MultiTextDialog)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(MultiTextDialog)
#include <wx/button.h>
//*)

//(*IdInit(MultiTextDialog)
const long MultiTextDialog::ID_STATICTEXT1 = wxNewId();
const long MultiTextDialog::ID_TEXTCTRL1 = wxNewId();
const long MultiTextDialog::ID_STATICTEXT2 = wxNewId();
const long MultiTextDialog::ID_TEXTCTRL2 = wxNewId();
const long MultiTextDialog::ID_STATICTEXT3 = wxNewId();
const long MultiTextDialog::ID_TEXTCTRL3 = wxNewId();
const long MultiTextDialog::ID_STATICTEXT4 = wxNewId();
const long MultiTextDialog::ID_TEXTCTRL4 = wxNewId();
const long MultiTextDialog::ID_STATICTEXT5 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MultiTextDialog,wxDialog)
	//(*EventTable(MultiTextDialog)
	//*)
END_EVENT_TABLE()

MultiTextDialog::MultiTextDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(MultiTextDialog)
	wxFlexGridSizer* FlexGridSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, id, _("New Entry"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer1->AddGrowableCol(1);
	FlexGridSizer1->AddGrowableRow(4);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Website"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(268,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer1->Add(TextCtrl1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("URL"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl2 = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxSize(268,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer1->Add(TextCtrl2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Username"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl3 = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxSize(268,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	FlexGridSizer1->Add(TextCtrl3, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Password"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl4 = new wxTextCtrl(this, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxSize(268,33), 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	FlexGridSizer1->Add(TextCtrl4, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	FlexGridSizer1->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
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

std::string MultiTextDialog::getTextBox(int boxNum){
    wxString content;
    switch(boxNum){
    case 1:
        content =  TextCtrl1->GetLineText(0);
        break;
    case 2:
        content = TextCtrl2->GetLineText(0);
        break;
    case 3:
        content = TextCtrl3->GetLineText(0);
        break;
    case 4:
        content = TextCtrl4->GetLineText(0);
        break;
    default:
        content = wxT("Null");
    }
    std::string str = std::string(content.mb_str());
    return str;
}
