/*
 * Copyright (C) 2022 - 2025 Vadym Hrynchyshyn <vadimgrn@gmail.com>
 */

#include "proto.h"
#include "trace.h"
#include "proto.tmh"

#include "context.h"

#include <libdrv\ch9.h>
#include <libdrv\usbd_helper.h>

namespace
{

_IRQL_requires_max_(DISPATCH_LEVEL)
auto fix_transfer_flags(_In_ ULONG TransferFlags, _In_ bool dir_out)
{
	if (IsTransferDirectionOut(TransferFlags) == dir_out) {
		return TransferFlags;
	}

	TraceDbg("Fix direction in TransferFlags(%#lx) to %s", TransferFlags, dir_out ? "OUT" : "IN");

	const ULONG in_flags = USBD_SHORT_TRANSFER_OK | USBD_TRANSFER_DIRECTION_IN;

	if (dir_out) {
		TransferFlags &= ~in_flags;
	} else {
		TransferFlags |= in_flags;
	}

	NT_ASSERT(IsTransferDirectionOut(TransferFlags) == dir_out);
	return TransferFlags;
}

} // namespace


/*
 * Direction in TransferFlags can be invalid for bulk transfer at least.
 * Always use direction from endpoint descriptor except for control pipe where setup packet has direction.
 * 
 * Default control pipe is bidirectional, direction in setup packet must be used instead of descriptor.
 * FIXME: are there exist non-default unidirectional control pipes?
 */
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS usbip::set_cmd_submit_usbip_header(
	_Out_ header &hdr, _Inout_ device_ctx &dev, _In_ const USB_ENDPOINT_DESCRIPTOR &epd,
	_In_ ULONG TransferFlags, _In_ ULONG TransferBufferLength, _In_ setup_dir setup_out)
{
	if ((TransferFlags & USBD_DEFAULT_PIPE_TRANSFER) && !usb_default_control_pipe(epd)) {
		Trace(TRACE_LEVEL_ERROR, "Inconsistency between TransferFlags(USBD_DEFAULT_PIPE_TRANSFER) and "
			                 "bEndpointAddress(%#x)", epd.bEndpointAddress);

		return STATUS_INVALID_PARAMETER;
	}
	
	NT_ASSERT(bool(setup_out) == (usb_endpoint_type(epd) == UsbdPipeTypeControl));
	auto dir_out = setup_out ? *setup_out : usb_endpoint_dir_out(epd);

	TransferFlags = fix_transfer_flags(TransferFlags, dir_out);

	hdr.command = CMD_SUBMIT;
	hdr.seqnum = next_seqnum(dev, !dir_out);
	hdr.devid = dev.devid();
	hdr.direction = dir_out ? direction::out : direction::in;
	hdr.ep = usb_endpoint_num(epd);

	if (auto r = &hdr.cmd_submit) {
		r->transfer_flags = to_linux_flags(TransferFlags, !dir_out);
		r->transfer_buffer_length = TransferBufferLength;
		r->start_frame = 0;
		r->number_of_packets = number_of_packets_non_isoch;
		r->interval = epd.bInterval;
		RtlZeroMemory(r->setup, sizeof(r->setup));
	}

	return STATUS_SUCCESS;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
void usbip::set_cmd_unlink_usbip_header(
	_Out_ header &hdr, _Inout_ device_ctx &dev, _In_ seqnum_t seqnum_unlink)
{
	hdr.command = CMD_UNLINK;
	hdr.devid = dev.devid();
	hdr.direction = direction::out;
	hdr.seqnum = next_seqnum(dev, hdr.direction);
	hdr.ep = 0;

	NT_ASSERT(is_valid_seqnum(seqnum_unlink));
	hdr.cmd_unlink.seqnum = seqnum_unlink;
}
