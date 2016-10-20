#!/bin/bash

DB_SIZE=16
TABLE_SIZE_SMALL=16
TABLE_SIZE_BIG=256


create_table() {
    printf "create database db_"$1";\n"
    printf "USE db_"$1";\n"
    printf "CREATE TABLE t_reg_list ( \
            \n   Fuid  int(20) unsigned NOT NULL AUTO_INCREMENT, \
            \n   Fdeviceid  varchar(64) NOT NULL unique default '0', \
            \n   Fupdate_time  timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, \
            \n  PRIMARY KEY  ( Fuid ), \
            \n  UNIQUE KEY Faccount (Fuid, Fdeviceid) \
            \n)ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=500000;\n\n" $j


if (($# != 1)); then
    printf " create user usage: %s db_name\n  db_name default is user \n" $0
    exit 1
fi

create_table $1 $2 $3
