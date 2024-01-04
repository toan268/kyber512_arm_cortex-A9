#include "fpga_sha.h"


//volatile unsigned long *h2f_CAM_search = NULL;
//volatile unsigned long *h2f_CAM_setting = NULL;
static volatile unsigned long *h2f_CAM_addr = NULL;

#define h2f_write_w_en(IN)          alt_write_word(h2f_CAM_addr + DATA_OUT_0, IN)
#define h2f_write_din_1(IN)         alt_write_word(h2f_CAM_addr + DATA_OUT_1, IN)
#define h2f_write_din_2(IN)         alt_write_word(h2f_CAM_addr + DATA_OUT_2, IN)
// #define h2f_write_din_valid(IN)     alt_write_word(h2f_CAM_addr + DATA_OUT_3, IN)
#define h2f_write_control_i(IN)     alt_write_word(h2f_CAM_addr + DATA_OUT_4, IN) //0_start, 1_lastblock

#define h2f_read_wfull_1            alt_read_word(h2f_CAM_addr + DATA_IN_0)
#define h2f_read_wfull_2            alt_read_word(h2f_CAM_addr + DATA_IN_1)
#define h2f_read_wfull_3            alt_read_word(h2f_CAM_addr + DATA_IN_2)
#define h2f_read_wfull_4            alt_read_word(h2f_CAM_addr + DATA_IN_3)
#define h2f_read_dout_1_fifo        alt_read_word(h2f_CAM_addr + DATA_IN_4)
#define h2f_read_dout_2_fifo        alt_read_word(h2f_CAM_addr + DATA_IN_5)
#define h2f_read_control_o          alt_read_word(h2f_CAM_addr + DATA_IN_6) //0_ready, 1_buffer_full
#define h2f_read_rempty3            alt_read_word(h2f_CAM_addr + DATA_IN_7)
#define h2f_read_rempty4            alt_read_word(h2f_CAM_addr + DATA_IN_8)

#define first32bit(IN)				*IN<<24|*(IN+1)<<16|*(IN+2)<<8|*(IN+3)
#define last32bit(IN)				*(IN+4)<<24|*(IN+5)<<16|*(IN+6)<<8|*(IN+7)
static void *virtual_base_lw;
static void *virtual_base_hw;
static int fd;
/*************************************************
* Name:        store64
*
* Description: Store a 64-bit integer to array of 8 bytes in little-endian order
*
* Arguments:   - uint8_t *x: pointer to the output byte array (allocated)
*              - uint64_t u: input 64-bit unsigned integer
**************************************************/
static void store32(uint8_t x[4], uint32_t u) {
  unsigned int i;

  for(i=0;i<4;i++)
    x[i] = u >> 8*i;
}

int hps_fpgainit(void) {
	// map the address space for the LED registers into user space so we can interact with them.
	// we'll actually map in the entire CSR span of the HPS since we want to access various registers within that span
	if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
		printf("ERROR: could not open \"/dev/mem\"...\n");
		return(1);
	}
	
	// Lightweight bus
	virtual_base_lw = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
	if (virtual_base_lw == MAP_FAILED) {
		printf("ERROR: mmap() lightweight failed...\n");
		close(fd);
		return(1);
	}

	// Heavyweight bus
	virtual_base_hw = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_H2F_BRIDGE_OFST);
	if (virtual_base_hw == MAP_FAILED) {
		printf("ERROR: mmap() heavyweight failed...\n");
		close(fd);
		return(1);
	}

	// Open bridge
	h2f_CAM_addr = virtual_base_lw + ((unsigned long)(HW_LW_H2F_BRIDGE_OFST + SLAVE_TEMPLATE_0_BASE) & (unsigned long)(HW_REGS_MASK));
	return(0);
}

int hps_fpgarelease(void){
	if (munmap(virtual_base_lw, HW_REGS_SPAN) != 0) {
		printf("ERROR: munmap() failed...\n");
		close(fd);
		return(1);
	}

	if (munmap(virtual_base_hw, HW_REGS_SPAN) != 0) {
		printf("ERROR: munmap() failed...\n");
		close(fd);
		return(1);
	}

	printf("END OF CODE! SUCCESSFULLY EXECUTING!\n");
	close(fd);
	return 0;
}
/*************************************************
* Name:        fpga_sha3_256
*
* Description: SHA3-256 using FPGA IP
*
* Arguments:   - uint8_t *h: pointer to output (32 bytes)
*              - const uint8_t *in: pointer to input
*              - size_t inlen: length of input in bytes
**************************************************/
void fpga_sha3_256(uint8_t h[32], const uint8_t *in, size_t inlen)
{
  unsigned int i, cnt;
  h2f_write_w_en(0x1);
  h2f_write_control_i(0b01);
  for (cnt=0; cnt<inlen;)
  { 
	if (h2f_read_control_o==0b00)&&(h2f_read_wfull_1==0x0)&&(h2f_read_wfull_2==0x0) //Write condition
	{
		h2f_write_din_1(first32bit(in+cnt));
		h2f_write_din_2(last32bit(in+cnt));
		cnt=cnt+8;
	}
  }
  for (i=0;i<4;)
  {
  	if ((h2f_read_rempty3==0)&&(h2f_read_rempty3==0))//Read condition
	{
		store32(h+i,h2f_read_dout_1_fifo);
		store32(h+i+4,h2f_read_dout_2_fifo);
		i++;
	}
  }
//   for(i=0;i<4;i++)
//     store64(h+8*i,s[i]);
}
