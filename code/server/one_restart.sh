
run_dir=/usr/local/bbwang
mobiled=$run_dir/mobiled
taskcenter=$run_dir/taskcenter
usercenter=$run_dir/usercenter
dbp=$run_dir/dbp
dbagent=$run_dir/dbagent
leveldbserver=$run_dir/leveldbserver


#redis-server /usr/local/etc/redis.conf 


#/bin/sh /usr/bin/mysqld_safe --datadir=/var/lib/mysql --socket=/var/lib/mysql/mysql.sock --pid-file=/var/run/mysqld/mysqld.pid --basedir=/usr --user=mysql
#/usr/libexec/mysqld --basedir=/usr --datadir=/var/lib/mysql --user=mysql --log-error=/var/log/mysqld.log --pid-file=/var/run/mysqld/mysqld.pid --socket=/var/lib/mysql/mysql.sock

pushd mobiled
echo "restart mobiled"
sh kill.sh
sh start.sh
popd



pushd taskcenter
echo "restart taskcenter"
sh kill.sh
sh start.sh
popd


pushd usercenter
echo "restart usercenter"
sh kill.sh
sh start.sh
popd


pushd dbp
echo "restart dbp"
sh kill.sh
sh start.sh
popd


pushd dbagent
echo "restart dbagent"
sh kill.sh
sh start.sh
popd


pushd leveldbserver
echo "restart leveldbserver"
sh kill.sh
sh start.sh
popd

