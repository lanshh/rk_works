ĿǰRV1108 + RK1608 ��RV1108���֣����õ��ڲ������汾��SDK����ԱȽϲ��ȶ�
Ϊ�˵���ʱ����ⲻ��Ҫ���鷳�����ｨ�鰴���²����SDK���˵������ʹ�õİ汾
1������rv1108_rk1608_ipc.xml�� SDK�� .repo\manifestsĿ¼
     scp rv1108_rk1608_ipc.xml lanshh@172.16.14.88:~/rv1108-rk1608-ipc/.repo/manifests
2���л�����Ӧmanifest   .re
    lanshh@lanshh-rk01:~/rv1108-rk1608-ipc$ .repo/repo/repo init -m rv1108_rk1608_ipc.xml
    lanshh@lanshh-rk01:~/rv1108-rk1608-ipc$ .repo/repo/repo sync -j4


��build  common kernel Ŀ¼������϶�Ӧ�Ĳ���
ע�⣬�������и��ļ�����Ҫ�Լ������£�

    lanshh@lanshh-rk01:~/rv1108-rk1608-ipc$ cp build/mkfirmware_emmc.sh ./


����
��װ���¹��ߣ�
    autotools-dev m4 autoconf2.13  autoconf-archive gnu-standards autoconf-doc libtool squashfs-tools mtd-utils


1��app����
    ./build_all.sh -j4

2���ں˱���
    cd kernel && make rv1108_ipc_defconfig && make rv1108-rk1608-evb-v10.img -j4

3�����
    ./mkfirmware_emmc.sh rv1108-rk1608-evb-v10