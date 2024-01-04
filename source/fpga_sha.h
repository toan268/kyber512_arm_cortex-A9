#ifndef FPGA_SHA_H
#define FPGA_SHA_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"
#include "slave_template_macros.h"
#include <stddef.h>
#include <stdint.h>

#define HW_H2F_BRIDGE_OFST		( 0xC0000000 )				// 0xC000_0000 - Heavyweight bus
#define HW_LW_H2F_BRIDGE_OFST	( ALT_LWFPGASLVS_OFST )		// 0xFF20_0000 - Lightweight bus
#define HW_REGS_BASE			( ALT_STM_OFST )			// 0xFC00_0000 - Peripherals Region
#define HW_REGS_SPAN			( 0x04000000 )
#define HW_REGS_MASK			( HW_REGS_SPAN - 1 )
#define WRITEENABLE				( 0x00000800 )

int hps_fpgainit(void);
int hps_fpgarelease(void);
void fpga_sha3_256(uint8_t h[32], const uint8_t *in, size_t inlen);

#endif
