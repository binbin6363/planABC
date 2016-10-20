#ifndef _CODE_CONVERTER_H_
#define _CODE_CONVERTER_H_

#include "iconv.h"
#include <string>

using namespace std;

namespace utils {

class CCodeConverter
{
public:
    CCodeConverter();
    ~CCodeConverter();

public:
	int BytesConvert(const char* pszFromCharset, const char* pszToCharset,
					 char* pszInBuffer, size_t* pnInbytesLeftLen,
					 char* pszOutBuffer, size_t* pnOutBytesLeftLen);

	int Convert(const string& sFromCharset,	const string& sToCharset,
				const string& sInString, string& sOutString);

private:
    iconv_t m_icnvtDescriptor;
};

} // namespace utils
#endif
