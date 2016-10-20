#!/bin/bash

DB_SIZE=16
TABLE_SIZE_SMALL=16
TABLE_SIZE_BIG=256


create_table() {
    for ((i=0; i<$1; ++i)); do
        printf "USE db_%s_%x; \n" $3 ${i}

        for ((j=0;j<$2;++j)); do
            if (($2 == TABLE_SIZE_SMALL)); then

                printf "CREATE TABLE t_user_%x ( \
						\n   Fuid  int(11) NOT NULL default 0, \
						\n   Fthird_id  varchar(256) NOT NULL default '0', \
						\n   Fthird_type  int(11) NOT NULL default 0, \
						\n   Fnick_name  varchar(256) default '0', \
						\n   Fgender  tinyint(3) default 0, \
						\n   Favatar  varchar(256) default '0', \
						\n   Finfo  varchar(256) default '0', \
						\n   Flocation  varchar(64) default '0', \
						\n   Fcity  varchar(64) default '0', \
						\n   Fstatus  int(11) NOT NULL default 0, \
						\n   Fcreate_time  timestamp NOT NULL default 0, \
						\n   Fupdate_time  timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, \
						\n  PRIMARY KEY  ( Fuid ), \
						\n  UNIQUE KEY Fuid (Fuid, Fthird_id, Fthird_type) \
						\n)ENGINE=InnoDB DEFAULT CHARSET=latin1;\n\n" $j
            else
                printf "CREATE TABLE t_user_%02x ( \
						\n   Fuid  int(11) NOT NULL default 0, \
						\n   Fthird_id  varchar(256) NOT NULL default '0', \
						\n   Fthird_type  int(11) NOT NULL default 0, \
						\n   Fnick_name  varchar(256) default '0', \
						\n   Fgender  tinyint(3) default 0, \
						\n   Favatar  varchar(256) default '0', \
						\n   Finfo  varchar(256) default '0', \
						\n   Flocation  varchar(64) default '0', \
						\n   Fcity  varchar(64) default '0', \
						\n   Fstatus  int(11) NOT NULL default 0, \
						\n   Fcreate_time  timestamp NOT NULL default 0, \
						\n   Fupdate_time  timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, \
						\n  PRIMARY KEY  ( Fuid ), \
						\n  UNIQUE KEY Fuid (Fuid, Fthird_id, Fthird_type) \
						\n)ENGINE=InnoDB DEFAULT CHARSET=latin1;\n\n" $j
            fi
        done
    done
}

if (($# != 3)); then
    printf " create user usage: %s db_size table_size db_name \n" $0
    exit 1
fi

create_table $1 $2 $3
