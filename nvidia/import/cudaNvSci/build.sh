rm ./cudaNvSci
make TARGET_ARCH=aarch64  dbg=1
/usr/local/cuda/bin/nvcc -ccbin g++   -m64 -g -G -O0      -o cudaNvSci cudaNvSci.o imageKernels.o main.o  -L/usr/lib/aarch64-linux-gnu/tegra/ -lcuda -lnvscibuf -lnvscisync

