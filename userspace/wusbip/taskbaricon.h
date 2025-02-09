﻿/*
 * Copyright (C) 2024 - 2025 Vadym Hrynchyshyn <vadimgrn@gmail.com>
 */

#pragma once

#include <wx/taskbar.h>
#include <memory>

class wxMenu;
class MainFrame;

class TaskBarIcon : public wxTaskBarIcon
{
public:
        TaskBarIcon();

        void show_balloon(_In_ const wxString &text, _In_ int flags = wxICON_INFORMATION);

private:
        bool m_cancel{};
        std::unique_ptr<wxMenu> m_popup;

        wxMenu *GetPopupMenu() override;
        std::unique_ptr<wxMenu> create_popup_menu();
        MainFrame& frame() const;

        void on_balloon_timeout(wxTaskBarIconEvent&) { m_cancel = false; }
        void on_left_dclick(wxTaskBarIconEvent&);
        void on_open(wxCommandEvent&);
};
