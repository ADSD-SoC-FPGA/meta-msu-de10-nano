/* SPDX-License-Identifier: MIT */
/*-------------------------------------------------------------------------
 * Description: Test program for the combFilterProcessor kernel module
 *
 * This program demonstrates how to interact with the Comb Filter Kernel Module
 * by reading and writing to the device file and sysfs attributes.
 * Written by Zackery Backman
 *-------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/stat.h>

/* Define the module name as seen in /proc/modules */
#ifndef MODULE_NAME
    #define MODULE_NAME "combFilter"
#endif

/* Define the device name for the character device and sysfs */
#ifndef DEVICE_NAME
    #define DEVICE_NAME "combFilterProcessor"
#endif

#ifndef DEVICE_PATH
    #define DEVICE_PATH "/dev/" DEVICE_NAME
#endif

#ifndef SYSFS_PATH
    #define SYSFS_PATH "/sys/class/misc/" DEVICE_NAME
#endif

#ifndef MODULE_PATH
    #define MODULE_PATH "/lib/modules/combFilter.ko"
#endif

/* Function to print usage instructions */
void print_usage(const char *program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  --read <offset>      Read from device at specific offset\n");
    printf("  --write <offset> <value>  Write value to device at specific offset\n");
    printf("  --show-regs          Show all register values via sysfs\n");
    printf("  --set-delaym <value> Set delaym register via sysfs\n");
    printf("  --set-b0 <value>     Set b0 register via sysfs\n");
    printf("  --set-bm <value>     Set bm register via sysfs\n");
    printf("  --set-wetdrymix <value> Set wetdrymix register via sysfs\n");
    printf("  --load-module        Load the kernel module if not already loaded\n");
    printf("  --unload-module      Unload the kernel module if currently loaded\n");
    printf("  -h, --help           Show this help message\n");
}

/* Function to read from the device at a specific offset */
int read_device_at_offset(int fd, off_t offset) {
    char buffer[4];
    ssize_t bytes_read;
    
    lseek(fd, offset, SEEK_SET);
    bytes_read = read(fd, buffer, sizeof(buffer));
    
    if (bytes_read < 0) {
        perror("read");
        return -1;
    }
    
    if (bytes_read == 0) {
        printf("No data read from offset %ld\n", offset);
        return 0;
    }
    
    unsigned int value = *(unsigned int*)buffer;
    printf("Read from offset %ld: 0x%08x (%u)\n", offset, value, value);
    return 0;
}

/* Function to write a value to the device at a specific offset */
int write_device_at_offset(int fd, off_t offset, unsigned int value) {
    ssize_t bytes_written;
    char buffer[4];
    
    *(unsigned int*)buffer = value;
    lseek(fd, offset, SEEK_SET);
    bytes_written = write(fd, buffer, sizeof(buffer));
    
    if (bytes_written < 0) {
        perror("write");
        return -1;
    }
    
    printf("Wrote 0x%08x (%u) to offset %ld\n", value, value, offset);
    return 0;
}

