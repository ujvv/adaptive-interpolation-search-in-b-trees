set -e
set -x

make clean
make main-optimized


#INTUNIFORM=100000 ./main-optimized
#INTUNIFORM=500000 ./main-optimized
#INTUNIFORM=1e6 ./main-optimized
#INTUNIFORM=5000000 ./main-optimized
#INTUNIFORM=7000000 ./main-optimized
INTUNIFORM=20000000 ./main-optimized