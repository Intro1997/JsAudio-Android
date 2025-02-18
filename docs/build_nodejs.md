# Build NodeJS for Android

NodeJS version: v18.20.4
Thirdparty repo: [nodejs-mobile](https://github.com/nodejs-mobile/nodejs-mobile)

It's ok to build NodeJS for Android if you use x86-64 Windows or Linux by [nodejs-mobile](https://github.com/nodejs-mobile/nodejs-mobile). But if you use arm macOS, please try the following method.

This method is valid in macOS 15.3.1 on M series chip, and thanks to the translation capabilities provided by rosetta, it can perform build work faster.

## Build on arm OS

This method is based on [lima](https://github.com/lima-vm/lima) and [containerd](https://github.com/containerd/containerd). And lima can use rosseta to support runing x86-64 application and run faster than pure x86-64 virtual machine, so let's do this!

## prepare work

> NOTE: As of the time of writing this article (2025/02/15), the Linux distribution with Linux kernel 6.11 cannot use Rosetta because it [removed support for DT_HASH](https://github.com/lima-vm/lima/issues/2891), and can only wait for Apple to fix it. Therefore, before executing `limactl start`, you need to check the Linux distributions with kernel versions lower than 6.11 that limactl supports.

First of all, we need to create a virtual machine by lima,

```shell
$ brew install lima
$ limactl start --list-templates # List all available images
# The kernel version of ubuntu-22.04 is lower than 6.11, and other distributions can also be used
# vz means using the macOS virtualization framework, refer to https://lima-vm.io/docs/config/vmtype/#vz
$ limactl start template://ubuntu-22.04 --rosetta --vm-type=vz
# After execution, enter the option interface and select the first option
```

Enter shell

```shell
$ limactl shell ubuntu-22.04
# Check rosetta support
$ cat /proc/sys/fs/binfmt_misc/rosetta
# The following supports rosetta
# If /proc/sys/fs/binfmt_misc/rosetta is not found, it is not supported
enabled
interpreter /mnt/lima-rosetta/rosetta
flags: OCF
offset 0
magic 7f454c4602010100000000000000000002003e00
mask fffffffffffefe00ffffffffffffffffffffffffff
```

Then use nerd to install linux:

```shell
# The image can be ubuntu:22.04, other image formats can be found here
# https://github.com/containerd/nerdctl/blob/main/docs/ipfs.md#using-nerdctl-with-ipfs
$ nerdctl run -it --platform amd64 ubuntu:22.04
```

If you are not in the virtual machine, you can `nerdctl ps -a` to view all installed virtual machines

```shell
$ nerdctl ps -a
CONTAINER ID IMAGE COMMAND CREATED STATUS PORTS NAMES
e00a6b2c7a1f docker.io/library/ubuntu:22.04 "/bin/bash" 26 hours ago Up ubuntu-e00a6
```

If the status is not Up, use `nerdctl start ubuntu-e00a6` to run the virtual machine, and then enter the shell through `nerdctl exec -it ubuntu-e00a6 /bin/bash`.

## build android so

> NOTE: The release package requires less memory, and 4G is enough in actual tests. However, the debug package requires at least 18G. Please allocate enough memory to the virtual machine under lima before packaging to avoid failure midway. When m3 max is allocated 4 cores, Release takes about 1 hour and Debug takes about 2.5 hours.

After installing unzip, git, cmake, g++, python3.11 and rsync, we clone [https://github.com/nodejs-mobile/nodejs-mobile](https://github.com/nodejs-mobile/nodejs-mobile), download NDK r24 (linux) from [https://github.com/android/ndk/wiki/Unsupported-Downloads](https://github.com/android/ndk/wiki/Unsupported-Downloads), and decompress it directly (for example, decompress it to `android_r24_ndk`).

```shell
$ cd nodejs-mobile
$ ./tools/android_build.sh /path/to/android/ndk 24 arm64
```

Then wait for a long time. After success, you can find `libnode.so` under `out/Release/`. It took about 1 hour on M3 Max.

After packaging, we need to export it in lima through nerdctl.

```shell
@lima-ubuntu-22.04:/Users/user$ nerdctl cp ubuntu-e00a6:/root/develop/nodejs-mobile/out/Release/libnode.so /tmp/lima/
```

The path `/tmp/lima/` is used because in the default configuration, other paths in the host machine are not writable, but this path is writable.

If you need to install the Debug package, in `android_configure.py`, add `--debug` after `./configure`

```python
if os.path.exists("./configure"):
# nodejs-mobile patch: added --with-intl=none and --shared
os.system("./configure --debug --dest-cpu=" + DEST_CPU + " --dest-os=android --openssl-no-asm --with-intl=none --cross-compiling --shared")
```
