#!/bin/sh
#desc: Measure system resources
#package:odcommon
#type:local
H=`date +%H`
if ! test -d /var/log/stats; then 
	mkdir -p /var/log/stats
fi
/usr/local/bin/get_load_avg.sh >>/var/log/stats/loadcpu.h$H
/usr/local/bin/get_mem_free.sh >>/var/log/stats/loadmem.h$H
exit 0
