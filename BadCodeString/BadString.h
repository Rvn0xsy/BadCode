#pragma once
#include <iostream>
#include <string>
#include <Windows.h>

class BadString {

protected:
	DWORD dwStrLength = 0;
	std::string szOutStr;
	std::string Base64decode(std::string szBase64String, LPDWORD lpdwLen);
public:
	BadString(std::string szInStr);
	operator std::string();
	~BadString();

};