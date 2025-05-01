On the Inspiron 5459  there are two cards and I got this output.
So why is the first GPU not connected to any connector ?

```bash
# ./queryConnector /dev/dri/card1 
Successfully opened DRM device: /dev/dri/card1 (fd=3)
Resource counts: connectors=0, encoders=0, crtcs=0, fbs=0
DRM Device: /dev/dri/card1
Found 0 connector(s)

# ./queryConnector /dev/dri/card2
Successfully opened DRM device: /dev/dri/card2 (fd=3)
Resource counts: connectors=2, encoders=2, crtcs=3, fbs=0
DRM Device: /dev/dri/card2
Found 2 connector(s)

Connector 95: found 2 modes, 10 props, 1 encoders
  [DEBUG] connector_id: 0x5f, connector_type: 0xe
  [DEBUG] pointers - modes: 0x0, encoders: 0x0
Connector 95:
  Type: eDP-1
  Status: Connected
  Physical Size: 310x170 mm
  Encoders: 1
  Encoder IDs: 94
  Modes: 2
  Available Modes:
    1366x768: 1366x768@60Hz
    1366x768: 1366x768@48Hz

Connector 103: found 0 modes, 12 props, 1 encoders
  [DEBUG] connector_id: 0x67, connector_type: 0xb
  [DEBUG] pointers - modes: 0x0, encoders: 0x0
Connector 103:
  Type: HDMI-A-1
  Status: Disconnected
  Physical Size: 0x0 mm
  Encoders: 1
  Encoder IDs: 102
  Modes: 0
```


```bash
# more /sys/class/drm/card1/device/vendor 
0x1002
# more /sys/class/drm/card1/device/device
0x6660
# more /sys/class/drm/card2/device/vendor 
0x8086
# more /sys/class/drm/card2/device/device
0x1916
```



Based on the additional information you've provided, I can give you a more specific analysis of your hybrid graphics setup:

## Analysis of Your DRM Devices

### Card1 (`/dev/dri/card1`)
```
Vendor ID: 0x1002 (AMD)
Device ID: 0x6660 (Radeon HD 8600/8700M or R7 M370)
Resource counts: connectors=0, encoders=0, crtcs=0, fbs=0
```
This is an AMD Radeon discrete GPU. The device ID 0x6660 corresponds to a mid-range mobile graphics chip in the AMD Radeon HD 8000M series or R7 series. Despite being a capable GPU, it shows zero connectors, encoders, and CRTCs in the DRM output.

### Card2 (`/dev/dri/card2`)
```
Vendor ID: 0x8086 (Intel)
Device ID: 0x1916 (HD Graphics 530, part of 6th Gen Intel Core "Skylake")
Resource counts: connectors=2, encoders=2, crtcs=3, fbs=0
Found 2 connector(s)
```
This is an Intel HD Graphics 530 integrated GPU. It's physically connected to all display outputs - your internal laptop eDP display and the HDMI port.

## What This Means

Your Inspiron 5459 has a hybrid graphics configuration with:

1. **Intel HD Graphics 530** (integrated into your CPU): This handles the actual display outputs through the connectors you see in the output. It's a 6th generation Intel GPU that comes with Skylake processors.

2. **AMD Radeon R7 M370** (or similar model): This discrete GPU provides additional graphics processing power but doesn't connect directly to any displays. 

This setup is known as **AMD Hybrid Graphics** (similar to NVIDIA's Optimus). In this architecture:

- The Intel GPU is always responsible for driving the displays
- The AMD GPU can render frames for demanding applications (games, 3D software)
- Rendered frames from the AMD GPU must be copied to the Intel GPU's memory for display
- This process is managed by drivers through technologies like PRIME or VFIO

## Why Your AMD GPU Shows No Connectors

In your specific laptop design:

1. All physical display connections (eDP for the laptop screen and HDMI for external displays) are hardwired to the Intel GPU
2. The AMD GPU has no direct path to any display connectors
3. The AMD GPU must work through the Intel GPU to display its output

This design brings several advantages:
- Power saving (the AMD GPU can be powered down when not needed)
- Simplified display management (only one GPU handles monitor detection, HDMI audio, etc.)
- The ability to switch between GPUs based on performance needs

The zero connectors on card1 confirms this is a "muxless" design, where there's no hardware multiplexer to switch which GPU drives the displays - it's always the Intel GPU that physically connects to the screens.
