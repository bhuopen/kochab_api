# kochab_api
kochab api for third-party developers

本文档所有步骤，建议用root帐号执行
系统要求：
64位linux。 推荐使用ubuntu 64位操作系统。




开发环境准备
1. 下载交叉编译包， http://pan.baidu.com/s/1dDF7Crv ,   放入/opt目录
2. 解压交叉编译包，确保目录结构为 /opt/mips_linux_74k
3. apt-get install make gcc g++ bison flex gettext patch (执行编译的过程中看缺啥补啥)
4. 执行以下命令，编译交叉工具
    cd /opt/mips_linux_74k
    ./build_gcc ap123





开发步骤(以下假设固件名称为fw.img)
1. 获取BHU的设备固件文件fw.img，放入当前目录
2. 导入环境变量
    . ./env
3. 在当前目录执行以下命令，解压固件，提取lib文件, 需要指定固件版本对应使用的头文件目录(通过TG参数)
    make TG=1.2.10 IMG=fw.img extract
    在build目录下，生成fw.img.part文件，info.txt是固件相关信息，其余文件是固件中解压出来的分区文件
4. 参考example目录的几个样例c文件
5. 在当前目录，执行以下命令，该命令自动编译example目录的c文件，并重新制作新的rootfs文件
    make TG=1.2.10 IMG=fw.img
6. 新制作的rootfs文件为build/fw.img.part/rootfs.new
7. 把各个文件提交到BHU提供的在线固件制作页面http://61.149.47.34:81/，通过页面，重新打包为一个新的固件。
   分区名称，就是对应的分区文件名。（只有rootfs.new, 打包时，指定的分区名称应为rootfs)
8. 系统内的固件升级接口，参看doc目录下相关文件.


example说明
conf_merge.c 两个xml格式文档合并的例子，(利用设备自带的xml操作库)
conf_praser.c xml解析例子(利用设备自带的xml操作库), 解析xml并打印解析获得的结果
ctrl.c 一个封装了http操作的c程序，可以在设备上和PC上运行。演示了如何查询，修改设备配置。
wlanuser.c 演示如何获取系统的事件消息。
CgiOpr.py 通过python控制设备的演示例子（未包含在Makefile中，需要Phtyon运行环境)



修改bhuroot帐号密码，参见doc目录下的《bhuroot帐号密码.txt》

