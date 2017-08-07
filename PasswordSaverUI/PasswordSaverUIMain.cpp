/***************************************************************
 * Name:      PasswordSaverUIMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Joel Veneracion (84veneracionj@gmail.com)
 * Created:   2017-07-26
 * Copyright: Joel Veneracion (84veneracionj@gmail.com)
 * License:
 **************************************************************/

#include "PasswordSaverUIMain.h"

#include "wx_pch.h"
#include <wx/msgdlg.h>
#include <wx/textdlg.h>

#include <fstream>
#include <string>

#include "data.h"
#include "Encryptor.h"
#include "MultiTextDialog.h"

//(*InternalHeaders(PasswordSaverUIFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(PasswordSaverUIFrame)
const long PasswordSaverUIFrame::ID_HYPERLINKCTRL1 = wxNewId();
const long PasswordSaverUIFrame::ID_STATICTEXT2 = wxNewId();
const long PasswordSaverUIFrame::ID_BUTTON7 = wxNewId();
const long PasswordSaverUIFrame::ID_BUTTON1 = wxNewId();
const long PasswordSaverUIFrame::ID_BUTTON2 = wxNewId();
const long PasswordSaverUIFrame::ID_BUTTON3 = wxNewId();
const long PasswordSaverUIFrame::ID_BUTTON4 = wxNewId();
const long PasswordSaverUIFrame::ID_BUTTON5 = wxNewId();
const long PasswordSaverUIFrame::ID_BUTTON6 = wxNewId();
const long PasswordSaverUIFrame::ID_LISTBOX1 = wxNewId();
const long PasswordSaverUIFrame::ID_PANEL1 = wxNewId();
const long PasswordSaverUIFrame::idMenuEncrypt = wxNewId();
const long PasswordSaverUIFrame::idMenuDecrypt = wxNewId();
const long PasswordSaverUIFrame::idMenuQuit = wxNewId();
const long PasswordSaverUIFrame::idMenuAbout = wxNewId();
const long PasswordSaverUIFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(PasswordSaverUIFrame,wxFrame)
    //(*EventTable(PasswordSaverUIFrame)
    //*)
END_EVENT_TABLE()

/**
 * wxSmith generated Constructor
 */
