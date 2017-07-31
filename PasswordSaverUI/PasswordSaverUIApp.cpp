/***************************************************************
 * Name:      PasswordSaverUIApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Joel Veneracion (84veneracionj@gmail.com)
 * Created:   2017-07-26
 * Copyright: Joel Veneracion (84veneracionj@gmail.com)
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "PasswordSaverUIApp.h"

//(*AppHeaders
#include "PasswordSaverUIMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(PasswordSaverUIApp);

bool PasswordSaverUIApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	PasswordSaverUIFrame* Frame = new PasswordSaverUIFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
