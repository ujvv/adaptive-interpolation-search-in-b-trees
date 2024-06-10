set -e
set -x

make clean
make main-optimized

AS=0 INTRANDOM=4e7 INTRANDOMSEED=30 ./main-optimized
AS=0 INTRANDOM=5e7 INTRANDOMSEED=30 ./main-optimized
AS=0 INTRANDOM=1e4 INTRANDOMSEED=66 ./main-optimized
AS=0 INTRANDOM=1e5 INTRANDOMSEED=66 ./main-optimized
AS=0 INTRANDOM=1e6 INTRANDOMSEED=66 ./main-optimized
AS=0 INTRANDOM=1e7 INTRANDOMSEED=66 ./main-optimized
AS=0 INTRANDOM=2e7 INTRANDOMSEED=66 ./main-optimized
AS=0 INTRANDOM=3e7 INTRANDOMSEED=66 ./main-optimized
AS=0 INTRANDOM=4e7 INTRANDOMSEED=66 ./main-optimized
AS=0 INTRANDOM=5e7 INTRANDOMSEED=66 ./main-optimized


AS=0 INTFAL=1e4 FALSHAPE=50 ./main-optimized
AS=0 INTFAL=1e4 FALSHAPE=105 ./main-optimized
AS=0 INTFAL=1e4 FALSHAPE=125 ./main-optimized
AS=0 INTFAL=1e4 FALSHAPE=150 ./main-optimized
AS=0 INTFAL=1e5 FALSHAPE=50 ./main-optimized
AS=0 INTFAL=1e5 FALSHAPE=105 ./main-optimized
AS=0 INTFAL=1e5 FALSHAPE=125 ./main-optimized
AS=0 INTFAL=1e5 FALSHAPE=150 ./main-optimized
AS=0 INTFAL=1e6 FALSHAPE=50 ./main-optimized
AS=0 INTFAL=1e6 FALSHAPE=105 ./main-optimized
AS=0 INTFAL=1e6 FALSHAPE=125 ./main-optimized
AS=0 INTFAL=1e6 FALSHAPE=150 ./main-optimized
AS=0 INTFAL=1e7 FALSHAPE=50 ./main-optimized
AS=0 INTFAL=1e7 FALSHAPE=105 ./main-optimized
AS=0 INTFAL=1e7 FALSHAPE=125 ./main-optimized
AS=0 INTFAL=1e7 FALSHAPE=150 ./main-optimized
AS=0 INTFAL=2e7 FALSHAPE=50 ./main-optimized
AS=0 INTFAL=2e7 FALSHAPE=105 ./main-optimized
AS=0 INTFAL=2e7 FALSHAPE=125 ./main-optimized
AS=0 INTFAL=2e7 FALSHAPE=150 ./main-optimized
AS=0 INTFAL=3e7 FALSHAPE=50 ./main-optimized
AS=0 INTFAL=3e7 FALSHAPE=105 ./main-optimized
AS=0 INTFAL=3e7 FALSHAPE=125 ./main-optimized
AS=0 INTFAL=3e7 FALSHAPE=150 ./main-optimized
AS=0 INTFAL=4e7 FALSHAPE=50 ./main-optimized
AS=0 INTFAL=4e7 FALSHAPE=105 ./main-optimized
AS=0 INTFAL=4e7 FALSHAPE=125 ./main-optimized
AS=0 INTFAL=4e7 FALSHAPE=150 ./main-optimized
AS=0 INTFAL=5e7 FALSHAPE=50 ./main-optimized
AS=0 INTFAL=5e7 FALSHAPE=105 ./main-optimized
AS=0 INTFAL=5e7 FALSHAPE=125 ./main-optimized
AS=0 INTFAL=5e7 FALSHAPE=150 ./main-optimized

AS=0 INTCFAL=1e4 CFALSHAPE=50 ./main-optimized
AS=0 INTCFAL=1e4 CFALSHAPE=105 ./main-optimized
AS=0 INTCFAL=1e4 CFALSHAPE=125 ./main-optimized
AS=0 INTCFAL=1e4 CFALSHAPE=150 ./main-optimized
AS=0 INTCFAL=1e5 CFALSHAPE=50 ./main-optimized
AS=0 INTCFAL=1e5 CFALSHAPE=105 ./main-optimized
AS=0 INTCFAL=1e5 CFALSHAPE=125 ./main-optimized
AS=0 INTCFAL=1e5 CFALSHAPE=150 ./main-optimized
AS=0 INTCFAL=1e6 CFALSHAPE=50 ./main-optimized
AS=0 INTCFAL=1e6 CFALSHAPE=105 ./main-optimized
AS=0 INTCFAL=1e6 CFALSHAPE=125 ./main-optimized
AS=0 INTCFAL=1e6 CFALSHAPE=150 ./main-optimized
AS=0 INTCFAL=1e7 CFALSHAPE=50 ./main-optimized
AS=0 INTCFAL=1e7 CFALSHAPE=105 ./main-optimized
AS=0 INTCFAL=1e7 CFALSHAPE=125 ./main-optimized
AS=0 INTCFAL=1e7 CFALSHAPE=150 ./main-optimized
AS=0 INTCFAL=2e7 CFALSHAPE=50 ./main-optimized
AS=0 INTCFAL=2e7 CFALSHAPE=105 ./main-optimized
AS=0 INTCFAL=2e7 CFALSHAPE=125 ./main-optimized
AS=0 INTCFAL=2e7 CFALSHAPE=150 ./main-optimized
AS=0 INTCFAL=3e7 CFALSHAPE=50 ./main-optimized
AS=0 INTCFAL=3e7 CFALSHAPE=105 ./main-optimized
AS=0 INTCFAL=3e7 CFALSHAPE=125 ./main-optimized
AS=0 INTCFAL=3e7 CFALSHAPE=150 ./main-optimized
AS=0 INTCFAL=4e7 CFALSHAPE=50 ./main-optimized
AS=0 INTCFAL=4e7 CFALSHAPE=105 ./main-optimized
AS=0 INTCFAL=4e7 CFALSHAPE=125 ./main-optimized
AS=0 INTCFAL=4e7 CFALSHAPE=150 ./main-optimized
AS=0 INTCFAL=5e7 CFALSHAPE=50 ./main-optimized
AS=0 INTCFAL=5e7 CFALSHAPE=105 ./main-optimized
AS=0 INTCFAL=5e7 CFALSHAPE=125 ./main-optimized
AS=0 INTCFAL=5e7 CFALSHAPE=150 ./main-optimized