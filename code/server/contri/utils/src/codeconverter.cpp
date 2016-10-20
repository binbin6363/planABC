#include "codeconverter.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>

namespace utils {

CCodeConverter::CCodeConverter()
{
    m_icnvtDescriptor = (iconv_t)(-1);
}

CCodeConverter::~CCodeConverter()
{
    iconv_close(m_icnvtDescriptor);
}

int CCodeConverter::BytesConvert(const char* pszFromCharset,
								 const char* pszToCharset,
								 char* pszInBuffer,
								 size_t* pnInbytesLeftLen,
								 char* pszOutBuffer,
								 size_t* pnOutBytesLeftLen)
{
    m_icnvtDescriptor = iconv_open(pszToCharset, pszFromCharset);
    if((iconv_t)-1 == m_icnvtDescriptor)
    {
        fprintf(stderr, "iconv_open open failed. errno:%d, pszFromCharset:%s, pszToCharset:%s.\n", errno, pszFromCharset, pszToCharset);
        
        return -1;
    }
    memset(pszOutBuffer, 0, *pnOutBytesLeftLen);
    char** pIn = &pszInBuffer;
    char** pOut = &pszOutBuffer;
    return iconv(m_icnvtDescriptor, pIn, (size_t*)pnInbytesLeftLen,
                 pOut, (size_t*)pnOutBytesLeftLen);
}

int CCodeConverter::Convert(const string& sFromCharset,
			    const string& sToCharset,
			    const string& sInString, string& sOutString)
{
    size_t unInStringLen = sInString.length();
    size_t unOutBufferLen = unInStringLen * 4;
    size_t unOutBufferLeftLen = unOutBufferLen;
    //char szOutBuffer[unOutBufferLen];
    char *szOutBuffer = new char[unOutBufferLen];
    memset(szOutBuffer, 0, unOutBufferLen);
    int nRet = BytesConvert(sFromCharset.c_str(), sToCharset.c_str(),
                            const_cast<char*>(sInString.c_str()),
                            &unInStringLen, szOutBuffer, &unOutBufferLeftLen);
    if (0 == nRet)
    {
        sOutString = string(szOutBuffer, (unOutBufferLen-unOutBufferLeftLen));
    }
    delete [] szOutBuffer;
    return nRet;
}
} // namespace utils
