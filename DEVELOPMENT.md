# Development

This file documents the overall process of putting together `urwg2tool`,
a (minimal) tool to interface with RØDE Wireless Go II transmitter
devices without resorting to the official RØDE application.

The device is identified using `lsusb` with a vendor and product
ID of `19f7:002d`.


# USB traffic sniffing

## RØDE Central

[RØDE Central](https://rode.com/en/software/rode-central) is an app
that lets you manage Wireless Go II devices via USB on Windows and
macOS. We sniffed its traffic on a Windows machine using
[Wireshark](https://wireshark.com/) with the fine
[WinPcap](https://www.winpcap.org/) packet capture library.

This trace documents attaching the Wireless Go II device and starting
the application. The application sends special USB commands to the
device to enable its Mass Storage interface. Now, the files on the
device are visible (in Windows Explorer as well as in the application).

## Linux

For comparison, we also sniffed the USB traffic when connecting the
device to a Linux machine. Since the RØDE Central application is not
available on this platform, the device does not expose its Mass
Storage interface.

(Other traffic for querying the device ID etc. is similar between
platforms, naturally.)


# USB commands

The Windows USB traffic contains USB HID `SET_REPORT` requests from
the application to the device, and these carry a few dozen data bytes.
After each request-response pair, the device sends a few dozen data
bytes back to the application using `URB_INTERRUPT` data transfer.

Data looks similar both among `SET_REPORT`s themselves (most of the
time, only one byte changes), and also across these reports and the
`URB_INTERRUPT` transfers (said changing byte is reflected there, and
a few other changing bytes always take the same positions in the data).

We have no idea about the meaning of the data yet. A trivial (?)
thing to try would be to replay exactly the captured values and
check the device's responses.

## No gain: `usb_modeswitch`

One hitherto unsuccessful experiment for this involved the well-known
[`usb_modeswitch` software](https://www.draisberghof.de/usb_modeswitch/).
It sends customizable commands to the device just fine. However, we
could not determine how to send the required command format to the
specific Endpoint on the device (and the fault is definitely ours).

## WIP: `libusb`

The current approach is to write a small program based on
[`libusb`](https://libusb.info/) to re-create the required commands
from scratch and send them to the device. The rough plan is:

* `libusb_init`
* `libusb_set_option` to turn on debugging
* (potentially detach kernel drivers here)
* `libusb_open_device_with_vid_pid`
* `libusb_control_transfer` to send the USB commands
* `libusb_interrupt_transfer` to catch the device's replies
* `libusb_exit`

This already works OK, and we can send the full "magic sequence" of
Reports to the device and receive the expected Interrupt data.

## Commands identified so far

* `SET_REPORT` with data `090101` (and zeros to fill up to 28 bytes)
  causes the device to respond in the Interrupt data with some kind
  of timestamp that changes every second.
  Assumed format:
  - `0a0141` (constant between traces so far)
  - `ss` two least-significant digits for the seconds counter,
  - `SS` two most-significant digits for the seconds coutner
  - `xxxx` (unknown, maybe a day/date). Examples so far:
    `7e63`, `19bc`


# Future work

We are currently concerned with "unlocking" the device's Mass Storage
mode only, so that recordings on the device can be accessed without
the RØDE Central application.

The application [has more features](https://rode.com/en/user-guides/wireless-go-ii/using-rode-central)
for Wireless Go II devices though:
* enable / disable the device's on-board recording function
* configure the recording mode (always / backup / off)
* audio editing
* recognize "marker drops" (i.e. dropouts, but also markers that the user
  sets by pressing the power button)
* export and recode the recordings -- a FLOSS `UGG`-to-`WAV` converter
  [already exists](https://github.com/fukidzon/Rode_WirelessGoII_UGG2wav)
  though
* rename and delete files stored on the device
* configure audio compression for recordings
* access the "safety channel" (secondary recording with -20dB level)
* configure LED brightness
* configure power button mode (mute / marker / none)
* probably update the device firmware

Some of these would be nice to have implemented, but let's not get
ahead of ourselves!

