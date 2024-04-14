set -e
set -x

make clean
make main-optimized

INT=5e7 FILE=data/urls ./main-optimized