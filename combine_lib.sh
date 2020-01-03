#!/bin/sh
#合并静态库
echo combine lib
mkdir -p tmp
cd tmp
ar x ../lib/libredis_client.a
ar x ../external/hiredis-0.14.0/libhiredis.a
ar cru libcredis_client.a *.o
ranlib libcredis_client.a
cp libcredis_client.a ../lib/
rm ../lib/libredis_client.a



