# 中文：Loader玩前须知

## 基本介绍
        这个玩意，是用来加载PE格式（俗称exe）文件的可执行文件加载器，运行在UEFI系统上，一般用于加载操作系统内核文件。
        它先构造好启动信息（内存分布，硬件分布，映射关系）。
        然后将可执行文件展开，重映射到内核运行所需要的基地址（修改CR3寄存器所指向的结构），解析出入口跳转地址，将CPU的指挥权交由内核。

## 使用说明
### 环境相关
        一：把这个项目放edk2里面编译好。

        二：把生成的EFI可执行文件放到启动设备的/EFI/BOOT/下面，重命名为BOOTX64.EFI。

        三：把你要加载的PE文件重命名为system.exe，放到启动设备根目录底下。
        （或者编辑一下functions.h里面的KERNEL_FILE_NAME，改字符串就行了，一定不要动最前面那个L）

        四：启动。
### PE文件相关
        写你的PE文件的时候不要带dll，构建的时候手动指定加载基地址和入口函数，要不然有可能被编译器和链接器坑。入口函数类型必须是void xxx(void)

## 硬件状态
        参见UEFI官方手册 UEFI_Spec_2_9_2021_03_18.pdf 第94页。
        启动信息指针就在地址0x5000里面

## 写在后面
        因为不知道取个啥名字就直接叫Loader了哈哈哈哈哈哈哈，在这里征集名称哦。
        
# English: Need to know before you play Loader

## Basic introduction
        This thing, is used to load Protable Executable format file, runs on UEFI based system, which usually used to load OS kernel file.
        It first construct the boot info, including memory map, hardware info. Then expand the file to the memory to creat an 'image'. Only image can run correctly on the machine. Loader will map the memory of image to the base address defined by the PE file instead of redirect the whole image. Then it parse the entry point of the image, call it and give the control to your image.

## How to use
### Envirment
        1.Put this thing into edk2 compile system to build up.

        2.Put the EFI binary file into your boot device. The path should be /EFI/BOOT/BOOTX64.EFI

        3.Rename your PE file to system.exe. Copy it to the root of your boot device.Or you can change the KERNEL_FILE_NAME in functions.h.(DO NOT TOUCH THAT 'L')

        4.Boot
### Your PE file
        Your PE file need specific base address which is defined by your self. And entry function. Entry function must be "void XXX(void)",XXX is your function name

## Hardware state
        UEFI_Spec_2_9_2021_03_18.pdf Page 94
        Boot info struct address is in 0x5000
