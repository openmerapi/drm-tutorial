# How to compile and run the program

queryConnector takes the first argument /dev/dri/cardX where X=0,1,...
On some systems you will find more than one card. For example on one laptop
I found /dev/dri/card1 and /dev/dri/card2. 

The following output is from another laptop that has only one GPU and three
connectors.

- **`eDP`** (Embedded DisplayPort): **Internal laptop display** (the built-in screen).
- **`DP`** (DisplayPort): External DisplayPort connector. (USB-C in DP Alt mode).
- **`HDMI-A`** (HDMI Type-A): External HDMI port.
  

```bash
$ gcc -o queryConnector queryConnector.c 

$ su
Password: 

# ./queryConnector /dev/dri/card1 
Successfully opened DRM device: /dev/dri/card1 (fd=3)
Resource counts: connectors=3, encoders=7, crtcs=4, fbs=0
DRM Device: /dev/dri/card1
Found 3 connector(s)

Connector 241: found 2 modes, 12 props, 1 encoders
  [DEBUG] connector_id: 0xf1, connector_type: 0xe
  [DEBUG] pointers - modes: 0x0, encoders: 0x0
Connector 241:
  Type: eDP-1
  Status: Connected
  Physical Size: 340x190 mm
  Encoders: 1
  Encoder IDs: 240 
  Modes: 2
  Available Modes:
    1920x1080: 1920x1080@120Hz
    1920x1080: 1920x1080@60Hz

Connector 250: found 0 modes, 14 props, 1 encoders
  [DEBUG] connector_id: 0xfa, connector_type: 0xb
  [DEBUG] pointers - modes: 0x0, encoders: 0x0
Connector 250:
  Type: HDMI-A-1
  Status: Disconnected
  Physical Size: 0x0 mm
  Encoders: 1
  Encoder IDs: 249 
  Modes: 0

Connector 259: found 0 modes, 14 props, 1 encoders
  [DEBUG] connector_id: 0x103, connector_type: 0xa
  [DEBUG] pointers - modes: 0x0, encoders: 0x0
Connector 259:
  Type: DisplayPort-1
  Status: Disconnected
  Physical Size: 0x0 mm
  Encoders: 1
  Encoder IDs: 258 
  Modes: 0
```
