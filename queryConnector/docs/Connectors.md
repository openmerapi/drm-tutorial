# The Relationship Between CRTCs and Connectors

## Introduction

This document explains the relationship between CRTCs and Connectors in the Linux DRM (Direct Rendering Manager) subsystem and how they work together to drive displays in a modern graphics system.

## Core DRM Display Components

### CRTC (Cathode Ray Tube Controller)

A CRTC is a hardware resource that generates and controls the display signal. Despite the name referencing older CRT technology, CRTCs remain essential components in modern display pipelines.

**Key responsibilities of a CRTC:**

* Controls pixel timing signals sent to displays
* Manages framebuffers containing image data
* Controls refresh rates and resolutions
* Handles vertical and horizontal synchronization
* Performs hardware-level scaling and color transformations

Think of a CRTC as a *signal generator* that produces a specific video signal with particular timing characteristics for one display output.

### Connector

A connector represents a physical output port on your graphics hardware.

**Common connector types:**

* HDMI ports
* DisplayPort outputs
* VGA connectors
* DVI connectors
* eDP (embedded DisplayPort for laptop screens)

A connector can be in one of three states:

* Connected (a display is attached)
* Disconnected (no display attached)
* Unknown connection status

### Encoder

An encoder sits between a CRTC and a connector. It converts the CRTC's signal into a format appropriate for a specific type of connector.

**Functions of an encoder:**

* Signal conversion between internal formats and output standards
* Protocol handling for digital outputs like HDMI and DisplayPort
* Supporting display-specific features like audio over HDMI

## The Display Pipeline

The Linux DRM display pipeline follows this path:

```
Framebuffer → CRTC → Encoder → Connector → Physical Display
```

This unidirectional pipeline explains several key constraints in the system:

1. A single CRTC can drive only one encoder at a time
2. An encoder is typically compatible with only certain types of connectors
3. A connector can receive signal from only one encoder at a time
4. Therefore, a CRTC ultimately drives only one connector (and thus one display) at a time

## Typical Hardware Configurations

### Resource Counts

On most modern graphics cards:

* **CRTCs**: Most GPUs have 3-6 CRTCs
* **Connectors**: Often more physical connectors than CRTCs
* **Encoders**: Varies, but generally at least one per supported output type

For example, this system configuration:
```
Resource counts: connectors=3, encoders=7, crtcs=4, fbs=0
```

Indicates:

* 4 CRTCs (can drive up to 4 independent displays)
* 3 physical connectors (output ports)
* 7 encoders (various signal types that can be routed to the connectors)

### Practical Limitations

The number of CRTCs typically determines how many independent displays your graphics hardware can drive simultaneously, regardless of how many physical connectors are available.

For example, if your graphics card has 6 output connectors but only 3 CRTCs, you can only use 3 displays at once, despite having more physical ports.

## Key Relationships

### Can a Connector Be Connected to Multiple CRTCs?

No, a connector cannot be simultaneously connected to multiple CRTCs. The display pipeline flows in only one direction, and a connector can only receive signal from one encoder (which is connected to one CRTC) at any given time.

### Can a CRTC Drive Multiple Connectors?

No, a CRTC can only be connected to one encoder at a time in the standard display pipeline. However, some specialized hardware implementations may support cloning or spanning capabilities at the hardware level.

### Dynamic Reassignment

While the one-to-one relationship is a constraint at any moment in time, the DRM subsystem allows for dynamic reassignment:

* CRTCs can be reassigned to different encoders
* Encoders can be reconnected to different connectors
* These changes happen during mode setting operations

## Programming Considerations

When working with the DRM API directly:

1. First enumerate all available resources (CRTCs, encoders, connectors)
2. For each active connector, find a compatible encoder
3. For each encoder, find an available CRTC
4. Configure the CRTC with the desired mode (resolution, refresh rate)
5. Attach a framebuffer to the CRTC

This process is typically handled by display servers (X11, Wayland) or graphics libraries (libdrm), but understanding the underlying model is crucial for low-level graphics programming.

## Conclusion

The relationship between CRTCs and connectors is mediated by encoders in a unidirectional pipeline. The number of CRTCs typically constrains how many displays can be simultaneously active, regardless of how many physical connectors are available.

Understanding this relationship is essential for working with the Linux DRM subsystem and developing graphics applications that interact directly with display hardware.
