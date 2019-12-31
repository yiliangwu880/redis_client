#!/bin/sh
# make clean, make操作

#cp acc
cp ./external/access_svr/bin/acc_svr ./bin/acc1/acc_svr
cp ./external/access_svr/bin/acc_svr ./bin/acc2/acc_svr
cp ./external/access_svr/bin/acc_svr ./bin/acc3/acc_svr
cp ./external/access_svr/bin/acc_svr ./bin/acc4/acc_svr
cp ./external/access_svr/bin/acc_svr ./bin/acc5/acc_svr

cp ./external/msg_forward/bin/mf_svr ./bin/msg_forward1/mf_svr
cp ./external/msg_forward/bin/mf_svr ./bin/msg_forward2/mf_svr

mkdir -p Debug
cd Debug
rm * -rf
cd ..
sh build.sh

