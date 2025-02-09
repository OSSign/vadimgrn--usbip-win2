/*
 * Copyright (C) 2022 - 2025 Vadym Hrynchyshyn <vadimgrn@gmail.com>
 */

#include "dbgcommon.h"
#include "usbd_helper.h"

#include <usbip\proto.h>
#include <usbip\vhci.h>

#include <usb.h>
#include <usbioctl.h>
#include <usbuser.h>
#include <ntstrsafe.h>

namespace
{

constexpr auto bmrequest_dir(BM_REQUEST_TYPE r)
{
	return r.s.Dir == BMREQUEST_HOST_TO_DEVICE ? "OUT" : "IN";
}

void print_cmd_submit(char *buf, size_t len, const usbip_header_cmd_submit *cmd, bool setup)
{
	auto st = RtlStringCbPrintfExA(buf, len,  &buf, &len, 0, 
					"cmd_submit: flags %#x, length %d, start_frame %d, isoc[%d], interval %d%s",
					cmd->transfer_flags, cmd->transfer_buffer_length, cmd->start_frame, 
					cmd->number_of_packets, cmd->interval, setup ? ", " : "");

	if (!st && setup) {
		usb_setup_pkt_str(buf, len, cmd->setup);
	}
}

void print_ret_submit(char *buf, size_t len, const usbip_header_ret_submit *cmd)
{
	RtlStringCbPrintfA(buf, len, "ret_submit: status %d, actual_length %d, start_frame %d, isoc[%d], error_count %d", 
			   cmd->status, cmd->actual_length, cmd->start_frame, cmd->number_of_packets, cmd->error_count);
}

} // namespace


const char *request_type(UCHAR type)
{
	static const char* v[] = { "STANDARD", "CLASS", "VENDOR", "BMREQUEST_3" };
	NT_ASSERT(type < ARRAYSIZE(v));
	return v[type];
}

const char *request_recipient(UCHAR recipient)
{
	static const char* v[] = { "DEVICE", "INTERFACE", "ENDPOINT", "OTHER" };
	NT_ASSERT(recipient < ARRAYSIZE(v));
	return v[recipient];
}

const char *brequest_str(UCHAR bRequest)
{
	switch (bRequest) {
	case USB_REQUEST_GET_STATUS: return "GET_STATUS";
	case USB_REQUEST_CLEAR_FEATURE: return "CLEAR_FEATURE";
	case USB_REQUEST_SET_FEATURE: return "SET_FEATURE";
	case USB_REQUEST_SET_ADDRESS: return "SET_ADDRESS";
	case USB_REQUEST_GET_DESCRIPTOR: return "GET_DESCRIPTOR";
	case USB_REQUEST_SET_DESCRIPTOR: return "SET_DESCRIPTOR";
	case USB_REQUEST_GET_CONFIGURATION: return "GET_CONFIGURATION";
	case USB_REQUEST_SET_CONFIGURATION: return "SET_CONFIGURATION";
	case USB_REQUEST_GET_INTERFACE: return "GET_INTERFACE";
	case USB_REQUEST_SET_INTERFACE: return "SET_INTERFACE";
	case USB_REQUEST_SYNC_FRAME: return "SYNC_FRAME";
	case USB_REQUEST_GET_FIRMWARE_STATUS: return "GET_FIRMWARE_STATUS";
	case USB_REQUEST_SET_FIRMWARE_STATUS: return "SET_FIRMWARE_STATUS";
	case USB_REQUEST_SET_SEL: return "SET_SEL";
	case USB_REQUEST_ISOCH_DELAY: return "ISOCH_DELAY";
	}

	return "?";
}

