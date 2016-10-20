#!/bin/bash

DB_SIZE=16
TABLE_SIZE_SMALL=16
TABLE_SIZE_BIG=256


create_table() {
    for ((i=0; i<$1; ++i)); do
        printf "USE db_%s_%x; \n" $3 ${i}

        for ((j=0;j<$2;++j)); do
            if (($2 == TABLE_SIZE_SMALL)); then

                printf "CREATE TABLE t_task_%x ( \
						\n   Ftaskid  bigint NOT NULL default 0, \
						\n   Ftask_status  Int(11) NOT NULL default 0, \
						\n   Ftask_algorithm  Varchar(512) NOT NULL default '0', \
						\n   Fpre_cash  decimal(12,2) NOT NULL default 0.00, \
						\n   Faward_cash  decimal(12,2) NOT NULL default 0.00, \
						\n   Ffetched_cash  decimal(12,2) NOT NULL default 0.00, \
						\n   Flimit_num  Int(11) NOT NULL default 0, \
						\n   Fstart_num  Int(11) NOT NULL default 0, \
						\n   Fstart_time  timestamp NOT NULL default 0, \
						\n   Fend_time  timestamp NOT NULL default 0, \
						\n   Fcreate_time  timestamp NOT NULL default 0, \
						\n   Fupdate_time  timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, \
						\n  PRIMARY KEY  ( Ftaskid )\
						\n)ENGINE=InnoDB DEFAULT CHARSET=latin1;\n\n" $j
            else
                printf "CREATE TABLE t_task_%02x ( \
						\n   Ftaskid  bigint NOT NULL default 0, \
						\n   Ftask_status  Int(11) NOT NULL default 0, \
						\n   Ftask_algorithm  Varchar(512) NOT NULL default '0', \
						\n   Fpre_cash  decimal(12,2) NOT NULL default 0.00, \
						\n   Faward_cash  decimal(12,2) NOT NULL default 0.00, \
						\n   Ffetched_cash  decimal(12,2) NOT NULL default 0.00, \
						\n   Flimit_num  Int(11) NOT NULL default 0, \
						\n   Fstart_num  Int(11) NOT NULL default 0, \
						\n   Fstart_time  timestamp NOT NULL default 0, \
						\n   Fend_time  timestamp NOT NULL default 0, \
						\n   Fcreate_time  timestamp NOT NULL default 0, \
						\n   Fupdate_time  timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, \
						\n  PRIMARY KEY  ( Ftaskid )\
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
