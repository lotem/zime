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
		void clear()
		{
			str.clear();
			attributes.clear();
		}
		bool empty() const
		{
			return str.empty();
		}
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
		void clear()
		{
			currentPage = 0;
			totalPages = 0;
			highlighted = 0;
			candies.clear();
		}
		bool empty() const
		{
			return candies.empty();
		}
		int currentPage;
		int totalPages;
		int highlighted;
		std::vector<Text> candies;
	};

	struct Context
	{
		Context() {}
		void clear()
		{
			preedit.clear();
			aux.clear();
			cinfo.clear();
		}
		bool empty() const
		{
			return preedit.empty() && aux.empty() && cinfo.empty();
		}
		Text preedit;
		Text aux;
		CandidateInfo cinfo;
	};

	// ��ime����
	struct Status
	{
		Status() : zh_mode(false), composing(false) {}
		void reset()
		{
			zh_mode = true;
			composing = false;
		}
		// �����D�Q�_�P
		bool zh_mode;
		// ������B
		bool composing;
	};
}