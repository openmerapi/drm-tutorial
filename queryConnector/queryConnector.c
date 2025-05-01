#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>

// DRM connector definitions based on drm.h and drm_mode.h
// We're defining these directly since we're not using libdrm

#define DRM_IOCTL_BASE                  'd'
#define DRM_IOCTL_MODE_GETRESOURCES     _IOWR(DRM_IOCTL_BASE, 0xA0, struct drm_mode_card_res)
#define DRM_IOCTL_MODE_GETCONNECTOR     _IOWR(DRM_IOCTL_BASE, 0xA7, struct drm_mode_get_connector)

// Connection status
#define DRM_MODE_CONNECTED          1
#define DRM_MODE_DISCONNECTED       2
#define DRM_MODE_UNKNOWNCONNECTION  3

// Connector types
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

struct drm_mode_modeinfo {
    uint32_t clock;
    uint16_t hdisplay;
    uint16_t hsync_start;
    uint16_t hsync_end;
    uint16_t htotal;
    uint16_t hskew;
    uint16_t vdisplay;
    uint16_t vsync_start;
    uint16_t vsync_end;
    uint16_t vtotal;
    uint16_t vscan;
    
    uint32_t vrefresh;
    
    uint32_t flags;
    uint32_t type;
    char name[32];
};

struct drm_mode_card_res {
    uint64_t fb_id_ptr;
    uint64_t crtc_id_ptr;
    uint64_t connector_id_ptr;
    uint64_t encoder_id_ptr;
    uint32_t count_fbs;
    uint32_t count_crtcs;
    uint32_t count_connectors;
    uint32_t count_encoders;
    uint32_t min_width;
    uint32_t max_width;
    uint32_t min_height;
    uint32_t max_height;
};

struct drm_mode_get_connector {
    uint64_t encoders_ptr;
    uint64_t modes_ptr;
    uint64_t props_ptr;
    uint64_t prop_values_ptr;
    
    uint32_t count_modes;
    uint32_t count_props;
    uint32_t count_encoders;
    
    uint32_t encoder_id;
    uint32_t connector_id;
    uint32_t connector_type;
    uint32_t connector_type_id;
    
    uint32_t connection;
    uint32_t mm_width;
    uint32_t mm_height;
    uint32_t subpixel;
    
    uint32_t pad;
};

// Get string representation of connector type
const char* get_connector_type_name(uint32_t connector_type) {
    switch (connector_type) {
        case DRM_MODE_CONNECTOR_Unknown: return "Unknown";
        case DRM_MODE_CONNECTOR_VGA: return "VGA";
        case DRM_MODE_CONNECTOR_DVII: return "DVI-I";
        case DRM_MODE_CONNECTOR_DVID: return "DVI-D";
        case DRM_MODE_CONNECTOR_DVIA: return "DVI-A";
        case DRM_MODE_CONNECTOR_Composite: return "Composite";
        case DRM_MODE_CONNECTOR_SVIDEO: return "S-Video";
        case DRM_MODE_CONNECTOR_LVDS: return "LVDS";
        case DRM_MODE_CONNECTOR_Component: return "Component";
        case DRM_MODE_CONNECTOR_9PinDIN: return "9-Pin DIN";
        case DRM_MODE_CONNECTOR_DisplayPort: return "DisplayPort";
        case DRM_MODE_CONNECTOR_HDMIA: return "HDMI-A";
        case DRM_MODE_CONNECTOR_HDMIB: return "HDMI-B";
        case DRM_MODE_CONNECTOR_TV: return "TV";
        case DRM_MODE_CONNECTOR_eDP: return "eDP";
        case DRM_MODE_CONNECTOR_VIRTUAL: return "Virtual";
        case DRM_MODE_CONNECTOR_DSI: return "DSI";
        case DRM_MODE_CONNECTOR_DPI: return "DPI";
        case DRM_MODE_CONNECTOR_WRITEBACK: return "Writeback";
        case DRM_MODE_CONNECTOR_SPI: return "SPI";
        case DRM_MODE_CONNECTOR_USB: return "USB";
        default: return "Unknown";
    }
}

