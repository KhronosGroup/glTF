// Copyright (c) Analytical Graphics, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "GLTF.h"
#include "encodingHelpers.h"

using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF
{
	inline bool base64_valid(char c)
	{
		if (c >= 'A' && c <= 'Z')
			return true;
		if (c >= 'a' && c <= 'z')
			return true;
		if (c >= '0' && c <= '9')
			return true;
		if (c == '+' || c == '/' || c == '=')
			return true;
		return false;
	}

	inline char base64_encode(unsigned char uc)
	{
		if (uc < 26)
			return 'A' + uc;
		if (uc < 52)
			return 'a' + (uc - 26);
		if (uc < 62)
			return '0' + (uc - 52);
		if (uc == 62)
			return '+';
		return '/';
	}

	inline unsigned char base64_decode(char c)
	{
		if (c >= 'A' && c <= 'Z')
			return c - 'A';
		if (c >= 'a' && c <= 'z')
			return c - 'a' + 26;
		if (c >= '0' && c <= '9')
			return c - '0' + 52;
		if (c == '+')
			return 62;
		return 63;
	}


	std::string base64_encode(const std::string& _data)
	{
		std::string retval;
		if (_data.size() == 0)
			return retval;
		for (unsigned int i = 0; i<_data.size(); i += 3)
		{
			unsigned char by1 = 0, by2 = 0, by3 = 0;
			by1 = _data[i];
			if (i + 1<_data.size())
				by2 = _data[i + 1];
			if (i + 2<_data.size())
				by3 = _data[i + 2];

			unsigned char by4 = 0, by5 = 0, by6 = 0, by7 = 0;
			by4 = by1 >> 2;
			by5 = ((by1 & 0x3) << 4) | (by2 >> 4);
			by6 = ((by2 & 0xf) << 2) | (by3 >> 6);
			by7 = by3 & 0x3f;
			retval += base64_encode(by4);
			retval += base64_encode(by5);
			if (i + 1<_data.size())
				retval += base64_encode(by6);
			else
				retval += "=";

			if (i + 2<_data.size())
				retval += base64_encode(by7);
			else
				retval += "=";
		}
		return retval;
	}

	std::string base64_decode(const std::string& _str)
	{
		std::string str;
		for (unsigned int j = 0; j<_str.length(); j++)
		{
			if (base64_valid(_str[j]))
				str += _str[j];
		}
		std::string retval;
		if (str.length() == 0)
			return retval;
		for (unsigned int i = 0; i<str.length(); i += 4)
		{
			char c1 = 'A', c2 = 'A', c3 = 'A', c4 = 'A';
			c1 = str[i];
			if (i + 1<str.length())
				c2 = str[i + 1];
			if (i + 2<str.length())
				c3 = str[i + 2];
			if (i + 3<str.length())
				c4 = str[i + 3];
			unsigned char by1 = 0, by2 = 0, by3 = 0, by4 = 0;
			by1 = base64_decode(c1);
			by2 = base64_decode(c2);
			by3 = base64_decode(c3);
			by4 = base64_decode(c4);
			retval.push_back((by1 << 2) | (by2 >> 4));
			if (c3 != '=')
				retval.push_back(((by2 & 0xf) << 4) | (by3 >> 2));
			if (c4 != '=')
				retval.push_back(((by3 & 0x3) << 6) | by4);
		}
		return retval;
	}

	std::string create_dataUri(const std::string& content, const std::string& contentType, bool base64Encode)
	{
		std::string result = "data:" + contentType;
		if (base64Encode)
			result += ";base64," + base64_encode(content);
		else
			result += "," + content;

		return result;
	}
}
