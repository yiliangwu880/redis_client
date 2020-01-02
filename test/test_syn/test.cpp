/*

*/

#include <string>
#include "libevent_cpp/include/include_all.h"
#include "unit_test.h"
#include "redis_client.h"

using namespace std;
using namespace lc;


namespace
{
	class MyAsy : public RedisAsynCon
	{

	private:

		virtual void OnCon() ;
		virtual void OnDisCon() ;
		virtual void OnRev(const redisReply *reply, void *privdata) ; //reply 回调函数结束后，自动释放

	};

	void MyAsy::OnCon()
	{

	}


	void MyAsy::OnDisCon()
	{

	}


	void MyAsy::OnRev(const redisReply *reply, void *privdata)
	{

	}

	MyAsy asynCon;
}

UNITTEST(test_syn)
{
	EventMgr::Obj().Init();

	lc::Timer tm;
	auto f = []() {};
	tm.StartTimer(1000, f, true);

	bool r = asynCon.Init(EventMgr::Obj().GetEventBase(), REDIS_IP, REDIS_PORT);
	UNIT_ASSERT(r);

	EventMgr::Obj().Dispatch();
	UNIT_INFO("-------------------- end--------------------");

}