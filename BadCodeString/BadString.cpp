#include "BadString.h"

std::string BadString::Base64decode(std::string szBase64String, LPDWORD lpdwLen)
{
	DWORD dwLen;
	DWORD dwNeed;
	PBYTE lpBuffer = NULL;
	dwLen = szBase64String.length();
	dwNeed = 0;
	CryptStringToBinaryA(szBase64String.c_str(), 0, CRYPT_STRING_BASE64, NULL, &dwNeed, NULL, NULL);
	if (dwNeed)
	{
		lpBuffer = new BYTE[dwNeed + 1];
		ZeroMemory(lpBuffer, dwNeed + 1);
		CryptStringToBinaryA(szBase64String.c_str(), 0, CRYPT_STRING_BASE64, lpBuffer, &dwNeed, NULL, NULL);
		*lpdwLen = dwNeed;
	}
	return std::string((PCHAR)lpBuffer);
}

BadString::BadString(std::string szInStr)
{
	this->dwStrLength = szInStr.length();
	this->szOutStr = this->Base64decode(szInStr, &this->dwStrLength);
}

BadString::operator std::string()
{
	return this->szOutStr;
}

BadString::~BadString()
{
}
