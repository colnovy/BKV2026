#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define MBR_SIZE 512
#define PARTITION_TABLE_OFFSET 0x1BE
#define PARTITION_ENTRY_SIZE 16
#define MBR_SIGNATURE 0xAA55

const char *get_filesystem_name(uint8_t type)
{
    switch (type)
    {
    case 0x00:
        return "Empty";
    case 0x01:
        return "FAT12";
    case 0x02:
        return "XENIX root";
    case 0x03:
        return "XENIX usr";
    case 0x04:
        return "FAT16 <32M";
    case 0x05:
        return "Extended";
    case 0x06:
        return "FAT16";
    case 0x07:
        return "HPFS/NTFS";
    case 0x08:
        return "AIX";
    case 0x09:
        return "AIX bootable";
    case 0x0a:
        return "OS/2 Boot Manager";
    case 0x0b:
        return "Win95 FAT32";
    case 0x0c:
        return "Win95 FAT32 (LBA)";
    case 0x0e:
        return "Win95 FAT16 (LBA)";
    case 0x0f:
        return "Win95 Ext'd (LBA)";
    case 0x10:
        return "OPUS";
    case 0x11:
        return "Hidden FAT12";
    case 0x12:
        return "Compaq diagnost";
    case 0x14:
        return "Hidden FAT16 <32M";
    case 0x16:
        return "Hidden FAT16";
    case 0x17:
        return "Hidden HPFS/NTFS";
    case 0x18:
        return "AST SmartSleep";
    case 0x1b:
        return "Hidden Win95 FAT32";
    case 0x1c:
        return "Hidden Win95 FAT32 (LBA)";
    case 0x1e:
        return "Hidden Win95 FAT16 (LBA)";
    case 0x24:
        return "NEC DOS";
    case 0x39:
        return "Plan 9";
    case 0x3c:
        return "PartitionMagic";
    case 0x40:
        return "Venix 80286";
    case 0x41:
        return "PPC PReP Boot";
    case 0x42:
        return "SFS";
    case 0x4d:
        return "QNX4.x";
    case 0x4e:
        return "QNX4.x 2nd part";
    case 0x4f:
        return "QNX4.x 3rd part";
    case 0x50:
        return "OnTrack DM";
    case 0x51:
        return "OnTrack DM6 Aux";
    case 0x52:
        return "CP/M";
    case 0x53:
        return "OnTrack DM6 Aux";
    case 0x54:
        return "OnTrackDM6";
    case 0x55:
        return "EZ-Drive";
    case 0x56:
        return "Golden Bow";
    case 0x5c:
        return "Priam Edisk";
    case 0x61:
        return "SpeedStor";
    case 0x63:
        return "GNU HURD or Sys";
    case 0x64:
        return "Novell Netware";
    case 0x65:
        return "Novell Netware";
    case 0x70:
        return "DiskSecure Multi-Boot";
    case 0x75:
        return "PC/IX";
    case 0x80:
        return "Old Minix";
    case 0x81:
        return "Minix / old Linux";
    case 0x82:
        return "Linux swap";
    case 0x83:
        return "Linux";
    case 0x84:
        return "OS/2 hidden C:";
    case 0x85:
        return "Linux extended";
    case 0x86:
        return "NTFS volume set";
    case 0x87:
        return "NTFS volume set";
    case 0x8e:
        return "Linux LVM";
    case 0x93:
        return "Amoeba";
    case 0x94:
        return "Amoeba BBT";
    case 0x9f:
        return "BSD/OS";
    case 0xa0:
        return "IBM Thinkpad hibernate";
    case 0xa5:
        return "BSD/386";
    case 0xa6:
        return "OpenBSD";
    case 0xa7:
        return "NeXTSTEP";
    case 0xb7:
        return "BSDI fs";
    case 0xb8:
        return "BSDI swap";
    case 0xbb:
        return "Boot Wizard hidden";
    case 0xc1:
        return "DRDOS/sec (FAT)";
    case 0xc4:
        return "DRDOS/sec (FAT)";
    case 0xc6:
        return "DRDOS/sec (FAT)";
    case 0xc7:
        return "Syrinx";
    case 0xda:
        return "Non-FS data";
    case 0xdb:
        return "CP/M / CTOS";
    case 0xde:
        return "Dell Utility";
    case 0xdf:
        return "BootIt";
    case 0xe1:
        return "DOS access";
    case 0xe3:
        return "DOS R/O";
    case 0xe4:
        return "SpeedStor";
    case 0xeb:
        return "BeOS fs";
    case 0xee:
        return "EFI GPT";
    case 0xef:
        return "EFI (FAT-12/16/32)";
    case 0xf1:
        return "SpeedStor";
    case 0xf2:
        return "DOS secondary";
    case 0xf4:
        return "SpeedStor";
    case 0xfd:
        return "Linux raid auto";
    case 0xfe:
        return "LANstep";
    case 0xff:
        return "BBT";
    default:
        return "Unknown";
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_MBR>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("Opening error");
        return 1;
    }


    unsigned char mbr[MBR_SIZE];
    read(fd, mbr, MBR_SIZE);

    uint16_t sig = (uint16_t)(mbr[510] | (mbr[511] << 8));
    if (sig != MBR_SIGNATURE)
    {
        fprintf(stderr, "Invalid signature MBR\n");
        close(fd);
        return 1;
    }


    fflush(stdout);

    uint8_t *pt = mbr + PARTITION_TABLE_OFFSET;

    for (int i = 0; i < 4; i++)
    {
        uint8_t *e = pt + i * PARTITION_ENTRY_SIZE;
        uint8_t type = e[4];
        uint32_t start = e[8] | (e[9] << 8) | (e[10] << 16) | (e[11] << 24);
        uint32_t size = e[12] | (e[13] << 8) | (e[14] << 16) | (e[15] << 24);

        if (type == 0 && start == 0 && size == 0)
            continue;

        printf("Section %d\n", i + 1);
        printf("Boot file: %s\n", (e[0] == 0x80) ? "yes (*)" : "no");
        printf("Type (HEX): 0x%02X\n", type);
        printf("File system: %s\n", get_filesystem_name(type));
        printf("Beginning of section: %u LBA\n", start);
        printf("Size of section: %u LBA\n\n", size);
        fflush(stdout);
    }

    close(fd);
    return 0;
}