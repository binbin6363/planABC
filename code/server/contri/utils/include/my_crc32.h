#ifndef _UTILS_CRC32_H_
#define _UTILS_CRC32_H_

#ifdef __cplusplus
extern "C" {
#endif 


namespace utils
{

void make_table();
unsigned int crc32(char *buff, unsigned int len, unsigned int crc = 0);

}



#ifdef __cplusplus
}
#endif 


#endif //_UTILS_CRC32_H_