const char *get_usbd_status(USBD_STATUS status)
{
	switch (status) {
	case USBD_STATUS_SUCCESS: 
		return "SUCCESS";
	case USBD_STATUS_PORT_OPERATION_PENDING:
		return "PORT_OPERATION_PENDING";
	case USBD_STATUS_PENDING:
		return "PENDING";
	case USBD_STATUS_CRC:
		return "CRC";
	case USBD_STATUS_BTSTUFF:
		return "BTSTUFF";
	case USBD_STATUS_DATA_TOGGLE_MISMATCH:
		return "DATA_TOGGLE_MISMATCH";
	case USBD_STATUS_STALL_PID:
		return "STALL_PID";
	case USBD_STATUS_DEV_NOT_RESPONDING:
		return "DEV_NOT_RESPONDING";
	case USBD_STATUS_PID_CHECK_FAILURE:
		return "PID_CHECK_FAILURE";
	case USBD_STATUS_UNEXPECTED_PID:
		return "UNEXPECTED_PID";
	case USBD_STATUS_DATA_OVERRUN:
		return "DATA_OVERRUN";
	case USBD_STATUS_DATA_UNDERRUN:
		return "DATA_UNDERRUN";
	case USBD_STATUS_BUFFER_OVERRUN:
		return "BUFFER_OVERRUN";
	case USBD_STATUS_BUFFER_UNDERRUN:
		return "BUFFER_UNDERRUN";
	case USBD_STATUS_NOT_ACCESSED:
		return "NOT_ACCESSED";
	case USBD_STATUS_FIFO:
		return "FIFO";
	case USBD_STATUS_XACT_ERROR:
		return "XACT_ERROR";
	case USBD_STATUS_BABBLE_DETECTED:
		return "BABBLE_DETECTED";
	case USBD_STATUS_DATA_BUFFER_ERROR:
		return "DATA_BUFFER_ERROR";
	case USBD_STATUS_NO_PING_RESPONSE:
		return "NO_PING_RESPONSE";
	case USBD_STATUS_INVALID_STREAM_TYPE:
		return "INVALID_STREAM_TYPE";
	case USBD_STATUS_INVALID_STREAM_ID:
		return "INVALID_STREAM_ID";
	case USBD_STATUS_ENDPOINT_HALTED:
		return "ENDPOINT_HALTED";
	case USBD_STATUS_INVALID_URB_FUNCTION:
		return "INVALID_URB_FUNCTION";
	case USBD_STATUS_INVALID_PARAMETER:
		return "INVALID_PARAMETER";
	case USBD_STATUS_ERROR_BUSY:
		return "ERROR_BUSY";
	case USBD_STATUS_INVALID_PIPE_HANDLE:
		return "INVALID_PIPE_HANDLE";
	case USBD_STATUS_NO_BANDWIDTH:
		return "NO_BANDWIDTH";
	case USBD_STATUS_INTERNAL_HC_ERROR:
		return "INTERNAL_HC_ERROR";
	case USBD_STATUS_ERROR_SHORT_TRANSFER:
		return "ERROR_SHORT_TRANSFER";
	case USBD_STATUS_BAD_START_FRAME:
		return "BAD_START_FRAME";
	case USBD_STATUS_ISOCH_REQUEST_FAILED:
		return "ISOCH_REQUEST_FAILED";
	case USBD_STATUS_FRAME_CONTROL_OWNED:
		return "FRAME_CONTROL_OWNED";
	case USBD_STATUS_FRAME_CONTROL_NOT_OWNED:
		return "FRAME_CONTROL_NOT_OWNED";
	case USBD_STATUS_NOT_SUPPORTED:
		return "NOT_SUPPORTED";
	case USBD_STATUS_INAVLID_CONFIGURATION_DESCRIPTOR:
		return "INAVLID_CONFIGURATION_DESCRIPTOR";
	case USBD_STATUS_INSUFFICIENT_RESOURCES:
		return "INSUFFICIENT_RESOURCES";
	case USBD_STATUS_SET_CONFIG_FAILED:
		return "SET_CONFIG_FAILED";
	case USBD_STATUS_BUFFER_TOO_SMALL:
		return "BUFFER_TOO_SMALL";
	case USBD_STATUS_INTERFACE_NOT_FOUND:
		return "INTERFACE_NOT_FOUND";
	case USBD_STATUS_INAVLID_PIPE_FLAGS:
		return "INAVLID_PIPE_FLAGS";
	case USBD_STATUS_TIMEOUT:
		return "TIMEOUT";
	case USBD_STATUS_DEVICE_GONE:
		return "DEVICE_GONE";
	case USBD_STATUS_STATUS_NOT_MAPPED:
		return "STATUS_NOT_MAPPED";
	case USBD_STATUS_HUB_INTERNAL_ERROR:
		return "HUB_INTERNAL_ERROR";
	case USBD_STATUS_CANCELED:
		return "CANCELED";
	case USBD_STATUS_ISO_NOT_ACCESSED_BY_HW:
		return "ISO_NOT_ACCESSED_BY_HW";
	case USBD_STATUS_ISO_TD_ERROR:
		return "ISO_TD_ERROR";
	case USBD_STATUS_ISO_NA_LATE_USBPORT:
		return "ISO_NA_LATE_USBPORT";
	case USBD_STATUS_ISO_NOT_ACCESSED_LATE:
		return "ISO_NOT_ACCESSED_LATE";
	case USBD_STATUS_BAD_DESCRIPTOR:
		return "BAD_DESCRIPTOR";
	case USBD_STATUS_BAD_DESCRIPTOR_BLEN:
		return "BAD_DESCRIPTOR_BLEN";
	case USBD_STATUS_BAD_DESCRIPTOR_TYPE:
		return "BAD_DESCRIPTOR_TYPE";
	case USBD_STATUS_BAD_INTERFACE_DESCRIPTOR:
		return "BAD_INTERFACE_DESCRIPTOR";
	case USBD_STATUS_BAD_ENDPOINT_DESCRIPTOR:
		return "BAD_ENDPOINT_DESCRIPTOR";
	case USBD_STATUS_BAD_INTERFACE_ASSOC_DESCRIPTOR:
		return "BAD_INTERFACE_ASSOC_DESCRIPTOR";
	case USBD_STATUS_BAD_CONFIG_DESC_LENGTH:
		return "BAD_CONFIG_DESC_LENGTH";
	case USBD_STATUS_BAD_NUMBER_OF_INTERFACES:
		return "BAD_NUMBER_OF_INTERFACES";
	case USBD_STATUS_BAD_NUMBER_OF_ENDPOINTS:
		return "BAD_NUMBER_OF_ENDPOINTS";
	case USBD_STATUS_BAD_ENDPOINT_ADDRESS:
		return "BAD_ENDPOINT_ADDRESS";
	}

	return "?";
}

