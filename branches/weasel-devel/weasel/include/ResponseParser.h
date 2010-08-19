#pragma once
#include <WeaselCommon.h>
#include <windows.h>
#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/smart_ptr.hpp>


namespace weasel
{
	class Deserializer;

	// ����server�ؑ��ı�
	struct ResponseParser
	{
		std::map<std::wstring, boost::shared_ptr<Deserializer> > deserializers;

		std::wstring& r_commit;
		Context& r_context;
		Status& r_status;

		// ��������������ʼ��, ����ֱ�Ӹ���Ŀ�˔�������, �����o�^�Č���ؐ!
		ResponseParser(std::wstring& commit, Context& context, Status& status);

		// ���d�����{���\���, �԰���ResponseHandler
		bool operator() (LPWSTR buffer, UINT length);

		// ̎��һ�лؑ��ı�
		void Feed(const std::wstring& line);
	};

}
