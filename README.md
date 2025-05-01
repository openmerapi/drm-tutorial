# Linux DRM Examples

This repository contains educational examples that demonstrate direct interaction with the Linux Direct Rendering Manager (DRM) subsystem without using any abstraction libraries such as libdrm, Intel Level Zero, or similar APIs.

## Purpose

The primary aim of this repository is to help students and beginners understand:

- The boundary between kernel space and user space in graphics programming
- How applications communicate directly with graphics hardware through the Linux kernel
- The underlying mechanisms that graphics libraries often abstract away
- Low-level DRM structures and IOCTL calls

By examining these examples, you can gain insight into how graphics hardware is managed at a fundamental level, providing a stronger foundation for understanding higher-level graphics APIs and libraries.

## Examples

### 1. Query Connector

This example demonstrates how to query information about display connectors attached to your system.

**Files:**
- [`queryConnector/queryConnector.c`](queryConnector/queryConnector.c) - Source code for the connector query utility
- [`queryConnector/Explanation.md`](queryConnector/Explanation.md) - Detailed explanation of the code, data structures, and DRM concepts
- [`queryConnector/LICENSE`](queryconnector/LICENSE) - MIT License for this specific example
- [`queryConnector/docs/Compilation.md`](queryConnector/docs/Compilation.md)  
- [`Inspiron 5459 Example`](queryConnector/docs/Inspiron-5459.md)  
- [`Hardware-to-Software Mapping for DRM Abstractions`](queryConnector/docs/hardware-to-software-mapping.md) 
- [`The Relationship Between CRTCs and Connectors`](queryConnector/docs/Connectors.md)  

**Features:**
- Opens and queries a DRM device directly using system calls
- Retrieves information about all available display connectors
- Shows connection status, connector types, and supported display modes
- Demonstrates proper memory management when working with the DRM subsystem

## Future Examples

More examples will be added to this repository in the future, each exploring different aspects of the DRM subsystem. Each example may have its own specific license, so please check the LICENSE file within each example directory.

## Usage

Each example includes compilation instructions and usage information in its respective directory. In general, you will need development packages for the Linux kernel headers to compile these examples.

## Note

These examples are intended for educational purposes. Production applications would typically use libraries like libdrm that provide a more stable and consistent API across different kernel versions and hardware.