/*
 * For IRP_MJ_DEVICE_CONTROL, IOCTL_USB_USER_REQUEST.
 */
const char *usbuser_request_name(_In_ ULONG UsbUserRequest)
{
	switch (UsbUserRequest) {
	case USBUSER_GET_CONTROLLER_INFO_0: return "GET_CONTROLLER_INFO_0";
	case USBUSER_GET_CONTROLLER_DRIVER_KEY: return "GET_CONTROLLER_DRIVER_KEY";
	case USBUSER_PASS_THRU: return "PASS_THRU";
	case USBUSER_GET_POWER_STATE_MAP: return "GET_POWER_STATE_MAP";
	case USBUSER_GET_BANDWIDTH_INFORMATION: return "GET_BANDWIDTH_INFORMATION";
	case USBUSER_GET_BUS_STATISTICS_0: return "GET_BUS_STATISTICS_0";
	case USBUSER_GET_ROOTHUB_SYMBOLIC_NAME: return "GET_ROOTHUB_SYMBOLIC_NAME";
	case USBUSER_GET_USB_DRIVER_VERSION: return "GET_USB_DRIVER_VERSION";
	case USBUSER_GET_USB2_HW_VERSION: return "GET_USB2_HW_VERSION";
	case USBUSER_USB_REFRESH_HCT_REG: return "USB_REFRESH_HCT_REG";

	case USBUSER_OP_SEND_ONE_PACKET: return "OP_SEND_ONE_PACKET";
	case USBUSER_OP_RAW_RESET_PORT: return "OP_RAW_RESET_PORT";
	case USBUSER_OP_OPEN_RAW_DEVICE: return "OP_OPEN_RAW_DEVICE";
	case USBUSER_OP_CLOSE_RAW_DEVICE: return "OP_CLOSE_RAW_DEVICE";
	case USBUSER_OP_SEND_RAW_COMMAND: return "OP_SEND_RAW_COMMAND";
	case USBUSER_OP_MASK_DEVONLY_API: return "OP_MASK_DEVONLY_API";
	case USBUSER_OP_MASK_HCTEST_API: return "OP_MASK_HCTEST_API";

	case USBUSER_SET_ROOTPORT_FEATURE: return "SET_ROOTPORT_FEATURE";
	case USBUSER_CLEAR_ROOTPORT_FEATURE: return "CLEAR_ROOTPORT_FEATURE";
	case USBUSER_GET_ROOTPORT_STATUS: return "GET_ROOTPORT_STATUS";

	case USBUSER_INVALID_REQUEST: return "INVALID_REQUEST";
	}

	return "?";
}