// Get string representation of connection status
const char* get_connection_status(uint32_t connection) {
    switch (connection) {
        case DRM_MODE_CONNECTED: return "Connected";
        case DRM_MODE_DISCONNECTED: return "Disconnected";
        case DRM_MODE_UNKNOWNCONNECTION: return "Unknown";
        default: return "Invalid";
    }
}

int main(int argc, char *argv[]) {
    int fd, ret;
    struct drm_mode_card_res res = {0};
    
    // Initialize all pointers to NULL for safety
    uint32_t *connector_ids = NULL;
    uint32_t *encoder_ids = NULL;
    uint32_t *crtc_ids = NULL;
    uint32_t *fb_ids = NULL;
    
    // Check for correct usage
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <DRM device path>\n", argv[0]);
        fprintf(stderr, "Example: %s /dev/dri/card0\n", argv[0]);
        return 1;
    }

    // Open the DRM device - use only O_RDWR to match kernel expectations
    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open DRM device '%s': %s\n", 
                argv[1], strerror(errno));
        return 1;
    }
    
    printf("Successfully opened DRM device: %s (fd=%d)\n", argv[1], fd);

    // First, get the resource counts
    memset(&res, 0, sizeof(res));
    ret = ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res);
    if (ret) {
        fprintf(stderr, "Failed to get DRM resources: %s\n", strerror(errno));
        close(fd);
        return 1;
    }
    
    printf("Resource counts: connectors=%d, encoders=%d, crtcs=%d, fbs=%d\n", 
           res.count_connectors, res.count_encoders, res.count_crtcs, res.count_fbs);

    // Allocate memory for connector IDs, crtc IDs, encoder IDs, etc.
    if (res.count_connectors > 0) {
        connector_ids = malloc(res.count_connectors * sizeof(uint32_t));
        if (!connector_ids) {
            fprintf(stderr, "Failed to allocate memory for connector IDs\n");
            close(fd);
            return 1;
        }
    }

    if (res.count_encoders > 0) {
        encoder_ids = malloc(res.count_encoders * sizeof(uint32_t));
        if (!encoder_ids) {
            fprintf(stderr, "Failed to allocate memory for encoder IDs\n");
            free(connector_ids);
            close(fd);
            return 1;
        }
    }

    if (res.count_crtcs > 0) {
        crtc_ids = malloc(res.count_crtcs * sizeof(uint32_t));
        if (!crtc_ids) {
            fprintf(stderr, "Failed to allocate memory for CRTC IDs\n");
            free(connector_ids);
            free(encoder_ids);
            close(fd);
            return 1;
        }
    }

    if (res.count_fbs > 0) {
        fb_ids = malloc(res.count_fbs * sizeof(uint32_t));
        if (!fb_ids) {
            fprintf(stderr, "Failed to allocate memory for FB IDs\n");
            free(connector_ids);
            free(encoder_ids);
            free(crtc_ids);
            close(fd);
            return 1;
        }
    }

    // Set the pointers and get the resources again to populate IDs
    res.connector_id_ptr = (uint64_t)(uintptr_t)connector_ids;
    res.encoder_id_ptr = (uint64_t)(uintptr_t)encoder_ids;
    res.crtc_id_ptr = (uint64_t)(uintptr_t)crtc_ids;
    res.fb_id_ptr = (uint64_t)(uintptr_t)fb_ids;

    ret = ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res);
    if (ret) {
        fprintf(stderr, "Failed to get connector IDs: %s\n", strerror(errno));
        free(connector_ids);
        free(encoder_ids);
        free(crtc_ids);
        free(fb_ids);
        close(fd);
        return 1;
    }

    printf("DRM Device: %s\n", argv[1]);
    printf("Found %d connector(s)\n\n", res.count_connectors);

    // Iterate through each connector
    for (uint32_t i = 0; i < res.count_connectors; i++) {
        struct drm_mode_get_connector conn = {0};
        
        // First call to get counts
        memset(&conn, 0, sizeof(conn));
        conn.connector_id = connector_ids[i];
        
        ret = ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn);
        if (ret) {
            fprintf(stderr, "Failed to get connector %d info: %s\n", 
                    connector_ids[i], strerror(errno));
            continue;
        }
        
        printf("Connector %d: found %d modes, %d props, %d encoders\n", 
               conn.connector_id, conn.count_modes, conn.count_props, conn.count_encoders);
               
        // Debug hex dumps to help identify potential issues
        printf("  [DEBUG] connector_id: 0x%x, connector_type: 0x%x\n", 
               conn.connector_id, conn.connector_type);
        printf("  [DEBUG] pointers - modes: 0x%" PRIx64 ", encoders: 0x%" PRIx64 "\n",
               conn.modes_ptr, conn.encoders_ptr);

        // Allocate memory for modes, encoders, and property info
        struct drm_mode_modeinfo *modes = NULL;
        uint32_t *encoders = NULL;
        uint32_t *props = NULL;
        uint64_t *prop_values = NULL;
        
        if (conn.count_modes > 0) {
            modes = calloc(conn.count_modes, sizeof(struct drm_mode_modeinfo));
            if (!modes) {
                fprintf(stderr, "Failed to allocate memory for modes\n");
                continue;
            }
        }
        
        if (conn.count_encoders > 0) {
            encoders = calloc(conn.count_encoders, sizeof(uint32_t));
            if (!encoders) {
                fprintf(stderr, "Failed to allocate memory for encoders\n");
                free(modes);
                continue;
            }
        }
        
        if (conn.count_props > 0) {
            props = calloc(conn.count_props, sizeof(uint32_t));
            if (!props) {
                fprintf(stderr, "Failed to allocate memory for properties\n");
                free(modes);
                free(encoders);
                continue;
            }
            
            prop_values = calloc(conn.count_props, sizeof(uint64_t));
            if (!prop_values) {
                fprintf(stderr, "Failed to allocate memory for property values\n");
                free(modes);
                free(encoders);
                free(props);
                continue;
            }
        }
        
        // Set all pointers
        conn.modes_ptr = (uint64_t)(uintptr_t)modes;
        conn.encoders_ptr = (uint64_t)(uintptr_t)encoders;
        conn.props_ptr = (uint64_t)(uintptr_t)props;
        conn.prop_values_ptr = (uint64_t)(uintptr_t)prop_values;

        // Second call to get the full connector information
        ret = ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn);
        if (ret) {
            fprintf(stderr, "Failed to get connector data: %s\n", strerror(errno));
            free(modes);
            free(encoders);
            continue;
        }

        // Print connector information
        printf("Connector %d:\n", conn.connector_id);
        printf("  Type: %s-%d\n", 
               get_connector_type_name(conn.connector_type), 
               conn.connector_type_id);
        printf("  Status: %s\n", get_connection_status(conn.connection));
        printf("  Physical Size: %dx%d mm\n", conn.mm_width, conn.mm_height);
        printf("  Encoders: %d\n", conn.count_encoders);
        
        if (conn.count_encoders > 0) {
            printf("  Encoder IDs: ");
            for (uint32_t j = 0; j < conn.count_encoders; j++) {
                printf("%d ", encoders[j]);
            }
            printf("\n");
        }
        
        printf("  Modes: %d\n", conn.count_modes);
        
        // Print modes if available and connected
        if (conn.connection == DRM_MODE_CONNECTED && conn.count_modes > 0) {
            printf("  Available Modes:\n");
            for (uint32_t j = 0; j < conn.count_modes; j++) {
                printf("    %s: %dx%d@%dHz\n", 
                       modes[j].name,
                       modes[j].hdisplay, 
                       modes[j].vdisplay,
                       modes[j].vrefresh);
            }
        }
        
        printf("\n");
        
        // Clean up all allocated resources
        free(modes);
        free(encoders);
        free(props);
        free(prop_values);
    }

    // Clean up all resources
    free(connector_ids);
    free(encoder_ids);
    free(crtc_ids);
    free(fb_ids);
    close(fd);
    
    return 0;
}
