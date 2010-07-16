#pragma once

#include <string>
#include <vector>

namespace weasel
{

	enum TextAttributeType
	{
		NONE = 0,
		HIGHLIGHTED,
		LAST_TYPE
	};

	struct TextRange
	{
		TextRange() : start(0), end(0) {}
		TextRange(int _start, int _end) : start(_start), end(_end) {}
		int start;
		int end;
	};

	struct TextAttribute
	{
		TextAttribute() {}
		TextAttribute(int _start, int _end, TextAttributeType _type) : range(_start, _end), type(_type) {}
		TextRange range;
		TextAttributeType type;
	};

	struct Text
	{
		Text() : str(L"") {}
		Text(std::wstring const& _str) : str(_str) {}
		std::wstring str;
		std::vector<TextAttribute> attributes;
	};

	struct CandidateInfo
	{
		CandidateInfo()
		{
			currentPage = 0;
			totalPages = 0;
			highlighted = 0;
		}
		int currentPage;
		int totalPages;
		int highlighted;
		std::vector<Text> candies;
	};

	struct ZIMEInfo
	{
		ZIMEInfo() {}
		Text preedit;
		Text aux;
		CandidateInfo cinfo;
	};

	struct ZIMEStatus
	{
		ZIMEStatus() { zhMode = false; }
		// 转换开关，false：西文，true：中文
		bool zhMode;
	};
}