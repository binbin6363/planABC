
if [ $2 == 16 ] ; 
then 
	for((i=0;i<$2;i++))
	{
        	printf "create database db_"$1"_%x;\n" $i	
	}
fi
if [ $2 == 256 ];
then
	for((i=0;i<16;i++))
	{
		for((j=0;j<16;j++))
		{
			printf "create database db_"$1"_%x%x;\n" $i $j	
		}
	}
fi

if (($# != 2)); then
    printf " create user usage: %s db_name db_size  \n" $0
    exit 1
fi
