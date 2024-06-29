set -e
set -x

make clean
make main-optimized

ISS=0 INTUNIFORM=1e7 ./main-optimized
ISS=0 INTUNIFORM=2e7 ./main-optimized
ISS=0 INTLITTLEENDIAN=1e7 ./main-optimized
ISS=0 INTLITTLEENDIAN=2e7 ./main-optimized
ISS=0 INTRANDOM=1e7 INTRANDOMSEED=30 ./main-optimized
ISS=0 INTRANDOM=2e7 INTRANDOMSEED=30 ./main-optimized
ISS=0 INTCFAL=1e7 CFALSHAPE=50 ./main-optimized
ISS=0 INTCFAL=2e7 CFALSHAPE=50 ./main-optimized
ISS=0 INTCFAL=1e7 CFALSHAPE=105 ./main-optimized
ISS=0 INTCFAL=2e7 CFALSHAPE=105 ./main-optimized