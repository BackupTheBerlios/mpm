#ifndef PRINTPART_H
#define PRINTPART_H

#define nr_partitions 4
#define nr_devices 8

typedef struct partition_s {
    int             exists;
    unsigned char   type;
    unsigned long   size;
} partition_t;

typedef partition_t partitions_t[nr_partitions];

typedef partitions_t subpartitions_t[nr_partitions];

typedef enum dtype_e {
    UNKNOWN_DTYPE = 0,
    HARDDISK,
    CDROM
} dtype_t;

typedef struct device_s {
    int             exists;
    dtype_t         type;
    partitions_t    partitions;
    subpartitions_t subpartitions;
} device_t;

typedef device_t devices_t[nr_devices];

typedef struct ind_s {
    unsigned char   ind;
    char            *name;
} ind_t;

#endif
