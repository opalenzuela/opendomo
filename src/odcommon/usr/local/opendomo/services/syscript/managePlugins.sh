#!/bin/sh
#type: local
#package: odcommon

TMPDIR="/var/opendomo/tmp"
OSVER=`cat /etc/VERSION`
REPOSITORY="https://cloud.opendomo.com/repo/$OSVER/"
if ! test -f $TMPDIR/repo.lst; then
  wget $REPOSITORY -O $TMPDIR/repo.lst
fi
if test -z "$1"; then
  echo "list:managePlugins.sh"
  cat $TMPDIR/repo.lst | cut -f1 -d';' | head -n1 |
  echo

#echo "actions:"
#echo "   managePlugins.sh   Details"
#echo

else
  # Parameter was passed (requesting plugin's details)
  URL = `grep ^$1 $TMPDIR/repo.lst | sort | head -n1 | cut -f2 -d';' `
  DESC = `grep ^$1 $TMPDIR/repo.lst | sort | head -n1 | cut -f3 -d';' `
  DEPS = `grep ^$1 $TMPDIR/repo.lst | sort | head -n1 | cut -f4 -d';' `
  echo "form:managePlugins.sh"
  echo "   code   Code   $1  readonly"
  echo "   desc   Description   $DESC  readonly"
  echo "   deps   Dependences   $DEPS   readonly"
  echo "   icon    Icon     $1.png  image"
  echo
  echo
  echo "actions:"
  if test -f /var/opendomo/plugins/$1; then
     echo "    installPlugin.sh  Install"
  else
     echo "    removePlugin.sh  Uninstall"
  fi
fi
