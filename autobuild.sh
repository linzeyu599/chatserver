#########################################################################
# File Name: autobuild.sh
# Author: linzeyu
# mail: linzeyu599@163.com
# Created Time: 2021年08月08日 星期日 22时00分00秒
#########################################################################
#!/bin/bash

set -x

rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make