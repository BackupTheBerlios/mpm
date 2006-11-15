#ifndef VGA_REGS_H
#define VGA_REGS_H

#define	VGA_AC_INDEX        0x3C0
#define	VGA_AC_WRITE        0x3C0
#define	VGA_AC_READ         0x3C1
#define	VGA_MISC_WRITE      0x3C2
#define VGA_SEQ_INDEX       0x3C4
#define VGA_SEQ_DATA        0x3C5
#define	VGA_DAC_READ_INDEX  0x3C7
#define	VGA_DAC_WRITE_INDEX 0x3C8
#define	VGA_DAC_DATA        0x3C9
#define	VGA_MISC_READ       0x3CC
#define VGA_GC_INDEX        0x3CE
#define VGA_GC_DATA         0x3CF
#define VGA_CRTC_INDEX      0x3D4
#define VGA_CRTC_DATA       0x3D5
#define	VGA_INSTAT_READ     0x3DA

#define	VGA_NUM_SEQ_REGS    5
#define	VGA_NUM_CRTC_REGS   25
#define	VGA_NUM_GC_REGS     9
#define	VGA_NUM_AC_REGS     21

typedef struct vga_registers_s {
    unsigned char misc[1];
    unsigned char seq[VGA_NUM_SEQ_REGS];
    unsigned char crtc[VGA_NUM_CRTC_REGS];
    unsigned char gc[VGA_NUM_GC_REGS];
    unsigned char ac[VGA_NUM_AC_REGS];
} vga_registers_t;

#endif
