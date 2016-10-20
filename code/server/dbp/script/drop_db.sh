
if [ $2 == 16 ] ; 
then 
	for((i=0;i<$2;i++))
	{
        	printf "drop database db_"$1"_%x;\n" $i	
	}
fi
if [ $2 == 256 ];
then
	for((i=0;i<16;i++))
	{
		for((j=0;j<16;j++))
		{
			printf "drop database db_"$1"_%x%x;\n" $i $j	
		}
	}
fi
