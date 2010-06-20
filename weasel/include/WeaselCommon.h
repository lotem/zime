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
		int start;
		int end;
	};

	struct TextAttribute
	{
		TextRange range;
		TextAttributeType type;
	};

	struct Text
	{
		Text(std::wstring const& _str) : str(_str) {}
		std::wstring str;
		std::vector<TextAttribute> attributes;
	};

	struct CandidateInfo
	{
		int currentPage;
		int totalPages;
		int highlighted;
		std::vector<Text> candies;
	};

	struct ZIMEInfo
	{
		Text preedit;
		Text aux;
		CandidateInfo cinfo;
	};

	struct ZIMEStatus
	{
		// 转换开关，false：西文，true：中文
		bool zhMode;
	};

}