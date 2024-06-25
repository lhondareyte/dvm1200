# Velleman DVM1200 interface for *NIX

The [Velleman DVM1200](https://www.velleman.eu/products/view/?id=372236) is a 
cheap digital multimeter (DM) with usb interface. There are many clones 
from differentes china sources. This utility read data from the DM interface 
and convert it to CSV format. This can be usefull for generate graphs and so on.

This is an adaptation of Sabin Iacob's `DVM1200.py` script, available [here](https://gist.github.com/m0n5t3r/4543215). 

This utility has no dependency and should works on any ***NIX** system.

## Installation : 

To install the utility, type:

    git clone https://github.com/lhondareyte/dvm1200.git
    cd dvm1200 && make && make install

**Warning, you must have permissions to access the serial port.**

See the man page for all available options.

## How it works :

The following documentation is mostly taken from Sabin Iacob script:

The multimeter is as dumb as they come when it comes to PC interfacing, 
and makes a screen dump to USB (over an IR link) instead of providing proper 
ASCII data. The original software is Windows only and doesn't work in Wine.
The output consists of 15 bytes packet that look like this when converted to hex:

    1b 27 3d 4d 5b 65 7b 87 9e a0 b0 c0 d4 e0 f1

* Each packet end with f1
* 4 MSB of each byte is a sequence number (`1 to f`) and can be ignored, leaving us with

    b 7 d d b 5 b 7 e 0 0 0 4 0

  Further, these can be grouped like

    b 7d db 5b 7e 00 04 0

These encode various things on the screen; in order of appearance:

* left side flags (AC, DC, PC-Link, AUTO)
* first digit
* second digit
* third digit
* fourth digit
* mostly scaling information (nano, micro, milli, etc.), but also diode, beep
* mostly units (V, A, F, Hz), but also REL, H(old)
* top flags: MIN, MAX, but also deg. C

The last half byte looks like [MIN, <no idea>, deg.C, MAX]. The digits use a non-standard layout (check decode_digit), but are basically seven segment
displays; the "decimal point" bit of the leftmost digit controls the sign display.

## Notes :

The DM interface is based on a PL2303 chip from PROLIFIC. The driver is available 
with any Linux distributions and all *BSD.

On macOS, you must install the driver from PROLIFIC. Note that this driver can be buggy: I recommend pl2303 driver from REPLEO but it's not free.

For FreeBSD users, a port is [available here](https://github.com/lhondareyte/ports/tree/master/dvm1200).

## License :

MIT license.
