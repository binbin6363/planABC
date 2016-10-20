MAKEFILE_TIME=$(date)
USER_BUILD=$(whoami)
MACHINE_IP=$(/sbin/ifconfig | grep "inet addr" | grep -v "127.0.0.1" | awk -F"inet addr:" '{print $2}' | awk '{print $1}')
GCC_VERSION=$(gcc --version | head -n 1 | sed 's/[()]//g')

#SVN_VERSION=$(svn info | grep Revision | awk '{print $2}')
SVN_VERSION=$(svn log 2>/dev/null|grep -v '\-\-\-'|head -n 1|awk -F'|' '{print $1}')
if [ ! "$SVN_VERSION" ]; then 
    SVN_VERSION=$(date +%s)
fi

MAKE_VERSION=1

if [ -f ".makev" ]; then 
    MAKE_VERSION=$(cat .makev) 
    MAKE_VERSION=$((MAKE_VERSION+1))
fi
echo $MAKE_VERSION > .makev

SERVER_VERSION="$SVN_VERSION.$MAKE_VERSION"

OUT="ADD_DEFINITIONS(-DMAKEFILE_TIME=\"$MAKEFILE_TIME\" -DUSER_BUILD=\"$USER_BUILD\" -DMACHINE_IP=\"$MACHINE_IP\" -DGCC_VERSION=\"$GCC_VERSION\" -DSERVER_VERSION=\"$SERVER_VERSION\")" 

echo $OUT > versioninfo.cmake
