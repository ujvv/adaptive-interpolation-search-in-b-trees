set -e
set -x

make clean
make main-optimized

BINARY=0 INTUNIFORM=1e3 ./main-optimized
BINARY=0 INTUNIFORM=1e4 ./main-optimized
BINARY=0 INTUNIFORM=2e4 ./main-optimized
BINARY=0 INTUNIFORM=3e4 ./main-optimized
BINARY=0 INTUNIFORM=4e4 ./main-optimized
BINARY=0 INTUNIFORM=5e4 ./main-optimized
BINARY=0 INTUNIFORM=6e4 ./main-optimized
BINARY=0 INTUNIFORM=7e4 ./main-optimized
BINARY=0 INTUNIFORM=8e4 ./main-optimized
BINARY=0 INTUNIFORM=9e4 ./main-optimized
BINARY=0 INTUNIFORM=1e5 ./main-optimized
BINARY=0 INTUNIFORM=2e5 ./main-optimized
BINARY=0 INTUNIFORM=4e5 ./main-optimized
BINARY=0 INTUNIFORM=4e5 ./main-optimized
BINARY=0 INTUNIFORM=5e5 ./main-optimized
BINARY=0 INTUNIFORM=6e5 ./main-optimized
BINARY=0 INTUNIFORM=7e5 ./main-optimized
BINARY=0 INTUNIFORM=8e5 ./main-optimized
BINARY=0 INTUNIFORM=9e5 ./main-optimized
BINARY=0 INTUNIFORM=1e6 ./main-optimized
BINARY=0 INTUNIFORM=2e6 ./main-optimized
BINARY=0 INTUNIFORM=3e6 ./main-optimized
BINARY=0 INTUNIFORM=4e6 ./main-optimized
BINARY=0 INTUNIFORM=5e6 ./main-optimized
BINARY=0 INTUNIFORM=6e6 ./main-optimized
BINARY=0 INTUNIFORM=7e6 ./main-optimized
BINARY=0 INTUNIFORM=8e6 ./main-optimized
BINARY=0 INTUNIFORM=9e6 ./main-optimized
BINARY=0 INTUNIFORM=1e7 ./main-optimized
BINARY=0 INTUNIFORM=2e7 ./main-optimized
BINARY=0 INTUNIFORM=3e7 ./main-optimized
BINARY=0 INTUNIFORM=4e7 ./main-optimized
BINARY=0 INTUNIFORM=5e7 ./main-optimized
BINARY=0 INTUNIFORM=6e7 ./main-optimized
BINARY=0 INTUNIFORM=7e7 ./main-optimized
BINARY=0 INTUNIFORM=8e7 ./main-optimized
BINARY=0 INTUNIFORM=9e7 ./main-optimized
BINARY=0 INTUNIFORM=1e8 ./main-optimized
BINARY=0 INTUNIFORM=2e8 ./main-optimized
BINARY=0 INTUNIFORM=3e8 ./main-optimized
BINARY=0 INTUNIFORM=4e8 ./main-optimized
BINARY=0 INTUNIFORM=5e8 ./main-optimized

BINARY=0 BYTE=1e3 BYTESIZE=8 ./main-optimized
BINARY=0 BYTE=1e3 BYTESIZE=32 ./main-optimized
BINARY=0 BYTE=1e3 BYTESIZE=128 ./main-optimized
BINARY=0 BYTE=1e4 BYTESIZE=8 ./main-optimized
BINARY=0 BYTE=1e4 BYTESIZE=32 ./main-optimized
BINARY=0 BYTE=1e4 BYTESIZE=128 ./main-optimized
BINARY=0 BYTE=1e5 BYTESIZE=8 ./main-optimized
BINARY=0 BYTE=1e5 BYTESIZE=32 ./main-optimized
BINARY=0 BYTE=1e5 BYTESIZE=128 ./main-optimized
BINARY=0 BYTE=1e6 BYTESIZE=8 ./main-optimized
BINARY=0 BYTE=1e6 BYTESIZE=32 ./main-optimized
BINARY=0 BYTE=1e6 BYTESIZE=128 ./main-optimized
BINARY=0 BYTE=1e7 BYTESIZE=8 ./main-optimized
BINARY=0 BYTE=1e7 BYTESIZE=32 ./main-optimized
BINARY=0 BYTE=1e7 BYTESIZE=128 ./main-optimized

BINARY=0 VARIABLEBYTE=1e3 ./main-optimized
BINARY=0 VARIABLEBYTE=1e4 ./main-optimized
BINARY=0 VARIABLEBYTE=1e5 ./main-optimized
BINARY=0 VARIABLEBYTE=1e6 ./main-optimized
BINARY=0 VARIABLEBYTE=1e7 ./main-optimized

BINARY=0 FILE=data/urls ./main-optimized