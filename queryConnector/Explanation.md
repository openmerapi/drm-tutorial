# Linux DRM queryConnector program documentation.

This document provides a detailed explanation of the DRM (Direct Rendering Manager) connector query program, focusing on the system calls, IOCTLs, and data structures used to interact with the Linux graphics subsystem.

## Overview of DRM

DRM (Direct Rendering Manager) is a subsystem of the Linux kernel responsible for interfacing with GPUs. It provides a user-space API that allows applications to communicate with graphics hardware in a unified way. The DRM subsystem is essential for:

- Managing display outputs
- Handling mode setting
- Controlling hardware acceleration
- Managing memory for graphic buffers

## Key IOCTLs Used in the Program

The program uses two primary IOCTL calls to communicate with the DRM subsystem:

### 1. `DRM_IOCTL_MODE_GETRESOURCES`

```c
#define DRM_IOCTL_MODE_GETRESOURCES _IOWR(DRM_IOCTL_BASE, 0xA0, struct drm_mode_card_res)
```

**Purpose**: This IOCTL retrieves basic information about the graphics card resources, including:
- Number of framebuffers
- Number of CRTCs (Cathode Ray Tube Controllers)
- Number of encoders
- Number of connectors
- Minimum and maximum resolution support

**Usage in the program**:
1. First called with empty arrays to get counts of resources
2. Called again with properly allocated arrays to fill in the actual IDs

**Return value**: 0 on success, -1 on failure with `errno` set

**Example from the code**:
```c
// Get resource counts
memset(&res, 0, sizeof(res));
ret = ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res);

// After allocating memory, get actual resource IDs
res.connector_id_ptr = (uint64_t)(uintptr_t)connector_ids;
res.encoder_id_ptr = (uint64_t)(uintptr_t)encoder_ids;
res.crtc_id_ptr = (uint64_t)(uintptr_t)crtc_ids;
res.fb_id_ptr = (uint64_t)(uintptr_t)fb_ids;
ret = ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res);
```

### 2. `DRM_IOCTL_MODE_GETCONNECTOR`

```c
#define DRM_IOCTL_MODE_GETCONNECTOR _IOWR(DRM_IOCTL_BASE, 0xA7, struct drm_mode_get_connector)
```

**Purpose**: This IOCTL retrieves detailed information about a specific connector, including:
- Connection status (connected, disconnected, unknown)
- Connector type (HDMI, DisplayPort, VGA, etc.)
- Available display modes
- Associated encoders
- Physical dimensions
- Properties and their values

**Usage in the program**:
1. First called with empty arrays to get counts of modes, encoders, and properties
2. Called again with properly allocated arrays to fill in the actual data

**Return value**: 0 on success, -1 on failure with `errno` set

**Example from the code**:
```c
// First call to get counts
memset(&conn, 0, sizeof(conn));
conn.connector_id = connector_ids[i];
ret = ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn);

// After allocating memory, get full connector information
conn.modes_ptr = (uint64_t)(uintptr_t)modes;
conn.encoders_ptr = (uint64_t)(uintptr_t)encoders;
conn.props_ptr = (uint64_t)(uintptr_t)props;
conn.prop_values_ptr = (uint64_t)(uintptr_t)prop_values;
ret = ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn);
```

## Important Data Structures

### 1. `struct drm_mode_card_res`

This structure holds information about the graphics card's resources.

```c
struct drm_mode_card_res {
    uint64_t fb_id_ptr;         // Pointer to framebuffer IDs
    uint64_t crtc_id_ptr;       // Pointer to CRTC IDs
    uint64_t connector_id_ptr;  // Pointer to connector IDs
    uint64_t encoder_id_ptr;    // Pointer to encoder IDs
    
    uint32_t count_fbs;         // Number of framebuffers
    uint32_t count_crtcs;       // Number of CRTCs
    uint32_t count_connectors;  // Number of connectors
    uint32_t count_encoders;    // Number of encoders
    
    uint32_t min_width;         // Minimum width supported
    uint32_t max_width;         // Maximum width supported
    uint32_t min_height;        // Minimum height supported
    uint32_t max_height;        // Maximum height supported
};
```

