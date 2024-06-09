set -e
set -x

make clean
make main

ANALYSIS=0 INTUNIFORM=1e3 ./main
ANALYSIS=0 INTUNIFORM=1e4 ./main
ANALYSIS=0 INTUNIFORM=1e5 ./main
ANALYSIS=0 INTUNIFORM=1e6 ./main
ANALYSIS=0 INTUNIFORM=1e7 ./main

ANALYSIS=0 INTLITTLEENDIAN=1e6 ./main
ANALYSIS=0 INTLITTLEENDIAN=1e7 ./main
ANALYSIS=0 INTLITTLEENDIAN=2e7 ./main
ANALYSIS=0 INTLITTLEENDIAN=3e7 ./main
ANALYSIS=0 INTLITTLEENDIAN=4e7 ./main
ANALYSIS=0 INTLITTLEENDIAN=5e7 ./main

ANALYSIS=0 INTRANDOM=1e4 INTRANDOMSEED=30 ./main
ANALYSIS=0 INTRANDOM=1e5 INTRANDOMSEED=30 ./main
ANALYSIS=0 INTRANDOM=1e6 INTRANDOMSEED=30 ./main
ANALYSIS=0 INTRANDOM=1e7 INTRANDOMSEED=30 ./main
ANALYSIS=0 INTRANDOM=2e7 INTRANDOMSEED=30 ./main
ANALYSIS=0 INTRANDOM=3e7 INTRANDOMSEED=30 ./main
ANALYSIS=0 INTRANDOM=4e7 INTRANDOMSEED=30 ./main
ANALYSIS=0 INTRANDOM=5e7 INTRANDOMSEED=30 ./main
ANALYSIS=0 INTRANDOM=1e4 INTRANDOMSEED=66 ./main
ANALYSIS=0 INTRANDOM=1e5 INTRANDOMSEED=66 ./main
ANALYSIS=0 INTRANDOM=1e6 INTRANDOMSEED=66 ./main
ANALYSIS=0 INTRANDOM=1e7 INTRANDOMSEED=66 ./main
ANALYSIS=0 INTRANDOM=2e7 INTRANDOMSEED=66 ./main
ANALYSIS=0 INTRANDOM=3e7 INTRANDOMSEED=66 ./main
ANALYSIS=0 INTRANDOM=4e7 INTRANDOMSEED=66 ./main
ANALYSIS=0 INTRANDOM=5e7 INTRANDOMSEED=66 ./main


ANALYSIS=0 INTFAL=1e4 FALSHAPE=50 ./main
ANALYSIS=0 INTFAL=1e4 FALSHAPE=105 ./main
ANALYSIS=0 INTFAL=1e4 FALSHAPE=125 ./main
ANALYSIS=0 INTFAL=1e4 FALSHAPE=150 ./main
ANALYSIS=0 INTFAL=1e5 FALSHAPE=50 ./main
ANALYSIS=0 INTFAL=1e5 FALSHAPE=105 ./main
ANALYSIS=0 INTFAL=1e5 FALSHAPE=125 ./main
ANALYSIS=0 INTFAL=1e5 FALSHAPE=150 ./main
ANALYSIS=0 INTFAL=1e6 FALSHAPE=50 ./main
ANALYSIS=0 INTFAL=1e6 FALSHAPE=105 ./main
ANALYSIS=0 INTFAL=1e6 FALSHAPE=125 ./main
ANALYSIS=0 INTFAL=1e6 FALSHAPE=150 ./main
ANALYSIS=0 INTFAL=1e7 FALSHAPE=50 ./main
ANALYSIS=0 INTFAL=1e7 FALSHAPE=105 ./main
ANALYSIS=0 INTFAL=1e7 FALSHAPE=125 ./main
ANALYSIS=0 INTFAL=1e7 FALSHAPE=150 ./main
ANALYSIS=0 INTFAL=2e7 FALSHAPE=50 ./main
ANALYSIS=0 INTFAL=2e7 FALSHAPE=105 ./main
ANALYSIS=0 INTFAL=2e7 FALSHAPE=125 ./main
ANALYSIS=0 INTFAL=2e7 FALSHAPE=150 ./main
ANALYSIS=0 INTFAL=3e7 FALSHAPE=50 ./main
ANALYSIS=0 INTFAL=3e7 FALSHAPE=105 ./main
ANALYSIS=0 INTFAL=3e7 FALSHAPE=125 ./main
ANALYSIS=0 INTFAL=3e7 FALSHAPE=150 ./main
ANALYSIS=0 INTFAL=4e7 FALSHAPE=50 ./main
ANALYSIS=0 INTFAL=4e7 FALSHAPE=105 ./main
ANALYSIS=0 INTFAL=4e7 FALSHAPE=125 ./main
ANALYSIS=0 INTFAL=4e7 FALSHAPE=150 ./main
ANALYSIS=0 INTFAL=5e7 FALSHAPE=50 ./main
ANALYSIS=0 INTFAL=5e7 FALSHAPE=105 ./main
ANALYSIS=0 INTFAL=5e7 FALSHAPE=125 ./main
ANALYSIS=0 INTFAL=5e7 FALSHAPE=150 ./main

