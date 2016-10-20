
echo 'create dbs ...'
sh create_db.sh user 16 |mysql -h127.0.0.1 -u root -p'e10adc3949ba59abbe56e057f20f883e'
sh create_db.sh third 16 |mysql -h127.0.0.1 -u root -p'e10adc3949ba59abbe56e057f20f883e'
sh create_db.sh task 16|mysql -h127.0.0.1 -u root -p'e10adc3949ba59abbe56e057f20f883e'
printf "create database db_global_id_0;\n"|mysql -h127.0.0.1 -u root -p'e10adc3949ba59abbe56e057f20f883e'

echo 'create tables ...'
sh create_user_table.sh 16 256 user |mysql -h127.0.0.1 -u root -p'e10adc3949ba59abbe56e057f20f883e'
sh create_third_bind_table.sh 16 16 third |mysql -h127.0.0.1 -u root -p'e10adc3949ba59abbe56e057f20f883e'
sh create_task_cash_table.sh 16 16 task |mysql -h127.0.0.1 -u root -p'e10adc3949ba59abbe56e057f20f883e'
sh create_global_id_table.sh 1 16 global_id_0|mysql  -h127.0.0.1 -u root -p'e10adc3949ba59abbe56e057f20f883e'
