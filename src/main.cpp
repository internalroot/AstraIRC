#include <wx/wx.h>
#include "MainFrame.h"

class AstraApp : public wxApp
{
public:
    bool OnInit() override
    {
        auto* frame = new MainFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(AstraApp);
