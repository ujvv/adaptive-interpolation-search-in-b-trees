set -e
set -x

make clean
make main-optimized

INT=5e7 ./main-optimized
FILE=data/urls ./main-optimized