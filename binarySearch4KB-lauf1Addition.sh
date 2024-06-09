set -e
set -x

make clean
make main-optimized

BINARY=0 VARIABLEBYTE=5e7 ./main-optimized