**Important fields**:
- `count_*` fields indicate how many elements exist of each type
- `*_id_ptr` fields are 64-bit pointers to user-allocated arrays where the kernel will store the IDs
- The width/height fields indicate the resolution limits of the graphics hardware

### 2. `struct drm_mode_get_connector`

This structure holds information about a specific display connector.

```c
struct drm_mode_get_connector {
    uint64_t encoders_ptr;      // Pointer to encoder IDs
    uint64_t modes_ptr;         // Pointer to mode information
    uint64_t props_ptr;         // Pointer to property IDs
    uint64_t prop_values_ptr;   // Pointer to property values
    
    uint32_t count_modes;       // Number of modes
    uint32_t count_props;       // Number of properties
    uint32_t count_encoders;    // Number of encoders
    
    uint32_t encoder_id;        // Current encoder ID
    uint32_t connector_id;      // This connector's ID
    uint32_t connector_type;    // Type of connector (HDMI, DP, etc.)
    uint32_t connector_type_id; // Index of this connector (for multiple of same type)
    
    uint32_t connection;        // Current connection status
    uint32_t mm_width;          // Physical width in millimeters
    uint32_t mm_height;         // Physical height in millimeters
    uint32_t subpixel;          // Subpixel order
    
    uint32_t pad;               // Padding
};
```

**Important fields**:
- `connector_id`: Unique identifier for the connector
- `connector_type`: Type of the connector (see connector type definitions)
- `connection`: Connection status (1 = connected, 2 = disconnected, 3 = unknown)
- `count_*` fields indicate how many elements exist of each type
- `*_ptr` fields are 64-bit pointers to user-allocated arrays where the kernel will store the data

### 3. `struct drm_mode_modeinfo`

This structure holds information about a display mode (resolution, refresh rate, etc.).

```c
struct drm_mode_modeinfo {
    uint32_t clock;           // Pixel clock in kHz
    uint16_t hdisplay;        // Horizontal display size
    uint16_t hsync_start;     // Horizontal sync start
    uint16_t hsync_end;       // Horizontal sync end
    uint16_t htotal;          // Horizontal total size
    uint16_t hskew;           // Horizontal skew
    
    uint16_t vdisplay;        // Vertical display size
    uint16_t vsync_start;     // Vertical sync start
    uint16_t vsync_end;       // Vertical sync end
    uint16_t vtotal;          // Vertical total size
    uint16_t vscan;           // Vertical scan
    
    uint32_t vrefresh;        // Refresh rate in Hz
    
    uint32_t flags;           // Mode flags
    uint32_t type;            // Mode type
    char name[32];            // Mode name
};
```

**Important fields**:
- `hdisplay`/`vdisplay`: The width and height of the display in pixels
- `vrefresh`: The refresh rate in Hz
- `name`: A human-readable name for the mode, often including resolution and refresh rate

## Connector Types and Status Codes

### Connector Types

The DRM subsystem defines various connector types that correspond to physical display interfaces:

```c
#define DRM_MODE_CONNECTOR_Unknown      0
#define DRM_MODE_CONNECTOR_VGA          1
#define DRM_MODE_CONNECTOR_DVII         2
#define DRM_MODE_CONNECTOR_DVID         3
#define DRM_MODE_CONNECTOR_DVIA         4
#define DRM_MODE_CONNECTOR_Composite    5
#define DRM_MODE_CONNECTOR_SVIDEO       6
#define DRM_MODE_CONNECTOR_LVDS         7
#define DRM_MODE_CONNECTOR_Component    8
#define DRM_MODE_CONNECTOR_9PinDIN      9
#define DRM_MODE_CONNECTOR_DisplayPort  10
#define DRM_MODE_CONNECTOR_HDMIA        11
#define DRM_MODE_CONNECTOR_HDMIB        12
#define DRM_MODE_CONNECTOR_TV           13
#define DRM_MODE_CONNECTOR_eDP          14
#define DRM_MODE_CONNECTOR_VIRTUAL      15
#define DRM_MODE_CONNECTOR_DSI          16
#define DRM_MODE_CONNECTOR_DPI          17
#define DRM_MODE_CONNECTOR_WRITEBACK    18
#define DRM_MODE_CONNECTOR_SPI          19
#define DRM_MODE_CONNECTOR_USB          20
```

For example, `DRM_MODE_CONNECTOR_HDMIA` (value 11) represents an HDMI Type A connector.

### Connection Status

The connection status indicates whether a display is connected to a particular connector:

```c
#define DRM_MODE_CONNECTED          1
#define DRM_MODE_DISCONNECTED       2
#define DRM_MODE_UNKNOWNCONNECTION  3
```

- `DRM_MODE_CONNECTED` (1): A display is connected
- `DRM_MODE_DISCONNECTED` (2): No display is connected
- `DRM_MODE_UNKNOWNCONNECTION` (3): Connection status cannot be determined

## Memory Management and Pointers

One of the key challenges in working with the DRM API is proper memory management. The program uses a specific pattern for interacting with the DRM kernel subsystem:

1. **First call to get counts**: Initialize a structure with zeros and call the IOCTL to retrieve the count of resources
2. **Allocate memory**: Use the counts to allocate properly sized memory buffers
3. **Set pointers**: Set the pointer fields in the structure to point to the allocated buffers
4. **Second call to get data**: Call the IOCTL again to retrieve the actual data

This pattern is used for both `DRM_IOCTL_MODE_GETRESOURCES` and `DRM_IOCTL_MODE_GETCONNECTOR`.

### Pointer Casting

The program uses a specific cast to convert between user-space pointers and the 64-bit values expected by the kernel:

```c
res.connector_id_ptr = (uint64_t)(uintptr_t)connector_ids;
```

This ensures compatibility across 32-bit and 64-bit architectures.

## Error Handling

The program implements robust error handling:

1. **IOCTL return values**: Checks the return value of each IOCTL call and reports errors
2. **Memory allocation**: Verifies that memory allocation succeeds before proceeding
3. **File handling**: Properly opens and closes the DRM device file
4. **Memory cleanup**: Frees all allocated memory, even in error cases

## System Access Requirements

Accessing DRM devices typically requires special permissions:

1. **File permissions**: The user needs read/write permissions for the DRM device files (usually `/dev/dri/card*`)
2. **Group membership**: The user should be a member of the `video` group
3. **Root privileges**: Some operations may require root privileges
4. **Display server conflicts**: If a display server (X11, Wayland) is running, it may have exclusive access to the DRM devices

## Additional Notes for Programming with DRM

1. **Atomic Mode Setting**: Newer DRM APIs support atomic mode setting, which allows for changing multiple display parameters simultaneously
2. **KMS (Kernel Mode Setting)**: The mode setting functionality is implemented in the kernel
3. **GEM (Graphics Execution Manager)**: Manages graphics memory
4. **libdrm**: While this program avoids external libraries, libdrm provides a more convenient API for interacting with the DRM subsystem
5. **DRM Authentication**: Some operations require DRM master privileges, which usually belong to the display server

## Conclusion

This program demonstrates direct interaction with the Linux DRM subsystem using only system calls and IOCTL operations. Understanding these low-level details is valuable for developers working on:

- Display servers
- Compositors
- Graphics drivers
- Video playback applications
- Any application that needs direct control over display hardware

The DRM subsystem continues to evolve, with newer versions offering more features and better performance. However, the basic principles and structures explained here remain relevant.
