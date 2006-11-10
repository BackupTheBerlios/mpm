/*
 * printparts.c, copyright (c) 2006, Ivo van Poorten
 *
 * BSD 3-clause license
 *
 * scan /dev/c0d*p*s* and print a list of available devices and partitions
 *
 */

#define _MINIX
#define _POSIX_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ibm/partition.h>
#include "printparts.h"

ind_t ind_table[] = {
    { 0x00,"None"      }, { 0x01,"FAT-12"    }, { 0x02,"XENIX /"   },
    { 0x03,"XENIX usr" }, { 0x04,"FAT-16"    }, { 0x05,"EXTENDED"  },
    { 0x06,"FAT-16"    }, { 0x07,"HPFS/NTFS" }, { 0x08,"AIX"       },
    { 0x09,"COHERENT"  }, { 0x0A,"OS/2"      }, { 0x0B,"FAT-32"    },
    { 0x0C,"FAT?"      }, { 0x0E,"FAT?"      }, { 0x0F,"EXTENDED"  },
    { 0x10,"OPUS"      }, { 0x40,"VENIX286"  }, { 0x42,"W2000 Dyn" },
    { 0x52,"MICROPORT" }, { 0x63,"386/IX"    }, { 0x64,"NOVELL286" },
    { 0x65,"NOVELL386" }, { 0x75,"PC/IX"     }, { 0x80,"MINIX-OLD" },
    { 0x81,"MINIX"     }, { 0x82,"LINUXswap" }, { 0x83,"LINUX"     },
    { 0x93,"AMOEBA"    }, { 0x94,"AMOEBAbad" }, { 0xA5,"386BSD"    },
    { 0xA6,"OPENBSD"   }, { 0xA9,"NETBSD"    },
    { 0xB7,"BSDI"      }, { 0xB8,"BSDI swap" }, { 0xC7,"SYRINX"    },
    { 0xDB,"CPM"       }, { 0xFF,"BADBLOCKS" }, { 0x00,NULL        }
};

int cflag = 0, dflag = 0, eflag = 0, uflag = 0;

static void print_ind(unsigned char ind) {
    int x;

    for(x=0; ind_table[x].name; x++) {
        if (ind_table[x].ind == ind) {
            printf(ind_table[x].name);
            return;
        }
    }
    printf("unknown");
}

static int part_exists(char *pdev, partition_t *partition) {
    int fd;
    char c;

    fd = open(pdev, O_RDONLY);
    if (fd > 0 ) {
        if (read(fd, &c, 1) == 1) {
            partition->exists = 1;
            close(fd);
        } else {
            partition->exists = 0;
        }
    }

    return partition->exists;
}

static void part_type(char *dev, off_t offset, partition_t *partition) {
    struct part_entry pent;
    int fd;

    fd = open(dev, O_RDONLY);

    if (fd > 0) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &pent, sizeof(pent));
        partition->type = pent.sysind;
    } else {
        partition->type = 0;
    }

    close(fd);
}

static void scan_partitions(char *dev, device_t *device) {
    int x, y;
    char pdev[16], sdev[16];
    partition_t *partitions = device->partitions;

    for (x=0; x<nr_partitions; x++) {
        snprintf(pdev, sizeof(pdev), "%sp%u", dev, x);
        if (part_exists(pdev, &partitions[x])) {
            part_type(dev, PART_TABLE_OFF + x * sizeof(struct part_entry),
                                                            &partitions[x]);
            if (partitions[x].type == 0x81 /* MINIX */ ) {
                partition_t *partitions = device->subpartitions[x];

                for (y=0; y<nr_partitions; y++) {
                    snprintf(sdev, sizeof(sdev), "%ss%u", pdev, y);
                    if (part_exists(sdev, &partitions[y])) {
                        part_type(pdev, PART_TABLE_OFF + \
                                        x * sizeof(struct part_entry), \
                                        &partitions[y]);
                    }
                }
            }
        }
    }
}

static void scan_devices(devices_t devices) {
    int x, fd;
    char dev[16];

    for (x=0; x<nr_devices; x++) {
        snprintf(dev, sizeof(dev), "/dev/c0d%u", x);
        fd = open(dev, O_RDONLY);
        if (fd > 0) {
            close(fd);
            devices[x].exists = 1;
            fd = open(dev, O_WRONLY);
            if (fd > 0)
                devices[x].type = HARDDISK;
            else
                devices[x].type = CDROM;
            close(fd);
            scan_partitions(dev, &devices[x]);
        } else {
            devices[x].exists = 0;
        }
    }
}

static void print_partitions(char *dev, device_t *device) {
    int x, y;
    char pdev[16], sdev[16];
    partition_t *partitions = device->partitions;

    for (x=0; x<nr_partitions; x++) {
        snprintf(pdev, sizeof(pdev), "%sp%u", dev, x);
        if (partitions[x].exists) {
            printf("    %s) ", pdev);
            print_ind(partitions[x].type);
            printf("\n");
            if (partitions[x].type == 0x81 /* MINIX */) {
                partition_t *partitions = device->subpartitions[x];
                for (y=0; y<nr_partitions; y++) {
                    snprintf(sdev, sizeof(pdev), "%ss%u", pdev, y);
                    if (partitions[y].exists) {
                        printf("        %s) ", sdev);
                        print_ind(partitions[y].type);
                        printf("\n");
                    } else {
                        if (eflag) printf("        %s) n/a\n", sdev);
                    }
                }
            }
        } else {
            if (eflag) printf("    %s) n/a\n", pdev);
        }
    }
}

static void print_devices(devices_t devices) {
    int x;
    char dev[16];

    for (x=0; x<nr_devices; x++) {
        snprintf(dev, sizeof(dev), "/dev/c0d%u", x);
        if (devices[x].exists) {
            switch(devices[x].type) {
                default:
                case UNKNOWN_DTYPE: {
                    if (uflag) {
                        printf("%s) unknown\n", dev);
                        print_partitions(dev, &devices[x]);
                    }
                    break;
                }
                case HARDDISK: {
                    if (dflag) {
                        printf("%s) harddisk\n", dev);
                        print_partitions(dev, &devices[x]);
                    }
                    break;
                }
                case CDROM: {
                    if (cflag) {
                        printf("%s) cdrom\n", dev);
                        print_partitions(dev, &devices[x]);
                    }
                    break;
                }
            }
        } else {
            if (eflag) printf("%s) n/a\n", dev);
        }
    }
}

static void usage(char *myname) {
    fprintf(stderr, "usage: %s [-c] [-d] [-e] [-h] [-u]\n", myname);
    fprintf(stderr, "  -c  include CD-ROM devices\n"
                    "  -d  include harddisk devices\n"
                    "  -e  include non-existent devices\n"
                    "  -h  print this help message\n"
                    "  -u  include unknown devices\n");
    _exit(1);
}

static void parse_args(int argc, char **argv) {
    int x;
    char *myname = argv[0];

    for (x=1; x<argc; x++) {
        if (argv[x][0] == '-' && argv[x][1] != '\0') {
            if (argv[x][2] != '\0') usage(myname);
            switch (argv[x][1]) {
                case 'c':   cflag = 1;      break;
                case 'd':   dflag = 1;      break;
                case 'e':   eflag = 1;      break;
                case 'u':   uflag = 1;      break;
                default:
                case 'h':   usage(myname);  break;
            }
        } else {
            usage(myname);
        }
    }
}

int main(int argc, char **argv) {
    devices_t devices;

    parse_args(argc, argv);
    scan_devices(devices);
    print_devices(devices);
}