/*
 * For IRP_MJ_DEVICE_CONTROL.
 */
const char *device_control_name(ULONG ioctl_code)
{
	using namespace usbip;
	static_assert(sizeof(ioctl_code) == sizeof(vhci::ioctl::PLUGIN_HARDWARE));

	switch (ioctl_code) {
	case vhci::ioctl::PLUGIN_HARDWARE: return "vhci_plugin_hardware";
	case vhci::ioctl::PLUGOUT_HARDWARE: return "vhci_plugout_hardware";
	case vhci::ioctl::GET_IMPORTED_DEVICES: return "vhci_get_imported_devices";
	case vhci::ioctl::GET_PERSISTENT: return "vhci_get_persistent";
	case vhci::ioctl::SET_PERSISTENT: return "vhci_set_persistent";

	case IOCTL_USB_DIAG_IGNORE_HUBS_ON: return "USB_DIAG_IGNORE_HUBS_ON";
	case IOCTL_USB_DIAG_IGNORE_HUBS_OFF: return "USB_DIAG_IGNORE_HUBS_OFF";
	case IOCTL_USB_DIAGNOSTIC_MODE_OFF: return "USB_DIAGNOSTIC_MODE_OFF";
	case IOCTL_USB_DIAGNOSTIC_MODE_ON: return "USB_DIAGNOSTIC_MODE_ON";
	case IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION: return "USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION";
	case IOCTL_USB_GET_HUB_CAPABILITIES: return "USB_GET_HUB_CAPABILITIES";
	
	static_assert(IOCTL_USB_GET_ROOT_HUB_NAME == IOCTL_USB_GET_NODE_INFORMATION);
	case IOCTL_USB_GET_ROOT_HUB_NAME: return "USB_GET_ROOT_HUB_NAME|USB_GET_NODE_INFORMATION";

	case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION: return "USB_GET_NODE_CONNECTION_INFORMATION";
	case IOCTL_USB_GET_NODE_CONNECTION_ATTRIBUTES: return "USB_GET_NODE_CONNECTION_ATTRIBUTES";
	case IOCTL_USB_GET_NODE_CONNECTION_NAME: return "USB_GET_NODE_CONNECTION_NAME";
	case IOCTL_USB_HCD_DISABLE_PORT: return "USB_HCD_DISABLE_PORT";
	case IOCTL_USB_HCD_ENABLE_PORT: return "USB_HCD_ENABLE_PORT";
	case IOCTL_USB_HCD_GET_STATS_1: return "USB_HCD_GET_STATS_1";
	case IOCTL_USB_HCD_GET_STATS_2: return "USB_HCD_GET_STATS_2";
	case IOCTL_USB_USER_REQUEST: return "USB_USER_REQUEST";
	case IOCTL_USB_HUB_CYCLE_PORT: return "USB_HUB_CYCLE_PORT";
	case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX: return "USB_GET_NODE_CONNECTION_INFORMATION_EX";
	case IOCTL_USB_RESET_HUB: return "USB_RESET_HUB";
	case IOCTL_USB_GET_HUB_CAPABILITIES_EX: return "USB_GET_HUB_CAPABILITIES_EX";
	case IOCTL_USB_GET_HUB_INFORMATION_EX: return "USB_GET_HUB_INFORMATION_EX";
	case IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES: return "USB_GET_PORT_CONNECTOR_PROPERTIES";
	case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX_V2: return "USB_GET_NODE_CONNECTION_INFORMATION_EX_V2";
	
	case IOCTL_GET_HCD_DRIVERKEY_NAME: return "GET_HCD_DRIVERKEY_NAME";
	static_assert(IOCTL_GET_HCD_DRIVERKEY_NAME == IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME); // beware
	
	case IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME: return "USB_GET_NODE_CONNECTION_DRIVERKEY_NAME";
	}

	return "?";
}

