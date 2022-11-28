#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

// The bRequest we send
#define SET_REPORT 0x09

// The structure of the REPORTs we send to the device
struct report {
	uint16_t length;
	char data[32];
};

struct report reports_list[] = {
	{ .length = 17, .data = {
		0x01, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00}
	},
	{ .length = 17, .data = {
		0x01, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00}
	},
	{ .length = 28, .data = {
		0x09, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00}
	},
	{ .length = 28, .data = {
		0x09, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00}
	},
	// Sentinel for the end of the array
	{ .length = -1, .data = {} }
};

void iserror(char* where, int what) {
	if (what != 0) {
		printf("%s error %d %s\n", where, what, libusb_error_name(what));
		exit(-1);
	}
}

int main(int argc, char* argv) {
	libusb_device_handle* h;
	libusb_context* ctx = NULL; // use default context for now
	int r=0; // to catch various return values
	
	uint8_t bmRequestType = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE; // 0x21
	uint16_t wValue = 0x0209; // ID 9, Type 2 (output)
	uint16_t wLength = 28;
	int transferred;
	char data[256];

	int i=0;

	r = libusb_init(&ctx);
	iserror("init", r);
	r = libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL,
			LIBUSB_LOG_LEVEL_WARNING);
	iserror("set opt", r);

	h = libusb_open_device_with_vid_pid(
			ctx, (uint16_t)0x19f7, (uint16_t)0x002d);
	iserror("open dev", h==NULL);

	if (libusb_kernel_driver_active(h, 0)) {
		libusb_detach_kernel_driver(h, 0);
	}

	for (int i=0; reports_list[i].length != (uint16_t)-1; i++) {
		struct report rtbl = reports_list[i];
		printf("Doing %d with %x\n", i, rtbl.data[1]);
		printf("Len is %d\n", rtbl.length);
		r = libusb_control_transfer(h, bmRequestType, SET_REPORT,
				wValue, 0, rtbl.data,
				rtbl.length, 0);
		// iserror("ctrl xfer", r);
		printf("ctrl xferred %d bytes\n", r);

		r = libusb_interrupt_transfer(h, 0x82, data, sizeof(data),
				&transferred, 0);
		iserror("intr xfer", r);
		printf("intr xferred %d bytes\n", transferred);
	}
	printf("there\n");
	libusb_attach_kernel_driver(h, 0);
	libusb_close(h);
	libusb_exit(ctx);
	
	return 0;
}
