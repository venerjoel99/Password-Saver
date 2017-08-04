#ifndef MULTITEXTDIALOG_H
#define MULTITEXTDIALOG_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(MultiTextDialog)
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	#include <wx/dialog.h>
	//*)
#endif
//(*Headers(MultiTextDialog)
//*)

class MultiTextDialog: public wxDialog
{
	public:

		MultiTextDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~MultiTextDialog();
        std::string getTextBox(int boxNum);
		//(*Declarations(MultiTextDialog)
		wxTextCtrl* TextCtrl4;
		wxStaticText* StaticText2;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxStaticText* StaticText5;
		wxTextCtrl* TextCtrl2;
		wxTextCtrl* TextCtrl1;
		wxStaticText* StaticText4;
		wxTextCtrl* TextCtrl3;
		//*)

	protected:

		//(*Identifiers(MultiTextDialog)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL4;
		static const long ID_STATICTEXT5;
		//*)

	private:

		//(*Handlers(MultiTextDialog)
		void OnTextCtrl2Text(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnTextCtrl4Text(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