/*
 * For IRP_MJ_INTERNAL_DEVICE_CONTROL.
 */
const char *internal_device_control_name(ULONG ioctl_code)
{
	static_assert(sizeof(ioctl_code) == sizeof(IOCTL_INTERNAL_USB_CYCLE_PORT));

	switch (ioctl_code) {
	case IOCTL_INTERNAL_USB_CYCLE_PORT: return "INTERNAL_USB_CYCLE_PORT";
	case IOCTL_INTERNAL_USB_ENABLE_PORT: return "INTERNAL_USB_ENABLE_PORT";
	case IOCTL_INTERNAL_USB_GET_BUS_INFO: return "INTERNAL_USB_GET_BUS_INFO|USB_GET_NODE_CONNECTION_DRIVERKEY_NAME";
	case IOCTL_INTERNAL_USB_GET_BUSGUID_INFO: return "INTERNAL_USB_GET_BUSGUID_INFO";
	case IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME: return "INTERNAL_USB_GET_CONTROLLER_NAME";
	case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE: return "INTERNAL_USB_GET_DEVICE_HANDLE";
	case IOCTL_INTERNAL_USB_GET_HUB_COUNT: return "INTERNAL_USB_GET_HUB_COUNT";
	case IOCTL_INTERNAL_USB_GET_HUB_NAME: return "INTERNAL_USB_GET_HUB_NAME";
	case IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO: return "INTERNAL_USB_GET_PARENT_HUB_INFO";
	case IOCTL_INTERNAL_USB_GET_PORT_STATUS: return "INTERNAL_USB_GET_PORT_STATUS";
	case IOCTL_INTERNAL_USB_RESET_PORT: return "INTERNAL_USB_RESET_PORT";
	case IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO: return "INTERNAL_USB_GET_ROOTHUB_PDO";
	case IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION: return "INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION";
	case IOCTL_INTERNAL_USB_SUBMIT_URB: return "INTERNAL_USB_SUBMIT_URB";
	case IOCTL_INTERNAL_USB_GET_TOPOLOGY_ADDRESS: return "INTERNAL_USB_GET_TOPOLOGY_ADDRESS";
	case IOCTL_INTERNAL_USB_NOTIFY_IDLE_READY: return "INTERNAL_USB_NOTIFY_IDLE_READY";
	case IOCTL_INTERNAL_USB_REQ_GLOBAL_SUSPEND: return "INTERNAL_USB_REQ_GLOBAL_SUSPEND";
	case IOCTL_INTERNAL_USB_REQ_GLOBAL_RESUME: return "INTERNAL_USB_REQ_GLOBAL_RESUME";
	case IOCTL_INTERNAL_USB_RECORD_FAILURE: return "INTERNAL_USB_RECORD_FAILURE";
	case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX: return "INTERNAL_USB_GET_DEVICE_HANDLE_EX";
	case IOCTL_INTERNAL_USB_GET_TT_DEVICE_HANDLE: return "INTERNAL_USB_GET_TT_DEVICE_HANDLE";
	case IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO: return "INTERNAL_USB_GET_DEVICE_CONFIG_INFO";
	case IOCTL_INTERNAL_USB_REGISTER_COMPOSITE_DEVICE: return "INTERNAL_USB_REGISTER_COMPOSITE_DEVICE";
	case IOCTL_INTERNAL_USB_UNREGISTER_COMPOSITE_DEVICE: return "INTERNAL_USB_UNREGISTER_COMPOSITE_DEVICE";
	case IOCTL_INTERNAL_USB_REQUEST_REMOTE_WAKE_NOTIFICATION: return "INTERNAL_USB_REQUEST_REMOTE_WAKE_NOTIFICATION";
	case IOCTL_INTERNAL_USB_FAIL_GET_STATUS_FROM_DEVICE: return "INTERNAL_USB_FAIL_GET_STATUS_FROM_DEVICE";
	}

	return "?";
}