ANALYSIS=0 INTCFAL=1e4 CFALSHAPE=50 ./main
ANALYSIS=0 INTCFAL=1e4 CFALSHAPE=105 ./main
ANALYSIS=0 INTCFAL=1e4 CFALSHAPE=125 ./main
ANALYSIS=0 INTCFAL=1e4 CFALSHAPE=150 ./main
ANALYSIS=0 INTCFAL=1e5 CFALSHAPE=50 ./main
ANALYSIS=0 INTCFAL=1e5 CFALSHAPE=105 ./main
ANALYSIS=0 INTCFAL=1e5 CFALSHAPE=125 ./main
ANALYSIS=0 INTCFAL=1e5 CFALSHAPE=150 ./main
ANALYSIS=0 INTCFAL=1e6 CFALSHAPE=50 ./main
ANALYSIS=0 INTCFAL=1e6 CFALSHAPE=105 ./main
ANALYSIS=0 INTCFAL=1e6 CFALSHAPE=125 ./main
ANALYSIS=0 INTCFAL=1e6 CFALSHAPE=150 ./main
ANALYSIS=0 INTCFAL=1e7 CFALSHAPE=50 ./main
ANALYSIS=0 INTCFAL=1e7 CFALSHAPE=105 ./main
ANALYSIS=0 INTCFAL=1e7 CFALSHAPE=125 ./main
ANALYSIS=0 INTCFAL=1e7 CFALSHAPE=150 ./main
ANALYSIS=0 INTCFAL=2e7 CFALSHAPE=50 ./main
ANALYSIS=0 INTCFAL=2e7 CFALSHAPE=105 ./main
ANALYSIS=0 INTCFAL=2e7 CFALSHAPE=125 ./main
ANALYSIS=0 INTCFAL=2e7 CFALSHAPE=150 ./main
ANALYSIS=0 INTCFAL=3e7 CFALSHAPE=50 ./main
ANALYSIS=0 INTCFAL=3e7 CFALSHAPE=105 ./main
ANALYSIS=0 INTCFAL=3e7 CFALSHAPE=125 ./main
ANALYSIS=0 INTCFAL=3e7 CFALSHAPE=150 ./main
ANALYSIS=0 INTCFAL=4e7 CFALSHAPE=50 ./main
ANALYSIS=0 INTCFAL=4e7 CFALSHAPE=105 ./main
ANALYSIS=0 INTCFAL=4e7 CFALSHAPE=125 ./main
ANALYSIS=0 INTCFAL=4e7 CFALSHAPE=150 ./main
ANALYSIS=0 INTCFAL=5e7 CFALSHAPE=50 ./main
ANALYSIS=0 INTCFAL=5e7 CFALSHAPE=105 ./main
ANALYSIS=0 INTCFAL=5e7 CFALSHAPE=125 ./main
ANALYSIS=0 INTCFAL=5e7 CFALSHAPE=150 ./main


ANALYSIS=0 BYTE=1e3 BYTESIZE=8 ./main
ANALYSIS=0 BYTE=1e3 BYTESIZE=32 ./main
ANALYSIS=0 BYTE=1e3 BYTESIZE=128 ./main
ANALYSIS=0 BYTE=1e4 BYTESIZE=8 ./main
ANALYSIS=0 BYTE=1e4 BYTESIZE=32 ./main
ANALYSIS=0 BYTE=1e4 BYTESIZE=128 ./main
ANALYSIS=0 BYTE=1e5 BYTESIZE=8 ./main
ANALYSIS=0 BYTE=1e5 BYTESIZE=32 ./main
ANALYSIS=0 BYTE=1e5 BYTESIZE=128 ./main
ANALYSIS=0 BYTE=1e6 BYTESIZE=8 ./main
ANALYSIS=0 BYTE=1e6 BYTESIZE=32 ./main
ANALYSIS=0 BYTE=1e6 BYTESIZE=128 ./main
#ANALYSIS=0 BYTE=1e7 BYTESIZE=8 ./main
#ANALYSIS=0 BYTE=1e7 BYTESIZE=32 ./main
#ANALYSIS=0 BYTE=1e7 BYTESIZE=128 ./main
#ANALYSIS=0 BYTE=2e7 BYTESIZE=8 ./main
#ANALYSIS=0 BYTE=2e7 BYTESIZE=32 ./main #HIER
#ANALYSIS=0 BYTE=2e7 BYTESIZE=128 ./main
#ANALYSIS=0 BYTE=3e7 BYTESIZE=8 ./main
#ANALYSIS=0 BYTE=3e7 BYTESIZE=32 ./main
#ANALYSIS=0 BYTE=3e7 BYTESIZE=128 ./main
#ANALYSIS=0 BYTE=4e7 BYTESIZE=8 ./main
#ANALYSIS=0 BYTE=4e7 BYTESIZE=32 ./main
#ANALYSIS=0 BYTE=4e7 BYTESIZE=128 ./main
#ANALYSIS=0 BYTE=5e7 BYTESIZE=8 ./main
#ANALYSIS=0 BYTE=5e7 BYTESIZE=32 ./main
#ANALYSIS=0 BYTE=5e7 BYTESIZE=128 ./main

ANALYSIS=0 VARIABLEBYTE=1e3 ./main
ANALYSIS=0 VARIABLEBYTE=1e4 ./main
ANALYSIS=0 VARIABLEBYTE=1e5 ./main
ANALYSIS=0 VARIABLEBYTE=1e6 ./main
#ANALYSIS=0 VARIABLEBYTE=1e7 ./main
#ANALYSIS=0 VARIABLEBYTE=2e7 ./main
#ANALYSIS=0 VARIABLEBYTE=3e7 ./main
#ANALYSIS=0 VARIABLEBYTE=4e7 ./main

ANALYSIS=0 FILE=data/urls ./main