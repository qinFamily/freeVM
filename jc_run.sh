#!/bin/bash

export JCHEVM_HOME=/home/ivan/experiments/harmony/jchevm/inst
export CLASSLIB_HOME=/home/ivan/experiments/harmony/CLASSLIB/deploy/jdk/jre
ADAPTER=$(dirname $0)

JC=$JCHEVM_HOME/bin/jc
JCZIP=$JCHEVM_HOME/share/jc/jc.zip
KERNEL=$ADAPTER/modules/kernel/src/main/java
NIO=$ADAPTER/modules/nio/src/main/java
LUNI=$ADAPTER/modules/luni/src/main/java
ADAPTER_JARS=$KERNEL:$NIO
CLASSLIB_JARS=$(ls $CLASSLIB_HOME/lib/boot/*.jar $CLASSLIB_HOME/lib/boot/*/*.jar | tr '\n' ':')

BOOT=$JCZIP:$ADAPTER_JARS:$CLASSLIB_JARS:$HOME/experiments/harmony/gnuclasspathadapter/ivan_classes

ADAPTER_LIBS=$ADAPTER
CLASSLIB_LIBS=$CLASSLIB_HOME/bin
LIB_PATH=$ADAPTER_LIBS:$CLASSLIB_LIBS

export LD_LIBRARY_PATH=$ADAPTER_LIBS:$CLASSLIB_LIBS:$LD_LIBRARY_PATH

#gdb --args \
$JC -Djava.library.path=$LIB_PATH -Djava.boot.class.path=$BOOT "$@"
