
if (($# != 3)); then
    printf " delete table usage: %s db_name table_name table_size \n" $0
    exit 1
fi


if [ $3 == 16 ] ; 
then
        for((i=0;i<16;i++))
        {
                printf "use db_$1_%x;\n" $i
                for((j=0; j < $3 ;j++))
                {
                        printf "drop table t_"$2"_%x;\n" $j
                }
        }
fi        
if [ $3 == 256 ];
then    
        for((i=0;i<16;i++))
        {    
                printf "use db_$1_%x;\n" $i
                for((j=0; j < $3 ;j++))
                {
                        printf "drop table t_"$2"_%02x;\n" $j 
                }
        }
fi