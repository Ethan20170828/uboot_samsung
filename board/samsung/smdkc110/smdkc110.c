/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <regs.h>
#include <asm/io.h>


/* ------------------------------------------------------------------------- */
#define SMC9115_Tacs	(0x0)	// 0clk		address set-up
#define SMC9115_Tcos	(0x4)	// 4clk		chip selection set-up
#define SMC9115_Tacc	(0xe)	// 14clk	access cycle
#define SMC9115_Tcoh	(0x1)	// 1clk		chip selection hold
#define SMC9115_Tah	(0x4)	// 4clk		address holding time
#define SMC9115_Tacp	(0x6)	// 6clk		page mode access cycle
#define SMC9115_PMC	(0x0)	// normal(1data)page mode configuration

#define SROM_DATA16_WIDTH(x)	(1<<((x*4)+0))
#define SROM_WAIT_ENABLE(x)	(1<<((x*4)+1))
#define SROM_BYTE_ENABLE(x)	(1<<((x*4)+2))

/* ------------------------------------------------------------------------- */
#define DM9000_Tacs	(0x0)	// 0clk		address set-up
#define DM9000_Tcos	(0x4)	// 4clk		chip selection set-up
#define DM9000_Tacc	(0xE)	// 14clk	access cycle
#define DM9000_Tcoh	(0x1)	// 1clk		chip selection hold
#define DM9000_Tah	(0x4)	// 4clk		address holding time
#define DM9000_Tacp	(0x6)	// 6clk		page mode access cycle
#define DM9000_PMC	(0x0)	// normal(1data)page mode configuration


static inline void delay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n" "bne 1b":"=r" (loops):"0"(loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */
// �������������ֲ�Ĺؼ�
// ��Ҫ���ܾ��ǳ�ʼ��DM9000������
// �����ʼ�����̺Ϳ�������DM9000����оƬ��Ӳ�����ӷ�ʽ�йء�����Ҫ��Ͽ�����ԭ��ͼ��������
// Ȼ��������������ô��̡�
static void dm9000_pre_init(void)
{
	unsigned int tmp;
// ԭ�����������ǹ���ʦ����SMDKV210��Ӳ���ӷ���д�ĳ�������Ҫ�����Լ��Ŀ������Ӳ���ӷ�ȥ�޸����
// ���������������ǵĿ��������ܹ�����
// SROM_BW_REG��SROM_BC5_REG������s5pv210�����ֲ��е�SROM�������Ĵ�������
#if defined(DM9000_16BIT_DATA)
// ���Կ������ǿ������ϵ�DM9000������bank5����bit20-bit23
	//SROM_BW_REG &= ~(0xf << 20);
	//SROM_BW_REG |= (0<<23) | (0<<22) | (0<<21) | (1<<20);
// ���ǿ������ϵ�DM9000������bank1����bit4-bit7					/* sxh changed */
	SROM_BW_REG &= ~(0xf << 4);
	SROM_BW_REG |= (1<<7) | (1<<6) | (1<<5) | (1<<4);
#else	
	SROM_BW_REG &= ~(0xf << 20);
	SROM_BW_REG |= (0<<19) | (0<<18) | (0<<16);
#endif
// ������BC5�����������ΪBC1����
	//SROM_BC5_REG = ((0<<28)|(1<<24)|(5<<16)|(1<<12)|(4<<8)|(6<<4)|(0<<0));
	SROM_BC1_REG = ((0<<28)|(1<<24)|(5<<16)|(1<<12)|(4<<8)|(6<<4)|(0<<0));

// MP01CON_REG�Ĵ�������s5pv210�����ֲ��GPIO��(MP01)
	tmp = MP01CON_REG;
	//tmp &=~(0xf<<20);
	//tmp |=(2<<20);
	tmp &=~(0xf<<4);
	tmp |=(2<<4);
	MP01CON_REG = tmp;
}


int board_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;
#ifdef CONFIG_DRIVER_SMC911X
	smc9115_pre_init();
#endif

// ֻҪ������оƬ��ȷ�ĳ�ʼ���ˣ�������оƬ���ܹ���(��˼����������dm9000x.c��dm9000x.h�����������
// ��ʼ��������)
// ��Linuxϵͳ�У���������һ���豸������豸�������������������һ���豸����ethn(n��0��1��2������)
// (������������һ���wlan0��wlan1������)��Ȼ��Linuxϵͳ��һЩר�õ�����������������Ʃ��ifconfig���
// ��Linux�µ�Ӧ�ó������ʹ��������������������ͨ��? ��ͨ�õķ�������socket�ӿڡ�Linuxϵͳ����һϵ��
// ��API�Ϳ⺯�����ṩ��һ��socket��̽ӿڣ�Linux�µ�Ӧ�ó�����ͨ��socket��ʵ��������socket�ڲ�����
// ��ӵ��õ���������ʵ������ͨ�ŵġ�
// Linux����Ƿǳ��걸�ģ�Ӧ�ò�����������ϸ����ġ�Ҳ����˵д������Ӧ�ò���˸������ù�������
// ֻҪ����socket�ӿڼ��ɣ�д�ײ��������˸������ù�Ӧ�ò㣬ֻҪ����Linux����������ģ�ͼ��ɡ�

// һ��Ҫ��ס: uboot������һ�����������һ�����壬û�зֲ㡣����uboot�и���û��������Ӧ�õĸ��
// �����߼���˵��ping����������ʵ�ֵĴ����������Ӧ�õ�Ӧ�ó�����dm9000x.c��dm9000x.h�����Ĵ���
// ������������������uboot����Щ����������һ��ģ�Ӧ����ֱ�ӵ�������ʵ�ֵġ�Ҳ����˵ping�����ڲ�
// ��ֱ�ӵ�����dm9000�����������еĺ�����ʵ���Լ��ġ�
#ifdef CONFIG_DRIVER_DM9000
	dm9000_pre_init();
#endif

	gd->bd->bi_arch_number = MACH_TYPE;
	gd->bd->bi_boot_params = (PHYS_SDRAM_1+0x100);

	return 0;
}
// ��������Ϣ�Լ�bdinfo������Ľ������ʾDRAM bank0��1��sizeֵ�����ô��ˡ�
// ʹ��md��mw��������ڴ棬����20000000��40000000
int dram_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

#if defined(PHYS_SDRAM_2)
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size = PHYS_SDRAM_2_SIZE;
#endif

	return 0;
}

#ifdef BOARD_LATE_INIT
#if defined(CONFIG_BOOT_NAND)
int board_late_init (void)
{
	uint *magic = (uint*)(PHYS_SDRAM_1);
	char boot_cmd[100];

	if ((0x24564236 == magic[0]) && (0x20764316 == magic[1])) {
		sprintf(boot_cmd, "nand erase 0 40000;nand write %08x 0 40000", PHYS_SDRAM_1 + 0x8000);
		magic[0] = 0;
		magic[1] = 0;
		printf("\nready for self-burning U-Boot image\n\n");
		setenv("bootdelay", "0");
		setenv("bootcmd", boot_cmd);
	}

	return 0;
}
#elif defined(CONFIG_BOOT_MOVINAND)
int board_late_init (void)
{
	uint *magic = (uint*)(PHYS_SDRAM_1);
	char boot_cmd[100];
	int hc;

	hc = (magic[2] & 0x1) ? 1 : 0;

	if ((0x24564236 == magic[0]) && (0x20764316 == magic[1])) {
		sprintf(boot_cmd, "movi init %d %d;movi write u-boot %08x", magic[3], hc, PHYS_SDRAM_1 + 0x8000);
		magic[0] = 0;
		magic[1] = 0;
		printf("\nready for self-burning U-Boot image\n\n");
		setenv("bootdelay", "0");
		setenv("bootcmd", boot_cmd);
	}

	return 0;
}
#else
int board_late_init (void)
{
	return 0;
}
#endif
#endif

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
#ifdef CONFIG_MCP_SINGLE
#if defined(CONFIG_VOGUES)
	printf("\nBoard:   VOGUESV210\n");
#else
	printf("\nBoard:   SMDKV210\n");
#endif //CONFIG_VOGUES
#else
	printf("\nBoard:   SMDKC110\n");
#endif
	return (0);
}
#endif

#ifdef CONFIG_ENABLE_MMU

#ifdef CONFIG_MCP_SINGLE
ulong virt_to_phy_smdkc110(ulong addr)
{
	if ((0xc0000000 <= addr) && (addr < 0xd0000000))
		//return (addr - 0xc0000000 + 0x20000000);
		return (addr - 0xc0000000 + 0x30000000);		/* sxh added */
	else
		printf("The input address don't need "\
			"a virtual-to-physical translation : %08lx\n", addr);

	return addr;
}
#else
ulong virt_to_phy_smdkc110(ulong addr)
{
	if ((0xc0000000 <= addr) && (addr < 0xd0000000))
		return (addr - 0xc0000000 + 0x30000000);
	else if ((0x30000000 <= addr) && (addr < 0x50000000))
		return addr;
	else
		printf("The input address don't need "\
			"a virtual-to-physical translation : %08lx\n", addr);

	return addr;
}
#endif

#endif

#if defined(CONFIG_CMD_NAND) && defined(CFG_NAND_LEGACY)
#include <linux/mtd/nand.h>
extern struct nand_chip nand_dev_desc[CFG_MAX_NAND_DEVICE];
void nand_init(void)
{
	nand_probe(CFG_NAND_BASE);
        if (nand_dev_desc[0].ChipID != NAND_ChipID_UNKNOWN) {
                print_size(nand_dev_desc[0].totlen, "\n");
        }
}
#endif
