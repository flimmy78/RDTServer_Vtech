#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <iostream>

#include "VtechIPHubGatewayDevice.hpp"

using namespace std;

#define PP_NARG(...) \
    PP_NARG_(__VA_ARGS__,PP_RSEQ_N())

#define PP_NARG_(...) \
    PP_ARG_N(__VA_ARGS__)

#define PP_ARG_N( \
    _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
    _61,_62,_63,N,...) N

#define PP_RSEQ_N() \
    63,62,61,60,                   \
    59,58,57,56,55,54,53,52,51,50, \
    49,48,47,46,45,44,43,42,41,40, \
    39,38,37,36,35,34,33,32,31,30, \
    29,28,27,26,25,24,23,22,21,20, \
    19,18,17,16,15,14,13,12,11,10, \
    9,8,7,6,5,4,3,2,1,0

#define N(...) \
    N(__VA_ARGS__)

int main(int argc, char *argv[])
{
    LOGD("uid = %s", argv[1]);
    
    {
//        PP_NARG(A) -> 1
//        PP_NARG(A,B) -> 2
//        PP_NARG(A,B,C) -> 3
//        PP_NARG(A,B,C,D) -> 4
//        PP_NARG(A,B,C,D,E) -> 5
//        PP_NARG(1,2,3,4,5,6,7,8,9,0,
//                1,2,3,4,5,6,7,8,9,0,
//                1,2,3,4,5,6,7,8,9,0,
//                1,2,3,4,5,6,7,8,9,0,
//                1,2,3,4,5,6,7,8,9,0,
//                1,2,3,4,5,6,7,8,9,0,
//                1,2,3) -> 63
        
//        N(a, b);
    }
    
    VtechIPHubGatewayDevice device;
    device.run(argv[1]);
    
	return 0;
}
