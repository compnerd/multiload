#!/bin/bash
# Copyright 2015 Saleem Abdulrasool <compnerd@compnerd.org>

: ${PROJECT:=multiload}

: ${ACLOCAL:=aclocal}
: ${AUTOCONF:=autoconf}
: ${AUTOMAKE:=automake}
: ${LIBTOOLIZE:=libtoolize}

run()
{
  echo >&2 ">>> ${@}"
  if ! ${@} ; then
    echo >&2 "failed to execute \`${@}\'"
    exit 127
  fi
}

srcdir=$(dirname "${0}")
[[ -n ${srcdir} ]] || srcdir='.'

( "${AUTOCONF}" --version ) </dev/null >/dev/null 2>&1 || {
  echo
  echo "You must have autoconf installed to compile ${PROJECT}."
  echo "Download the appropriate package for your distribution,"
  echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
  echo

  exit 127
}

( "${AUTOMAKE}" --version ) </dev/null >/dev/null 2>&1 || {
  echo
  echo "You must have automake installed to compile ${PROJECT}."
  echo "Download the appropriate package for your distribution,"
  echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
  echo

  exit 127
}

run mkdir -p "${srcdir}/config"
rm -f config.cache
run "${ACLOCAL}"
run "${LIBTOOLIZE}" --copy --quiet
run "${AUTOCONF}"
run "${AUTOMAKE}" --add-missing --copy --foreign

