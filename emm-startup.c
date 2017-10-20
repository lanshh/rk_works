#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <pthread.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <getopt.h>
// vendor parameter
#define VENDOR_REQ_TAG 0x56524551
#define VENDOR_READ_IO _IOW('v', 0x01, unsigned int)
#define VENDOR_WRITE_IO _IOW('v', 0x02, unsigned int)

#define VENDOR_SN_ID 1
#define VENDOR_WIFI_MAC_ID 2
#define VENDOR_LAN_MAC_ID 3
#define VENDOR_BLUETOOTH_ID 4

#define VENDOR_GUEST_ID 12
#define VENDOR_PROTOCOL_ONVIF_ID    13
#define VENDOR_STREAM_ID 14
#define VENDOR_PROTOCOL_ID  15

/* Storage parameter */
#define VENDOR_PARAMETER_ID 5
/* Change the id when flash firmware */
#define VENDOR_FW_UPDATE_ID 6
#define VENDOR_EMMCINFO_ID 7

#define VENDOR_DATA_SIZE (3 * 1024)  // 3k
#define VENDOR_DATA_PROTOCOL_SIZE (384)  /* 64*6=384 byte */

#define VERDOR_DEVICE "/dev/vendor_storage"

static char emmc_partition[64] = "/dev/mmcblk0p7";
static char emmc_fs[64] = "ext4";
static char mount_dir[64] = "/user";

typedef struct _RK_VERDOR_REQ {
  uint32_t tag;
  uint16_t id;
  uint16_t len;
  uint8_t data[VENDOR_DATA_SIZE];
} RK_VERDOR_REQ;

typedef struct _RK_VERDOR_PROTOCOL_REQ {
  uint32_t tag;
  uint16_t id;
  uint16_t len;
  uint8_t data[VENDOR_DATA_PROTOCOL_SIZE];
} RK_VERDOR_PROTOCOL_REQ;

struct storage_param {
  int emmc_formated;
};

int parameter_vendor_read(int buffer_size, uint8_t* buffer, uint16_t vendor_id) 
{
    int ret;
    RK_VERDOR_REQ req;

    int sys_fd = open(VERDOR_DEVICE, O_RDWR, 0);
    if (sys_fd < 0) {
        printf("vendor_storage open fail\n");
        return -2;
    }

    req.tag = VENDOR_REQ_TAG;
    req.id = vendor_id;
    /* max read length to read */
    req.len = buffer_size > VENDOR_DATA_SIZE ? VENDOR_DATA_SIZE : buffer_size;
    ret = ioctl(sys_fd, VENDOR_READ_IO, &req);
    /* return req->len is the real data length stored in the NV-storage */
    if (ret) {
        //  printf("vendor read error ret = %d\n", ret);
        close(sys_fd);
        return -1;
    }
    memcpy(buffer, req.data, req.len);
    // rknand_print_hex_data("vendor read:", (uint32_t *)(& req), req.len + 8);
    close(sys_fd);
    return 0;
}

int parameter_vendor_write(int buffer_size, uint8_t* buffer, uint16_t vendor_id) 
{
    int ret;
    RK_VERDOR_REQ req;

    int sys_fd = open(VERDOR_DEVICE, O_RDWR, 0);
    if (sys_fd < 0) {
        printf("vendor_storage open fail\n");
        return -1;
    }
    req.tag = VENDOR_REQ_TAG;
    req.id = vendor_id;
    req.len = buffer_size > VENDOR_DATA_SIZE ? VENDOR_DATA_SIZE : buffer_size;
    memcpy(req.data, buffer, req.len);
    ret = ioctl(sys_fd, VENDOR_WRITE_IO, &req);
    if (ret) {
        printf("vendor write error\n");
        close(sys_fd);
        return -1;
    }
    // rknand_print_hex_data("vendor write:", (uint32_t *)(& req), req.len + 8);
    close(sys_fd);
    return 0;
}

#define parameter_write( x, y, z ) (z == 1 || z == 2 || z == 3 || z == 4 || z == 6 )?parameter_vendor_read( x, y, z ):parameter_file_write( x, y, z )
#define parameter_read( x, y, z )  (z == 1 || z == 2 || z == 3 || z == 4 || z == 6 )?parameter_vendor_write( x, y, z ):parameter_file_read( x, y, z )

int main(int argc, char *argv[])
{
    int ret, force = 0, opt;
    struct storage_param sp;
    char shellcmd[64];

    while ((opt = getopt(argc, argv, "ht:D:d:F")) != -1) {
        switch(opt) {
        case 't':
            sprintf(emmc_fs, "%s", optarg);
            break;
        case 'D':
            sprintf(mount_dir, "%s", optarg);
            break;
        case 'd':
            sprintf(emmc_partition, "%s", optarg);
            break;
        case 'F':
            force = 1;
            break;
        case 'h':
            printf("Usage: emmc_startup -t ext4 -d /dev/mmcblk0p6 -D /user\n");
            printf(" -t     mount file system type\n");
            printf(" -h     print this message\n");
            printf(" -d     device which to be mounted\n");
            printf(" -D     mount directory\n");
            printf(" -F     force format\n");
            return 0;
        }
    }
    printf("you will format %s as %s and mount it on %s\n", emmc_partition, emmc_fs, mount_dir);
    if (force) {
        sp.emmc_formated = 0;
        parameter_vendor_write(sizeof(struct storage_param), (uint8_t*)&sp, VENDOR_EMMCINFO_ID);
    }
    if(parameter_vendor_read(sizeof(struct storage_param), (uint8_t*)(uint8_t*)&sp, VENDOR_EMMCINFO_ID)) {
        printf("parameter_vendor_read fail, write default value\n");
        sp.emmc_formated = 0;
        parameter_vendor_write(sizeof(struct storage_param), (uint8_t*)&sp, VENDOR_EMMCINFO_ID);
    } else {
        printf("parameter_vendor_read %d\n", sp.emmc_formated);
    }
    sprintf(shellcmd, "mke2fs -F %s", emmc_partition);
    if(!sp.emmc_formated) {
        ret = system(shellcmd);
        if(ret) {
            printf("%s fail\n", shellcmd);
            return -1;
        }
        printf("%s  successfully\n", shellcmd);
    } else {
        printf("%s already formated, exit\n", emmc_partition);
        return 0;
    }
    sprintf(shellcmd, "mount -t %s %s %s", emmc_fs, emmc_partition, mount_dir);
    ret = system(shellcmd);
    if(ret) {
        printf("%s fail\n", shellcmd);
        return -1;
    }
    printf("%s  successfully\n", shellcmd);
    sp.emmc_formated = 1;
    parameter_vendor_write(sizeof(struct storage_param), (uint8_t*)&sp, VENDOR_EMMCINFO_ID);
    return 0;
}