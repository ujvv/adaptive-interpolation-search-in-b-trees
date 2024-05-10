set -e
set -x

make clean
make main-optimized


INT=100000 ./main-optimized
INT=500000 ./main-optimized
INT=1e6 ./main-optimized
INT=5000000 ./main-optimized
INT=7000000 ./main-optimized