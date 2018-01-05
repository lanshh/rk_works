目前RV1108 + RK1608 的RV1108部分，采用的内部开发版本的SDK，相对比较不稳定
为了导入时候避免不不要的麻烦，这里建议按以下步骤把SDK回退到我这边使用的版本
1、拷贝rv1108_rk1608_ipc.xml到 SDK的 .repo\manifests目录
     scp rv1108_rk1608_ipc.xml lanshh@172.16.14.88:~/rv1108-rk1608-ipc/.repo/manifests
2、切换到对应manifest   .re
    lanshh@lanshh-rk01:~/rv1108-rk1608-ipc$ .repo/repo/repo init -m rv1108_rk1608_ipc.xml
    lanshh@lanshh-rk01:~/rv1108-rk1608-ipc$ .repo/repo/repo sync -j4


在build  common kernel 目录下面打上对应的补丁
注意，这里面有个文件，需要自己拷贝下：

    lanshh@lanshh-rk01:~/rv1108-rk1608-ipc$ cp build/mkfirmware_emmc.sh ./


编译
安装以下工具：
    autotools-dev m4 autoconf2.13  autoconf-archive gnu-standards autoconf-doc libtool squashfs-tools mtd-utils


1、app编译
    ./build_all.sh -j4

2、内核编译
    cd kernel && make rv1108_ipc_defconfig && make rv1108-rk1608-evb-v10.img -j4

3、打包
    ./mkfirmware_emmc.sh rv1108-rk1608-evb-v10