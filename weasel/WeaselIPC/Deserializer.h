#pragma once
#include <WeaselIPC.h>

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
		virtual void Store(Deserializer::KeyType k, std::wstring const& value);

		static void Define(std::wstring const& action, Factory factory);
		static bool Require(std::wstring const& action, ResponseParser* pTarget);
		static void Feed(std::wstring const& line);

	protected:
		ResponseParser* m_pTarget;

	private:
		static void _Dispatch(Deserializer::KeyType k, std::wstring const& value);

		static std::map<std::wstring, Factory> s_factories;
		static std::map<std::wstring, Ptr> s_instances;

	};

}
