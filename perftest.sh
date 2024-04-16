set -e
set -x

make clean
make main-optimized

FILE=data/urls ./main-optimized