/* Function to read and display all register values from sysfs */
int show_registers() {
    struct stat st;
    if (stat(SYSFS_PATH, &st) != 0 || !S_ISDIR(st.st_mode)) {
        printf("Sysfs path %s not found or not a directory\n", SYSFS_PATH);
        return -1;
    }

    char path[256];
    FILE *file;
    char buffer[32];
    int value;

    printf("Reading register values from sysfs:\n");

    /* Read delaym */
    snprintf(path, sizeof(path), "%s/delaym", SYSFS_PATH);
    file = fopen(path, "r");
    if (file) {
        if (fgets(buffer, sizeof(buffer), file)) {
            value = atoi(buffer);
            printf("delaym: %d\n", value);
        } else {
            printf("Failed to read %s\n", path);
        }
        fclose(file);
    } else {
        perror("fopen delaym");
    }

    /* Read b0 */
    snprintf(path, sizeof(path), "%s/b0", SYSFS_PATH);
    file = fopen(path, "r");
    if (file) {
        if (fgets(buffer, sizeof(buffer), file)) {
            value = atoi(buffer);
            printf("b0: %d\n", value);
        } else {
            printf("Failed to read %s\n", path);
        }
        fclose(file);
    } else {
        perror("fopen b0");
    }

    /* Read bm */
    snprintf(path, sizeof(path), "%s/bm", SYSFS_PATH);
    file = fopen(path, "r");
    if (file) {
        if (fgets(buffer, sizeof(buffer), file)) {
            value = atoi(buffer);
            printf("bm: %d\n", value);
        } else {
            printf("Failed to read %s\n", path);
        }
        fclose(file);
    } else {
        perror("fopen bm");
    }

    /* Read wetDryMix */
    snprintf(path, sizeof(path), "%s/wetDryMix", SYSFS_PATH);
    file = fopen(path, "r");
    if (file) {
        if (fgets(buffer, sizeof(buffer), file)) {
            value = atoi(buffer);
            printf("wetDryMix: %d\n", value);
        } else {
            printf("Failed to read %s\n", path);
        }
        fclose(file);
    } else {
        perror("fopen wetDryMix");
    }

    return 0;
}

/* Function to set a sysfs register value */
int set_register(const char *reg_name, int value) {
    char path[256];
    FILE *file;
    
    snprintf(path, sizeof(path), "%s/%s", SYSFS_PATH, reg_name);
    file = fopen(path, "w");
    if (!file) {
        perror("fopen");
        printf("Failed to open sysfs attribute %s\n", path);
        return -1;
    }
    
    fprintf(file, "%d", value);
    fclose(file);
    printf("Set %s to %d\n", reg_name, value);
    
    return 0;
}

/* Function to check if the kernel module is loaded */
int is_module_loaded(const char *module_name) {
    /* Check /proc/modules for the module */
    FILE *fp = fopen("/proc/modules", "r");
    if (!fp) {
        perror("fopen /proc/modules");
        return -1;
    }

    char line[256];
    int module_found = 0;
    while (fgets(line, sizeof(line), fp)) {
        char *name = strtok(line, " \t\n");
        if (name && strcmp(name, module_name) == 0) {
            module_found = 1;
            break;
        }
    }
    fclose(fp);

    if (!module_found) {
        printf("Module %s not found in /proc/modules\n", module_name);
        return 0;
    }

    /* Check device file existence */
    struct stat st;
    if (stat(DEVICE_PATH, &st) == 0 && S_ISCHR(st.st_mode)) {
        return 1; /* Module loaded and device file exists */
    }

    printf("Warning: Module %s is loaded, but device file %s not found\n", module_name, DEVICE_PATH);
    return 0;
}

/* Function to load the kernel module */
int load_module(const char *module_name) {
    if (is_module_loaded(module_name) == 1) {
        printf("Module %s is already loaded\n", module_name);
        return 0;
    }

    /* Verify module file exists */
    struct stat st;
    if (stat(MODULE_PATH, &st) != 0) {
        printf("Module file %s does not exist\n", MODULE_PATH);
        return -1;
    }

    char command[256];
    snprintf(command, sizeof(command), "insmod %s", MODULE_PATH);
    int result = system(command);
    if (result == 0) {
        /* Verify module loaded successfully */
        if (is_module_loaded(module_name) == 1) {
            printf("Module %s loaded successfully\n", module_name);
            return 0;
        } else {
            printf("Module %s failed to load (insmod succeeded but module not found)\n", module_name);
            return -1;
        }
    } else {
        /* Check if failure was due to module already loaded */
        if (is_module_loaded(module_name) == 1) {
            printf("Module %s is already loaded (insmod failed: File exists)\n", module_name);
            return 0;
        }
        printf("Failed to load module %s (insmod error)\n", module_name);
        return -1;
    }
}