const char *usbd_pipe_type_str(USBD_PIPE_TYPE t)
{
	static const char* v[] = { "Ctrl", "Isoch", "Bulk", "Intr" };
	NT_ASSERT(t < ARRAYSIZE(v));
	return v[t];
}

/*
 * Can't use CUSTOM_TYPE(urb_function, ItemListShort(...)), it's too big for WPP.
 */
const char *urb_function_str(int function)
{
	static const char* v[] = 
	{
		"SELECT_CONFIGURATION",
		"SELECT_INTERFACE",
		"ABORT_PIPE",

		"TAKE_FRAME_LENGTH_CONTROL",
		"RELEASE_FRAME_LENGTH_CONTROL",

		"GET_FRAME_LENGTH",
		"SET_FRAME_LENGTH",
		"GET_CURRENT_FRAME_NUMBER",

		"CONTROL_TRANSFER",
		"BULK_OR_INTERRUPT_TRANSFER",
		"ISOCH_TRANSFER",

		"GET_DESCRIPTOR_FROM_DEVICE",
		"SET_DESCRIPTOR_TO_DEVICE",

		"SET_FEATURE_TO_DEVICE",
		"SET_FEATURE_TO_INTERFACE",
		"SET_FEATURE_TO_ENDPOINT",

		"CLEAR_FEATURE_TO_DEVICE",
		"CLEAR_FEATURE_TO_INTERFACE",
		"CLEAR_FEATURE_TO_ENDPOINT",

		"GET_STATUS_FROM_DEVICE",
		"GET_STATUS_FROM_INTERFACE",
		"GET_STATUS_FROM_ENDPOINT",

		"RESERVED_0X0016",       

		"VENDOR_DEVICE",
		"VENDOR_INTERFACE",
		"VENDOR_ENDPOINT",

		"CLASS_DEVICE",
		"CLASS_INTERFACE",
		"CLASS_ENDPOINT",

		"RESERVE_0X001D",

		"SYNC_RESET_PIPE_AND_CLEAR_STALL",

		"CLASS_OTHER",
		"VENDOR_OTHER",

		"GET_STATUS_FROM_OTHER",

		"CLEAR_FEATURE_TO_OTHER",
		"SET_FEATURE_TO_OTHER",

		"GET_DESCRIPTOR_FROM_ENDPOINT",
		"SET_DESCRIPTOR_TO_ENDPOINT",

		"GET_CONFIGURATION",
		"GET_INTERFACE",

		"GET_DESCRIPTOR_FROM_INTERFACE",
		"SET_DESCRIPTOR_TO_INTERFACE",

		"GET_MS_FEATURE_DESCRIPTOR",

		"RESERVE_0X002B",
		"RESERVE_0X002C",
		"RESERVE_0X002D",
		"RESERVE_0X002E",
		"RESERVE_0X002F",

		"SYNC_RESET_PIPE",
		"SYNC_CLEAR_STALL",
		"CONTROL_TRANSFER_EX",

		"RESERVE_0X0033",
		"RESERVE_0X0034 ",                 

		"OPEN_STATIC_STREAMS",
		"CLOSE_STATIC_STREAMS",
		"BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL",
		"ISOCH_TRANSFER_USING_CHAINED_MDL",

		"RESERVE_0X0039",
		"RESERVE_0X003A",        
		"RESERVE_0X003B",        
		"RESERVE_0X003C",        

		"GET_ISOCH_PIPE_TRANSFER_PATH_DELAYS"
	};

	return function >= 0 && function < ARRAYSIZE(v) ? v[function] : "URB_FUNCTION_?";
}

