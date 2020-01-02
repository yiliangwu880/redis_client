/*

*/

#include <string>
#include "libevent_cpp/include/include_all.h"
#include "unit_test.h"
#include "redis_client.h"

using namespace std;
using namespace lc;

static const char *REDIS_IP = "192.168.1.204";
static const short REDIS_PORT = 8309;

namespace
{
	class MyAsy : public RedisAsynCon
	{
		int m_step = 1;
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
		UNIT_ASSERT(reply);
		UNIT_INFO("step : %d", m_step)
		switch (m_step)
		{
		default:
			UNIT_ASSERT(false);
			break;
		case 1:
			UNIT_INFO("rev: %s", reply->str);
			UNIT_ASSERT(string("PONG")== reply->str);
			Command(nullptr, "select 5");
			UNIT_INFO("req select 5");
			break;
		case 2:
			UNIT_INFO("rev: %s", reply->str);
			UNIT_ASSERT(string("OK") == reply->str);
			break;
		}
		m_step++;

	}

	MyAsy g_asynCon;
	
	//void  t()
	//{
	//		redisContext *c;
	//		const char *hostname =  "127.0.0.1";
	//		int port =  6379;

	//		struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	//		c = redisConnectWithTimeout(hostname, port, timeout);
	//		if (c == NULL || c->err) {
	//			if (c) {
	//				printf("Connection error: %s\n", c->errstr);
	//				redisFree(c);
	//			}
	//			else {
	//				printf("Connection error: can't allocate redis context\n");
	//			}
	//		}
	//}
}

UNITTEST(test_syn)
{
	return;
	EventMgr::Obj().Init();

	lc::Timer tm;
	auto f = []() {
		g_asynCon.Dicon();
		EventMgr::Obj().StopDispatch();
	};
	tm.StartTimer(1000, f);

	bool r = g_asynCon.Init(EventMgr::Obj().GetEventBase(), REDIS_IP, REDIS_PORT);
	UNIT_ASSERT(r);


	g_asynCon.Command(nullptr, "PING");
	EventMgr::Obj().Dispatch();
	UNIT_INFO("-------------------- end--------------------");

}