/* Function to unload the kernel module */
int unload_module(const char *module_name) {
    if (is_module_loaded(module_name) != 1) {
        printf("Module %s is not loaded\n", module_name);
        return 0;
    }

    char command[256];
    snprintf(command, sizeof(command), "rmmod %s", module_name);
    int result = system(command);
    if (result == 0) {
        printf("Module %s unloaded successfully\n", module_name);
        return 0;
    } else {
        printf("Failed to unload module %s\n", module_name);
        return -1;
    }
}

/* Main function */
int main(int argc, char *argv[]) {
    int fd = -1;
    int i;

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    /* Handle load/unload module commands first */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--load-module") == 0) {
            int module_loaded = is_module_loaded(MODULE_NAME);
            if (module_loaded == -1) {
                printf("Error checking module status\n");
                return 1;
            }
            if (module_loaded == 0) {
                if (load_module(MODULE_NAME) != 0) {
                    return 1;
                }
            } else {
                printf("Module %s is already loaded\n", MODULE_NAME);
            }
            return 0;
        }
        else if (strcmp(argv[i], "--unload-module") == 0) {
            int module_loaded = is_module_loaded(MODULE_NAME);
            if (module_loaded == -1) {
                printf("Error checking module status\n");
                return 1;
            }
            if (module_loaded == 1) {
                if (unload_module(MODULE_NAME) != 0) {
                    return 1;
                }
            } else {
                printf("Module %s is not loaded\n", MODULE_NAME);
            }
            return 0;
        }
    }

    /* Check if module is loaded */
    int module_loaded = is_module_loaded(MODULE_NAME);
    if (module_loaded == -1) {
        printf("Error checking module status\n");
        return 1;
    }
    if (module_loaded == 0) {
        printf("Module %s is not loaded. Attempting to load...\n", MODULE_NAME);
        if (load_module(MODULE_NAME) != 0) {
            return 1;
        }
    }

    /* Open the device */
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("open");
        printf("Failed to open device %s. Ensure the kernel module is loaded and device file exists.\n", DEVICE_PATH);
        return 1;
    }

    /* Process other commands */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--read") == 0) {
            if (i + 1 >= argc) {
                printf("Missing offset argument for --read\n");
                close(fd);
                return 1;
            }
            off_t offset = atoi(argv[++i]);
            read_device_at_offset(fd, offset);
        }
        else if (strcmp(argv[i], "--write") == 0) {
            if (i + 2 >= argc) {
                printf("Missing offset and value arguments for --write\n");
                close(fd);
                return 1;
            }
            off_t offset = atoi(argv[++i]);
            unsigned int value = atoi(argv[++i]);
            write_device_at_offset(fd, offset, value);
        }
        else if (strcmp(argv[i], "--show-regs") == 0) {
            show_registers();
        }
        else if (strcmp(argv[i], "--set-delaym") == 0) {
            if (i + 1 >= argc) {
                printf("Missing value argument for --set-delaym\n");
                close(fd);
                return 1;
            }
            int value = atoi(argv[++i]);
            set_register("delaym", value);
        }
        else if (strcmp(argv[i], "--set-b0") == 0) {
            if (i + 1 >= argc) {
                printf("Missing value argument for --set-b0\n");
                close(fd);
                return 1;
            }
            int value = atoi(argv[++i]);
            set_register("b0", value);
        }
        else if (strcmp(argv[i], "--set-bm") == 0) {
            if (i + 1 >= argc) {
                printf("Missing value argument for --set-bm\n");
                close(fd);
                return 1;
            }
            int value = atoi(argv[++i]);
            set_register("bm", value);
        }
        else if (strcmp(argv[i], "--set-wetdrymix") == 0) {
            if (i + 1 >= argc) {
                printf("Missing value argument for --set-wetdrymix\n");
                close(fd);
                return 1;
            }
            int value = atoi(argv[++i]);
            set_register("wetDryMix", value);
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            close(fd);
            return 0;
        }
        else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            close(fd);
            return 1;
        }
    }

    close(fd);
    return 0;
}