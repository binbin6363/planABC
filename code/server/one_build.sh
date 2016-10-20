
echo "start one build script ..."

workspace=/home/bbwang/workspace/project/code/server
contri=$workspace/contri
mobiled=$workspace/mobiled/trunk/src
user_center=$workspace/user_center/trunk/src
task_center=$workspace/task_center/trunk/src
dbagent=$workspace/dbagent/trunk/src
dbp=$workspace/dbp/trunk/src
leveldb_server=$workspace/leveldb_server/src
version_log=$workspace/version.log

rm -f $version_log
touch  $version_log

echo "## 1. build comm #############################################"
pushd $contri/comm
svn up
make clean
make
popd

echo "## 2. build utils #############################################"
pushd $contri/utils
svn up
make clean
make
popd

echo "## 3. build net #############################################"
pushd $contri/net
svn up
sh autogen.sh
sh configure
make
popd

echo "## 4. build coroutine #############################################"
pushd $contri/utils
svn up
make clean
make
popd

echo "## 5. build proto #############################################"
pushd $contri/proto_src
svn up
make clean
make
popd

echo "## 6. build mobiled #############################################"
pushd $mobiled
svn  up
make clean
sh build.sh
echo "mobiled" >> $version_log
cat versioninfo.cmake >> $version_log
cmake .
make
popd

echo "## 7. build user_center #############################################"
pushd $user_center
svn  up
make clean
sh build.sh
echo "user_center" >> $version_log
cat versioninfo.cmake >> $version_log
cmake .
make
popd

echo "## 8. build task_center #############################################"
pushd $task_center
svn  up
make clean
sh build.sh
echo "task_center" >> $version_log
cat versioninfo.cmake >> $version_log
cmake .
make
popd

echo "## 9. build dbagent #############################################"
pushd $dbagent
svn  up
make clean
sh build.sh
echo "dbagent" >> $version_log
cat versioninfo.cmake >> $version_log
cmake .
make
popd

echo "## 10. build dbp #############################################"
pushd $dbp
svn  up
make clean
sh build.sh
echo "dbp" >> $version_log
cat versioninfo.cmake >> $version_log
cmake .
make
popd

echo "## 11. build leveldb_server #############################################"
pushd $leveldb_server
svn  up
make clean
sh build.sh
echo "leveldb_server" >> $version_log
cat versioninfo.cmake >> $version_log
cmake .
make
popd

