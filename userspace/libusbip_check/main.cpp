/*
 * Copyright (C) 2023 - 2025 Vadym Hrynchyshyn <vadimgrn@gmail.com>
 */

// Test libusbip API for C++17 compatibility.

#include <libusbip\format_message.h>
#include <libusbip\win_handle.h>
#include <libusbip\src\setupapi.h>
#include <libusbip\src\hkey.h>
#include <libusbip\output.h>
#include <libusbip\remote.h>
#include <libusbip\vhci.h>
#include <libusbip\persistent.h>

int main()
{
        using namespace usbip;
        
        libusbip::set_debug_output([] (auto) {});
        wformat_message(ERROR_INVALID_PARAMETER);
        hdevinfo devinfo;
        HKey key;
        HModule module;
        connect("", "1234");
        vhci::open();
}
