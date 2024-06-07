set -e
set -x

make clean
make main-optimized

BINARY=0 INTUNIFORM=1e5 ./main-optimized
BINARY=0 INTUNIFORM=2e5 ./main-optimized
BINARY=0 INTUNIFORM=5e7 ./main-optimized
BINARY=0 FILE=data/urls ./main-optimized