/*
 * Copyright (C) 2021 - 2025 Vadym Hrynchyshyn <vadimgrn@gmail.com>
 */

#include "usbip.h"

#include <libusbip\vhci.h>
#include <spdlog\spdlog.h>

bool usbip::cmd_detach(void *p)
{
	auto &args = *reinterpret_cast<detach_args*>(p);

	auto dev = vhci::open();
	if (!dev) {
		spdlog::error(GetLastErrorMsg());
		return false;
	}

	auto ok = vhci::detach(dev.get(), args.port);

	if (!ok) {
		spdlog::error(GetLastErrorMsg());		
	} else if (args.port <= 0) {
		printf("all ports are detached\n");
	} else {
		printf("port %d is succesfully detached\n", args.port);
	}

	return ok;
}