const char *dbg_usbip_hdr(char *buf, size_t len, const usbip_header *hdr, bool setup_packet)
{
	if (!hdr) {
		return "usbip_header{null}";
	}

	auto result = buf;

	auto st = RtlStringCbPrintfExA(buf, len, &buf, &len, 0, "{seqnum %u, devid %#x, %s[%u]}, ",
					hdr->seqnum, 
					hdr->devid,			
					hdr->direction == USBIP_DIR_OUT ? "out" : "in",
					hdr->ep);

	if (st != STATUS_SUCCESS) {
		return "dbg_usbip_hdr error";
	}

	switch (hdr->command) {
	case USBIP_CMD_SUBMIT:
		print_cmd_submit(buf, len, &hdr->cmd_submit, setup_packet);
		break;
	case USBIP_RET_SUBMIT:
		print_ret_submit(buf, len, &hdr->ret_submit);
		break;
	case USBIP_CMD_UNLINK:
		RtlStringCbPrintfA(buf, len, "cmd_unlink: seqnum %u", hdr->cmd_unlink.seqnum);
		break;
	case USBIP_RET_UNLINK:
		RtlStringCbPrintfA(buf, len, "ret_unlink: status %d", hdr->ret_unlink.status);
		break;
	default:
		RtlStringCbPrintfA(buf, len, "command %u", hdr->command);
	}

	return result;
}

const char *usb_setup_pkt_str(char *buf, size_t len, const void *packet)
{
	auto r  = static_cast<const USB_DEFAULT_PIPE_SETUP_PACKET*>(packet);

	auto st = RtlStringCbPrintfA(buf, len, 
					"{%s|%s|%s, %s(%#02hhx), wValue %#04hx, wIndex %#04hx, wLength %#04hx(%d)}",
					bmrequest_dir(r->bmRequestType),
					bmrequest_type(r->bmRequestType),
					bmrequest_recipient(r->bmRequestType),
					brequest_str(r->bRequest),
					r->bRequest,
					r->wValue.W,
					r->wIndex.W, 
					r->wLength,
					r->wLength);

	return st != STATUS_INVALID_PARAMETER ? buf : "usb_setup_pkt_str invalid parameter";
}

const char* usbd_transfer_flags(char *buf, size_t len, ULONG TransferFlags)
{
	auto dir = IsTransferDirectionOut(TransferFlags) ? "OUT" : "IN";

	auto st = RtlStringCbPrintfA(buf, len, "%s%s%s%s", dir,
					TransferFlags & USBD_SHORT_TRANSFER_OK ? "|SHORT_OK" : "",
					TransferFlags & USBD_START_ISO_TRANSFER_ASAP ? "|ISO_ASAP" : "",
					TransferFlags & USBD_DEFAULT_PIPE_TRANSFER ? "|DEFAULT_PIPE" : "");

	return st != STATUS_INVALID_PARAMETER ? buf : "usbd_transfer_flags invalid parameter";
}

