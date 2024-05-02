set -e
set -x

make clean
make main-optimized

INT=1000  ./main-optimized
INT=1e6  ./main-optimized
LONG1=500 ./main-optimized
LONG2=500 ./main-optimized
FILE=data/urls ./main-optimized