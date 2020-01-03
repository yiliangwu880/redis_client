

#include "libevent_cpp/include/include_all.h"
#include "redis_client.h"
#include "unit_test.h"

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

		virtual void OnCon();
		virtual void OnDisCon();
		virtual void OnRev(const redisReply *reply, void *privdata); //reply 回调函数结束后，自动释放

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
				UNIT_ASSERT(string("PONG") == reply->str);
				Command(nullptr, "select 5");
				UNIT_INFO("req select 5");
				break;
			case 2:
				UNIT_INFO("rev: %s", reply->str);
				UNIT_ASSERT(string("OK") == reply->str);
				Command(nullptr, "SET runoobkey redis");
				break;
			case 3:
				UNIT_ASSERT(string("OK") == reply->str);
				Command(nullptr, "GET runoobkey");
				break;
			case 4:
				UNIT_ASSERT(string("redis") == reply->str);
				Command(nullptr, "HMSET hset name1 redis name2 abc");
				break;
			case 5:
				UNIT_ASSERT(string("OK") == reply->str);
				Command(nullptr, "HGETALL hset");
				break;
			case 6:
				if (reply->type == REDIS_REPLY_ARRAY) {
					for (uint32 j = 0; j < reply->elements; j++) {
						UNIT_INFO("%s", reply->element[j]->str);
					}
				}
				UNIT_ASSERT(string("redis") == reply->element[1]->str);
				Command(nullptr, "del hset");

				Command(nullptr, "del runoobkey");
				break;
			case 7:
				UNIT_ASSERT(1 == reply->integer);
				break;
			case 8:
				UNIT_ASSERT(1 == reply->integer);
				break;
			}
		m_step++;

	}

	MyAsy g_asynCon;
}

void test()
{
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

int main(int argc, char* argv[])
{
	test();
	return 0;
}
