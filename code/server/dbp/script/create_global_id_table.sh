#!/bin/bash

DB_SIZE=1
TABLE_SIZE_SMALL=16
TABLE_SIZE_BIG=256


create_table() {
        printf "USE db_%s_0; \n" $3

        for ((j=0;j<$2;++j)); do
            if (($2 == TABLE_SIZE_SMALL)); then

                printf "CREATE TABLE t_id_%x ( \
						\n   Fid  int(11) NOT NULL default 0, \
						\n   Fupdate_time  timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, \
						\n  PRIMARY KEY  ( Fid ) \
						\n)ENGINE=InnoDB DEFAULT CHARSET=latin1;\n\n" $j
            else
                printf "CREATE TABLE t_id_%02x ( \
						\n   Fid  int(11) NOT NULL default 0, \
						\n   Fupdate_time  timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, \
						\n  PRIMARY KEY  ( Fid ) \
						\n)ENGINE=InnoDB DEFAULT CHARSET=latin1;\n\n" $j
            fi
        done
}

if (($# != 3)); then
    printf " create user usage: %s db_size table_size db_name \n" $0
    exit 1
fi

create_table $1 $2 $3
