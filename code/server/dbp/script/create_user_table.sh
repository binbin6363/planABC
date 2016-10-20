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
						\n   Fuid  int(11) unsigned NOT NULL default 0, \
						\n   Fdeviceid  varchar(64) NOT NULL default '0', \
						\n   Faccount  varchar(256) default '0', \
						\n   Faccounttype  int(11) NOT NULL default 0, \
						\n   Fpassword  varchar(64) NOT NULL default '0', \
						\n   Fnick_name  varchar(256) default '0', \
						\n   Fstatus  int(11)  NOT NULL default 0, \
						\n   Fpre_cash  decimal(12)  NOT NULL default 0, \
						\n   Fuseable_cash  decimal(12)  NOT NULL default 0, \
						\n   Ffetched_cash  decimal(12)  NOT NULL default 0, \
						\n   Fcreate_time  timestamp NOT NULL default 0, \
						\n   Fupdate_time  timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, \
						\n  PRIMARY KEY  ( Fuid ), \
						\n  UNIQUE KEY Faccount (Fuid, Fdeviceid) \
						\n)ENGINE=InnoDB DEFAULT CHARSET=latin1;\n\n" $j
            else
                printf "CREATE TABLE t_user_%02x ( \
						\n   Fuid  int(11) unsigned NOT NULL default 0, \
						\n   Fdeviceid  varchar(64) NOT NULL default '0', \
						\n   Faccount  varchar(256) default '0', \
						\n   Faccounttype  int(11) NOT NULL default 0, \
						\n   Fpassword  varchar(64) NOT NULL default '0', \
						\n   Fnick_name  varchar(256) default '0', \
						\n   Fstatus  int(11)  NOT NULL default 0, \
						\n   Fpre_cash  decimal(12)  NOT NULL default 0, \
						\n   Fuseable_cash  decimal(12)  NOT NULL default 0, \
						\n   Ffetched_cash  decimal(12)  NOT NULL default 0, \
						\n   Fcreate_time  timestamp NOT NULL default 0, \
						\n   Fupdate_time  timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, \
						\n  PRIMARY KEY  ( Fuid ), \
						\n  UNIQUE KEY Faccount (Fuid, Fdeviceid) \
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
