set -e
set -x

make clean
make main-optimized

LONG1=512 ./main-optimized
LONG2=512 ./main-optimized