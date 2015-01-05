#!/bin/sh

# Common info
MEMINFO="/proc/meminfo"
PKGSTMP="/mnt/tmpfs"

# Memory info
TOTALMEMORY=`grep -m1 MemTotal $MEMINFO | sed 's/[^0-9]//g'`
REALMEMFREE=`grep -m1 MemFree  $MEMINFO | sed 's/[^0-9]//g'`
SYSTEMCACHE=`grep -m1 Cached   $MEMINFO | sed 's/[^0-9]//g'`

# Changes memory info
if grep -c1 aufs=no "/proc/cmdline" &>/dev/null; then
    # System is in white mode, no memory limited
    CHNGMEMFREE="20000000"
else
    # Normal mode
    USEDCHNGMEM=`du $PKGSTMP 2>/dev/null | tail -n1 | awk {'print$1'}`
    let CHNGMEMTOTAL="$TOTALMEMORY / 2"
    let CHNGMEMFREE="$CHNGMEMTOTAL - $USEDCHNGMEM"
fi

# See results
echo "TOTAL_MEMORY=$TOTALMEMORY"
echo "REAL_MEMORY_FREE=$REALMEMFREE"
echo "TOTAL_CACHED_MEMORY=$SYSTEMCACHE"
echo "PACKAGE_MEMORY_TOTAL=$CHNGMEMTOTAL"
echo "PACKAGE_MEMORY_USED=$USEDCHNGMEM"
echo "PACKAGE_MEMORY_FREE=$CHNGMEMFREE"
