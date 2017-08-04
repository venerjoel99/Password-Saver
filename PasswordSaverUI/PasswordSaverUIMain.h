/***************************************************************
 * Name:      PasswordSaverUIMain.h
 * Purpose:   Defines Application Frame
 * Author:    Joel Veneracion (84veneracionj@gmail.com)
 * Created:   2017-07-26
 * Copyright: Joel Veneracion (84veneracionj@gmail.com)
 * License:
 **************************************************************/

#ifndef PASSWORDSAVERUIMAIN_H
#define PASSWORDSAVERUIMAIN_H

//(*Headers(PasswordSaverUIFrame)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/hyperlink.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)
#include "Data.h"
#include "Encryptor.h"

class PasswordSaverUIFrame: public wxFrame
{
    public:

        PasswordSaverUIFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~PasswordSaverUIFrame();

    private:
        std::string dir = "";
        std::string mainFile = "";
        std::string keyDir = "";
        std::string realDir;
        std::string realFile;
        Data file;
        void addToListBox(std::string);
        void showStatusDialog(Data::Success);
        void showStatusDialog(Encryptor::Status, bool);
        void refresh();
        Encryptor::Status encrypt(std::string);
        Encryptor::Status decrypt(std::string);
        void encrypt(bool);
        bool isEncrypted();
        //(*Handlers(PasswordSaverUIFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnnewButtonClick(wxCommandEvent& event);
        void OnChangeButtonClick(wxCommandEvent& event);
        void OnNewButtonClick(wxCommandEvent& event);
        void OnSearchButtonClick(wxCommandEvent& event);
        void OnDisplayButtonClick(wxCommandEvent& event);
        void OnEncryptButtonClick(wxCommandEvent& event);
        void OnDecryptButtonClick(wxCommandEvent& event);
        void OnDeleteButtonClick(wxCommandEvent& event);
        void OnEditButtonClick(wxCommandEvent& event);
        void OnClearButtonClick(wxCommandEvent& event);
        void OnClearButtonClick1(wxCommandEvent& event);
        void OnDlgButtonClick(wxCommandEvent& event);
        //*)

        //(*Identifiers(PasswordSaverUIFrame)
        static const long ID_HYPERLINKCTRL1;
        static const long ID_STATICTEXT2;
        static const long ID_BUTTON7;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_BUTTON3;
        static const long ID_BUTTON4;
        static const long ID_BUTTON5;
        static const long ID_BUTTON6;
        static const long ID_BUTTON8;
        static const long ID_LISTBOX1;
        static const long ID_PANEL1;
        static const long idMenuEncrypt;
        static const long idMenuDecrypt;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(PasswordSaverUIFrame)
        wxPanel* mainPanel;
        wxButton* Button1;
        wxMenuItem* MenuItem4;
        wxListBox* FileBox;
        wxStaticText* FileLabel;
        wxButton* SearchButton;
        wxMenuItem* MenuItem3;
        wxButton* DisplayButton;
        wxStatusBar* StatusBar1;
        wxButton* ClearButton;
        wxButton* ChangeButton;
        wxButton* NewButton;
        wxButton* DeleteButton;
        wxButton* EditButton;
        wxHyperlinkCtrl* webLink;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // PASSWORDSAVERUIMAIN_H
