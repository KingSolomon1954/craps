#!/usr/bin/env bash
#
# -----------------------------------------------------------
#
# Performs a minimal checkout of the tar ball.
#
# -----------------------------------------------------------

# $1 = build directory   _build
# $2 = tar-file          _build/release/RoyalCraps-0.0.3.tgz

b=$(basename $2)         # RoyalCraps-0.0.3.tgz
release="${b%%".tgz"}"   # strip .tgz

testdir=$1/test-tarball
rm -rf   $testdir
mkdir -p $testdir
tar -xzf $2 --directory=$testdir

APP_EXEC=$(grep "APP_EXEC" $1/../Makefile | awk '{ print $3}' )

releaseDir=$testdir/$release

[ ! -d $releaseDir ]                && exit 1
[ ! -x $releaseDir/${APP_EXEC} ]    && exit 1
[ ! -x $releaseDir/${APP_EXEC}-db ] && exit 1
[ ! -f $releaseDir/version ]        && exit 1
[ ! -f $releaseDir/changelog.md ]   && exit 1

exit 0
