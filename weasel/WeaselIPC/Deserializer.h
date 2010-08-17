#pragma once
#include <ResponseParser.h>
#include <vector>

namespace weasel
{

	class Deserializer
	{
	public:
		typedef std::vector<std::wstring>::const_iterator KeyType;
		typedef boost::shared_ptr<Deserializer> Ptr;
		typedef boost::function<Ptr (ResponseParser* pTarget)> Factory;

		Deserializer(ResponseParser* pTarget);
		virtual ~Deserializer();
		virtual void Store(KeyType k, std::wstring const& value);

		static void Define(std::wstring const& action, Factory factory);
		static bool Require(std::wstring const& action, ResponseParser* pTarget);

	protected:
		ResponseParser* m_pTarget;

	private:
		static std::map<std::wstring, Factory> s_factories;
	};

}
