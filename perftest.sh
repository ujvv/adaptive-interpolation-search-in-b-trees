set -e
set -x

make clean
make main-optimized

INT=1000  ./main-optimized
INT=234567 ./main-optimized
INT=1e6  ./main-optimized
INT=2000000 ./main-optimized
LONG1=500 ./main-optimized
LONG2=500 ./main-optimized
FILE=data/urls ./main-optimized
VARIABLEBYTE=20000 ./main-optimized
VARIABLEBYTE=100000 ./main-optimized
VARIABLEBYTE=1000000 ./main-optimized