PasswordSaverUIFrame::PasswordSaverUIFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(PasswordSaverUIFrame)
    wxFlexGridSizer* mainGrid;
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;
    wxBoxSizer* ButtonSizer;

    Create(parent, id, _("Passwords"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    {
    	wxIcon FrameIcon;
    	FrameIcon.CopyFromBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_SAVE_AS")),wxART_FRAME_ICON));
    	SetIcon(FrameIcon);
    }
    mainPanel = new wxPanel(this, ID_PANEL1, wxPoint(160,368), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    mainGrid = new wxFlexGridSizer(2, 2, 0, 0);
    mainGrid->AddGrowableCol(1);
    mainGrid->AddGrowableRow(1);
    webLink = new wxHyperlinkCtrl(mainPanel, ID_HYPERLINKCTRL1, _("Link"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_CONTEXTMENU|wxHL_ALIGN_CENTRE|wxNO_BORDER, _T("ID_HYPERLINKCTRL1"));
    webLink->Hide();
    mainGrid->Add(webLink, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FileLabel = new wxStaticText(mainPanel, ID_STATICTEXT2, _("File: No file opened"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    mainGrid->Add(FileLabel, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    ButtonSizer = new wxBoxSizer(wxVERTICAL);
    ChangeButton = new wxButton(mainPanel, ID_BUTTON7, _("Open File"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
    ButtonSizer->Add(ChangeButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    NewButton = new wxButton(mainPanel, ID_BUTTON1, _("New"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    NewButton->SetToolTip(_("Enter new login info"));
    ButtonSizer->Add(NewButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SearchButton = new wxButton(mainPanel, ID_BUTTON2, _("Search"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    SearchButton->SetToolTip(_("Search for login info"));
    ButtonSizer->Add(SearchButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DisplayButton = new wxButton(mainPanel, ID_BUTTON3, _("Display"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    DisplayButton->SetToolTip(_("Display current file name and content"));
    ButtonSizer->Add(DisplayButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    EditButton = new wxButton(mainPanel, ID_BUTTON4, _("Edit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    EditButton->SetToolTip(_("Edit username or password"));
    ButtonSizer->Add(EditButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DeleteButton = new wxButton(mainPanel, ID_BUTTON5, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
    DeleteButton->SetToolTip(_("Delete login info"));
    ButtonSizer->Add(DeleteButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ClearButton = new wxButton(mainPanel, ID_BUTTON6, _("Clear"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
    ButtonSizer->Add(ClearButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    mainGrid->Add(ButtonSizer, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    FileBox = new wxListBox(mainPanel, ID_LISTBOX1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
    mainGrid->Add(FileBox, 1, wxALL|wxEXPAND, 5);
    mainPanel->SetSizer(mainGrid);
    mainGrid->Fit(mainPanel);
    mainGrid->SetSizeHints(mainPanel);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, idMenuEncrypt, _("Encrypt"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem4 = new wxMenuItem(Menu1, idMenuDecrypt, _("Decrypt"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);

    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnChangeButtonClick);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnNewButtonClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnSearchButtonClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnDisplayButtonClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnEditButtonClick);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnDeleteButtonClick);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnClearButtonClick1);
    Connect(idMenuEncrypt,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnEncryptButtonClick);
    Connect(idMenuDecrypt,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnDecryptButtonClick);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PasswordSaverUIFrame::OnAbout);
    //*)
    FileLabel->SetLabel(wxT("File: "));
}

/**
 * wxSmith generated Destructor
 */
PasswordSaverUIFrame::~PasswordSaverUIFrame()
{
    //(*Destroy(PasswordSaverUIFrame)
    //*)
}

/**
 * Shows the corresponding dialog box for each Data class generated
 * enumeration
 * @param stat - the enumeration provided by a function in the Data class
 */
void PasswordSaverUIFrame::showStatusDialog(Data::Success stat){
    wxMessageDialog* dlg;
    std::string fullStr = dir + mainFile;
    wxString fullFile(fullStr.c_str(), wxConvUTF8);
    switch(stat){
    case Data::SUCCESS:
        dlg = new wxMessageDialog(NULL, wxT("Operation complete"), wxT("Success"), wxOK);
        break;
    case Data::FILE_NOT_FOUND:
        dlg = new wxMessageDialog(NULL, wxT("File not found: ") + fullFile, wxT("Error"), wxOK);
        break;
    case Data::NO_RESULTS:
        dlg = new wxMessageDialog(NULL, wxT("No results found"), wxT("No result"), wxOK);
        break;
    default:
        dlg = new wxMessageDialog(NULL, wxT("Error: Unknown"), wxT("Error"), wxOK);
    }
    dlg->ShowModal();
    delete dlg;
}

/**
 * Shows the corresponding dialog box for each Encryptor class generated
 * enumeration
 * @param stat - the enumeration provided by a function in the Encryptor class
 * @param encrypting - Was the enumeration returned during encryption or decryption?
 */
void PasswordSaverUIFrame::showStatusDialog(Encryptor::Status stat, bool encrypting){
    wxString action = encrypting ? wxT("Encryption") : wxT("Decryption");
    wxMessageDialog* dlg;
    std::string fullStr = dir + mainFile.substr(0, mainFile.find('.')) + "/key.bin";
    wxString fullKey(fullStr.c_str(), wxConvUTF8);
    switch(stat){
    case Encryptor::SUCCESS:
        dlg = new wxMessageDialog(NULL, action + wxT(" successful!"), wxT("Success!"), wxOK);
        break;
    case Encryptor::KEY_NOT_FOUND:
        dlg = new wxMessageDialog(NULL, wxT("Key not found: ") + fullKey, wxT("Error"), wxOK);
        break;
    case Encryptor::WRONG_PASSWORD:
        dlg = new wxMessageDialog(NULL, action + wxT(" completed!"), wxT("Complete!"), wxOK);
        break;
    case Encryptor::ENCRYPTED:
        dlg = new wxMessageDialog(NULL, wxT("This file is already encrypted!"), wxT("Warning"), wxOK);
        break;
    case Encryptor::DECRYPTED:
        dlg = new wxMessageDialog(NULL, wxT("File is already decrypted!"), wxT("Warning"), wxOK);
        break;
    case Encryptor::FILE_NOT_FOUND:
        dlg = new wxMessageDialog(NULL, wxT("A file is missing!"), wxT("Warning"), wxOK);
        break;
    case Encryptor::INVALID_PASSWORD:
        dlg = new wxMessageDialog(NULL, wxT("Password is invalid!"), wxT("Error"), wxOK);
        break;
    default:
        dlg = new wxMessageDialog(NULL, wxT("Error: Unknown"), wxT("Error"), wxOK);
    }
    dlg->ShowModal();
    delete dlg;
}

/**
 * Closes application when quit button in the menu bar is clicked
 */
void PasswordSaverUIFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

/**
 * Shows a message box of the build version when clicking About in the menu
 */
void PasswordSaverUIFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

/**
 * Opens a specified file when clicked
 */
void PasswordSaverUIFrame::OnChangeButtonClick(wxCommandEvent& event)
{
    wxFileDialog dlg(NULL, wxT("Select a file"));
    dlg.SetWildcard(wxT("*.bin"));
    int confirm = dlg.ShowModal();
    if (confirm==wxID_CANCEL) return;
    std::string str = std::string(dlg.GetDirectory().mb_str());
    dir = str + "\\";
    str = std::string(dlg.GetFilename().mb_str());
    mainFile = str;
    int mainFileSize = mainFile.size();
    int dirSize = dir.size();
    if (mainFileSize==0){
        showStatusDialog(Data::FILE_NOT_FOUND);
        return;
    }
    int periodPos = mainFile.find('.');
    if (periodPos >= 0 && periodPos != std::string::npos){
        std::string ext = mainFile.substr(periodPos, mainFileSize - 1);
        if (ext!=".bin"){
            mainFile = mainFile.substr(0, periodPos) + ".bin";
        }
    }
    else{
        mainFile +=".bin";
    }
    realDir = dir;
    realFile = mainFile;
    if (mainFileSize > 0 && dirSize > 0){
        unsigned int periodPos = mainFile.find('.');
        keyDir = (periodPos!=std::string::npos) ?
        dir + mainFile.substr(0, periodPos) + "\\" :
        dir + mainFile + "\\";
    }
    else{
        keyDir = "";
    }
    file.changeFile(dir, mainFile);
    ChangeButton->SetLabel(wxT("Change file"));
    std::string fileName = realDir + realFile;
    wxString wxFile(fileName.c_str(), wxConvUTF8);
    FileLabel->SetLabel(wxT("File: ") + wxFile);
    if (webLink->IsShown()) webLink->Hide();
    if(isEncrypted()){
        FileBox->Clear();
        showStatusDialog(Encryptor::ENCRYPTED, true);
    }
    else{
        refresh();
    }
}

/**
 * Adds the string object to the list box
 * line by line
 * @param info - the string object to add(whole file or just data structure)
 */
void PasswordSaverUIFrame::addToListBox(std::string info){
    FileBox->Clear();
    int previous = 0;
    for (unsigned int i = 0; i < info.size(); i++){
        if (info.at(i)=='\n'){
            std::string line;
            line = info.substr(previous, i - previous);
            unsigned int pos1 = line.find('(');
            unsigned int pos2 = line.find(')');
            if (pos1!=std::string::npos &&
                pos2!=std::string::npos){
                line = line.substr(0, pos1);
            }
            wxString str(line.c_str(), wxConvUTF8);
            FileBox->Append(str);
            previous = i + 1;
        }
    }
}

/**
 * Refreshes the content of the giant list box
 */
void PasswordSaverUIFrame::refresh(){
    if (isEncrypted()){
        wxMessageDialog error(NULL, wxT("Error: File is encrypted"), wxT("Error"), wxOK | wxICON_ERROR);
        error.ShowModal();
        return;
    }
    std::string data;
    Data::Success stat = file.readFromFile(data);
    if (stat==Data::FILE_NOT_FOUND){
        showStatusDialog(stat);
        return;
    }
    addToListBox(data);
}

/**
 * Prompts user to make a new entry into the file when clicked
 */
void PasswordSaverUIFrame::OnNewButtonClick(wxCommandEvent& event)
{
    std::string testStr;
    Data::Success test = file.readFromFile(testStr);
    if (test!=Data::SUCCESS){
        showStatusDialog(test);
        return;
    }
    if (isEncrypted()) {
        wxMessageDialog error(NULL, wxT("Error: File is encrypted"), wxT("Error"), wxOK | wxICON_ERROR);
        error.ShowModal();
        return;
    }
    MultiTextDialog dlg(NULL, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    int confirm = dlg.ShowModal();
    if (confirm==wxID_CANCEL) return;
    std::string website = dlg.getTextBox(1);
    std::string urlStr = dlg.getTextBox(2);
    std::string username = dlg.getTextBox(3);
    std::string password = dlg.getTextBox(4);
    Data::Success stat;
    if (file.findSearch(website).getWebsite()!="Null"){
        stat = file.changeInfo(website, username, password);
    }
    else{
        Info info(website + "(" + urlStr + ")", username, password);
        stat = file.writeIntoFile(info);
    }
    file.close();
    if (stat!=Data::SUCCESS){
        showStatusDialog(stat);
        FileLabel->SetLabel(wxT("File: Error entering new credentials"));
        return;
    }
    else{
        FileLabel->SetLabel(wxT("File: Entered new password for website"));
        refresh();
    }
    if (webLink->IsShown()) webLink->Hide();
}

/**
 * Prompts the user to search for an entry when search button is clicked
 */
void PasswordSaverUIFrame::OnSearchButtonClick(wxCommandEvent& event)
{
    std::string testStr;
    Data::Success test = file.readFromFile(testStr);
    if (test!=Data::SUCCESS){
        showStatusDialog(test);
        return;
    }
    if (isEncrypted()) {
        wxMessageDialog error(NULL, wxT("Error: File is encrypted"), wxT("Error"), wxOK | wxICON_ERROR);
        error.ShowModal();
        return;
    }
    wxTextEntryDialog dlg(NULL, wxT("Enter the website for the password"), wxT("Website"), wxT(""), wxOK | wxCANCEL);
    int confirm = dlg.ShowModal();
    if (confirm==wxID_CANCEL) return;
    std::string str = std::string(dlg.GetValue().mb_str());
    Info result = file.findSearch(str);
    if(result.getWebsite()=="Null"){
        showStatusDialog(Data::NO_RESULTS);
        return;
    }
    file.close();
    std::string webStr;
    std::string urlStr = "null";
    Info* info;
    unsigned int pos1 = result.getWebsite().find('(');
    unsigned int pos2 = result.getWebsite().find(')');
    if (pos1!=std::string::npos &&
        pos2!=std::string::npos){
        webStr = result.getWebsite().substr(0, pos1);
        pos1++;
        urlStr = result.getWebsite().substr(pos1, pos2-pos1);
        info = new Info(webStr, result.getUsername(), result.getPassword());
    }
    else{
        info = new Info(result.getWebsite(), result.getUsername(), result.getPassword());
    }
    if(info->getWebsite()=="Null"){
        showStatusDialog(Data::NO_RESULTS);
        return;
    }
    addToListBox(info->toStr());
    if (urlStr!="null"){
        wxString web(webStr.c_str(), wxConvUTF8);
        wxString link(urlStr.c_str(), wxConvUTF8);
        webLink->SetURL(link);
        if (!webLink->IsShown()) webLink->Show();
    }
    delete info;
    FileLabel->SetLabel(wxT("File: Searched for login info"));
}

/**
 * Displays entire file content to the list box when display button is clicked
 */
void PasswordSaverUIFrame::OnDisplayButtonClick(wxCommandEvent& event)
{
    refresh();
    std::string fileName = realDir + realFile;
    wxString file(fileName.c_str(), wxConvUTF8);
    FileLabel->SetLabel(wxT("File: ") + file);
    if (webLink->IsShown()) webLink->Hide();
}

/**
 * Encrypt the file with either PIN or passwowrd
 * @param usePIN - whether to use PIN or password
 * @param passcode - the PIN or password string
 * @return the Encryptor class generated enumeration result
 */
Encryptor::Status PasswordSaverUIFrame::encrypt(std::string passcode){
    file.close();
    mkdir(keyDir.c_str());
    Encryptor aes(keyDir);
    return aes.encrypt(passcode, dir + mainFile);
}

/**
 * Decrypt the file with either PIN or passwowrd
 * @param passcode - the PIN or password string
 * @return the Encryptor class generated enumeration result
 */
Encryptor::Status PasswordSaverUIFrame::decrypt(std::string passcode){
    file.close();
    Encryptor aes(keyDir);
    return aes.decrypt(passcode, dir + mainFile);
}

/**
 * Encrypt/Decrypt the dummy file
 * @param state - encrypt(true) or decrypt(false)
 */
void PasswordSaverUIFrame::encrypt(bool state){
    std::string defaultFile = "key0.bin";
    Encryptor obj;
    if (obj.isEncrypted(keyDir + "key0.bin")!=state){
        obj.encryptFile(state, keyDir + "key0.bin",
        "abcdefghijklmnop",
        "abcdefghijklmnop");
    }
}

/**
 * Is the file in the file stream object encrypted?
 * @return whether it is encrypted
 */
bool PasswordSaverUIFrame::isEncrypted(){
    file.close();
    Encryptor obj;
    return obj.isEncrypted(dir + mainFile);
}

/**
 * Prompts user to encrypt the file when encrypt button is clicked
 */
void PasswordSaverUIFrame::OnEncryptButtonClick(wxCommandEvent& event)
{
    std::string testStr;
    Data::Success test = file.readFromFile(testStr);
    if (test!=Data::SUCCESS){
        showStatusDialog(test);
        return;
    }
    if (isEncrypted()){
        showStatusDialog(Encryptor::ENCRYPTED, true);
        return;
    }
    int confirm;
    std::string info;
    do{
        wxPasswordEntryDialog dlg(NULL, wxT("Enter a new password"), wxT("New password"), wxEmptyString,
                                    wxOK | wxCANCEL);
        confirm = dlg.ShowModal();
        if (confirm==wxID_CANCEL) return;
        info = std::string(dlg.GetValue().mb_str());
        wxMessageDialog msg(NULL, wxT("Do you remember your password?"), wxT("Confirm"), wxYES_NO |
                        wxNO_DEFAULT | wxICON_QUESTION);
        confirm = msg.ShowModal();
    } while (confirm==wxID_NO);
    FileBox->Clear();
    std::string fullFile = realDir + realFile;
    wxString str(fullFile.c_str(), wxConvUTF8);
    FileLabel->SetLabel(wxT("File: ") + str);
    if (realDir + realFile != dir + mainFile){
        encrypt(true);
        showStatusDialog(Encryptor::WRONG_PASSWORD, true);
        return;
    }
    Data dummy(keyDir, "key0.bin");
    std::string hashStr = "";
    for (unsigned int i = 0; i < testStr.size(); i++){
        char c = testStr.at(i);
        hashStr += (c=='\n') ? c : '-';
    }
    dummy.writeIntoFile(hashStr, true);
    Encryptor::Status stats = encrypt(info);
    showStatusDialog(stats, true);
}

/**
 * Prompts user to decrypt the file when decrypt button is clicked
 */
void PasswordSaverUIFrame::OnDecryptButtonClick(wxCommandEvent& event)
{
    dir = realDir;
    mainFile = realFile;
    file.changeFile(dir, mainFile);
    std::string testStr;
    Data::Success test = file.readFromFile(testStr);
    if (test!=Data::SUCCESS){
        showStatusDialog(test);
        return;
    }
    if (!isEncrypted()){
        showStatusDialog(Encryptor::DECRYPTED, false);
        return;
    }
    wxPasswordEntryDialog dlg(NULL, wxT("Enter your password"), wxT("Authenticate"), wxEmptyString,
                                 wxOK | wxCANCEL);
    int confirm = dlg.ShowModal();
    if (confirm==wxID_CANCEL) {
        dir = keyDir;
        mainFile = "key0.bin";
        file.changeFile(dir, mainFile);
        return;
    }
    std::string info = std::string(dlg.GetValue().mb_str());
    Encryptor::Status stats = decrypt(info);
    if (stats==Encryptor::WRONG_PASSWORD){
        encrypt(false);
        dir = keyDir;
        mainFile = "key0.bin";
        file.changeFile(dir, mainFile);
    }
    showStatusDialog(stats, false);
    refresh();
}

/**
 * Prompts user to delete a specified entry when delete button is clicked
 */
void PasswordSaverUIFrame::OnDeleteButtonClick(wxCommandEvent& event)
{
    std::string testStr;
    Data::Success test = file.readFromFile(testStr);
    if (test!=Data::SUCCESS){
        showStatusDialog(test);
        return;
    }
    if (isEncrypted()){
        wxMessageDialog error(NULL, wxT("Error: File is encrypted"), wxT("Error"), wxOK | wxICON_ERROR);
        error.ShowModal();
        return;
    }
    wxTextEntryDialog dlg(NULL, wxT("Enter the website to delete information"), wxT("Website"), wxT(""), wxOK | wxCANCEL);
    int confirm = dlg.ShowModal();
    if (confirm==wxID_CANCEL) return;
    wxString website = dlg.GetValue();
    std::string str = std::string(website.mb_str());
    wxMessageDialog msg(NULL, wxT("Are you sure? (This cannot be undone)"), wxT("Confirm"), wxYES_NO |
                        wxNO_DEFAULT | wxICON_QUESTION);
    confirm = msg.ShowModal();
    if (confirm==wxID_NO) return;
    Data::Success stat = file.changeInfo(str, "null", Data::DE);
    file.close();
    if (stat!=Data::SUCCESS){
        showStatusDialog(stat);
        FileLabel->SetLabel(wxT("File: Error deleting website credential"));
        return;
    }
    else{
        FileLabel->SetLabel(wxT("File: Deleted information for website: ") + website);
        refresh();
    }
    if (webLink->IsShown()) webLink->Hide();
}

/**
 * Prompts user to edit the content of a file entry when edit button is clicked
 */
void PasswordSaverUIFrame::OnEditButtonClick(wxCommandEvent& event)
{
    std::string testStr;
    Data::Success test = file.readFromFile(testStr);
    if (test!=Data::SUCCESS){
        showStatusDialog(test);
        return;
    }
    if (isEncrypted()){
        wxMessageDialog error(NULL, wxT("Error: File is encrypted"), wxT("Error"), wxOK | wxICON_ERROR);
        error.ShowModal();
        return;
    }
    wxArrayString arr;
    arr.Add(wxT("Username"));
    arr.Add(wxT("Password"));
    wxSingleChoiceDialog choice(NULL, wxT("Choose which to edit:"), wxT("Username or password"), arr, NULL, wxOK | wxCANCEL);
    int confirm = choice.ShowModal();
    if (confirm==wxID_CANCEL) return;
    Data::Change mode = (choice.GetSelection()==0) ? Data::US : Data::PS;
    wxString prompt = (choice.GetSelection()==0) ? wxT("username") : wxT("password");
    wxTextEntryDialog dlg1(NULL, wxT("Enter the website to change passwords for"), wxT("Website"), wxT(""), wxOK | wxCANCEL);
    wxTextEntryDialog dlg2(NULL, wxT("Enter new ") + prompt, wxT("New ") + prompt, wxT(""), wxOK | wxCANCEL);
    confirm = dlg1.ShowModal();
    if (confirm==wxID_CANCEL) return;
    wxString webStr = dlg1.GetValue();
    std::string website = std::string(webStr.mb_str());
    if (file.findSearch(website).getWebsite()=="Null"){
        showStatusDialog(Data::NO_RESULTS);
        return;
    }
    confirm = dlg2.ShowModal();
    if (confirm==wxID_CANCEL) return;
    std::string info = std::string(dlg2.GetValue().mb_str());
    Data::Success stat = file.changeInfo(website, info, mode);
    file.close();
    if (stat!=Data::SUCCESS){
        showStatusDialog(stat);
        FileLabel->SetLabel(wxT("File: Error changing website credential"));
        return;
    }
    else{
        FileLabel->SetLabel(wxT("File: Changed password for website: ") + webStr);
        refresh();
    }
    if (webLink->IsShown()) webLink->Hide();
}

/**
 * Clears the list box when clear button is clicked
 */
void PasswordSaverUIFrame::OnClearButtonClick1(wxCommandEvent& event)
{
    FileBox->Clear();
}
