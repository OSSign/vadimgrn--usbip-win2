/*
 * Copyright (C) 2023 - 2025 Vadym Hrynchyshyn <vadimgrn@gmail.com>
 */

#include "request.h"

/*
 * UDECXUSBDEVICE never receives USB_REQUEST_SET_CONFIGURATION and USB_REQUEST_SET_INTERFACE 
 * in URB_FUNCTION_CONTROL_TRANSFER because UDE handles them itself. Therefore these requests 
 * are passed as USB_REQUEST_GET_FIRMWARE_STATUS.
 */
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
void usbip::filter::pack_request(
        _Out_ _URB_CONTROL_TRANSFER_EX &r, _In_ void *TransferBuffer, _In_ USHORT function)
{
        r.Hdr.Length = sizeof(r);
        r.Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER_EX;

        r.TransferBuffer = TransferBuffer;
        NT_ASSERT(!r.TransferBufferLength);

        r.TransferFlags = USBD_DEFAULT_PIPE_TRANSFER | USBD_TRANSFER_DIRECTION_IN;
        r.Timeout = impl::const_part | function;

        get_setup_packet(r) = impl::setup_packet;

        NT_ASSERT(is_request(r));
        NT_ASSERT(get_function(r) == function);
}
