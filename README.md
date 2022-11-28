# urwg2tool

`urwg2tool` wants to become a tool that enables access to the internal
storage of
[RØDE Wireless Go II TX](https://rode.com/microphones/wireless/wirelessgoii) 
devices without using the
[RØDE Central](https://rode.com/software/rode-central) application.

It is mainly developed for Linux at the moment.

# Status
`urwg2tool` is currently under development. It does not work yet.

# Build
## Dependencies
Install `libusb-dev` and `libusb`, potentially called `libusb-1.0-0`
or similar. (You already have `gcc`, right?)

## Compile
`gcc main.c -lusb-1.0`

## Run
Depending on your exact OS and configuration, your user account needs
to be in the `plugdev` group and/or a `udev` rule is needed for USB
device `19f7:002d` and/or you need to run `urwg2tool` with `sudo`.

# Further reading
See [DEVELOPMENT.md](DEVELOPMENT.md)
