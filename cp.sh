# 文件复制



sudo cp /mnt/hgfs/winshare/s5pv210/uboot/android_uboot_smdkv210.tar/u-boot-samsung-dev/board/samsung/smdkc110/lowlevel_init.S ./board/samsung/smdkc110/ -f


sudo cp /mnt/hgfs/winshare/s5pv210/uboot/android_uboot_smdkv210.tar/u-boot-samsung-dev/include/configs/smdkv210single.h ./include/configs/smdkv210single.h -f

sudo cp /mnt/hgfs/winshare/s5pv210/uboot/android_uboot_smdkv210.tar/u-boot-samsung-dev/board/samsung/smdkc110/smdkc110.c ./board/samsung/smdkc110/ -f

sudo cp /mnt/hgfs/winshare/s5pv210/uboot/android_uboot_smdkv210.tar/u-boot-samsung-dev/drivers/mmc/mmc.c ./drivers/mmc/ -f


sudo make smdkv210single_config
