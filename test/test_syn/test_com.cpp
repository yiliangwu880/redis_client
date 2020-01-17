/*

*/

#include <string>
#include "libevent_cpp/include/include_all.h"
#include "unit_test.h"
#include "redis_client.h"
#include "redis_com.h"

using namespace std;
using namespace redis_com;
static const char *REDIS_IP = "192.168.1.204";
static const short REDIS_PORT = 8309;

namespace
{
	
	RedisCom g_con;

}

UNITTEST(test_syn_com)
{
	UNIT_ASSERT(g_con.Init(REDIS_IP, REDIS_PORT));

	
	{
		RedisCon::UNIQUE_PTR r = g_con.m_client.Cmd("select 5");
		UNIT_ASSERT(r.get() != nullptr);
		UNIT_ASSERT(string("OK") == r->str);
	}

	bool r = false;
	{
		 r = g_con.SetStr("k1", "v1");
		 UNIT_ASSERT(r);

		 string s = "a";
		 s.append(1,0);
		 s.append("b");
		 r = g_con.SetStr(s, s);
		 UNIT_ASSERT(r);
		 string ret_s = g_con.GetStr(s);
		 UNIT_ASSERT(ret_s==s);

		 UNIT_ASSERT(false == g_con.DelKey("a"));
		 UNIT_ASSERT(g_con.DelKey("k1"));
		 UNIT_ASSERT(g_con.DelKey(s));
	}
	//{
	//	RedisCon::UNIQUE_PTR r = g_con.Cmd("get %s", "foo");
	//	UNIT_ASSERT(string("hello world") == r->str);
	//}
	//{
	//	RedisCon::UNIQUE_PTR r = g_con.Cmd("HMSET hset name1 redis name2 abc");
	//	UNIT_ASSERT(string("OK") == r->str);
	//}
	//{
	//	RedisCon::UNIQUE_PTR reply = g_con.Cmd("HGETALL hset");
	//	if (reply->type == REDIS_REPLY_ARRAY) {
	//		for (size_t j = 0; j < reply->elements; j++) {
	//			UNIT_INFO("%s", reply->element[j]->str);
	//		}
	//	}
	//	UNIT_ASSERT(string("redis") == reply->element[1]->str);
	//}
	//{
	//	RedisCon::UNIQUE_PTR r = g_con.Cmd("del hset");
	//	UNIT_ASSERT(1 == r->integer);
	//}
	//{
	//	RedisCon::UNIQUE_PTR r = g_con.Cmd("del foo");
	//	UNIT_ASSERT(1 == r->integer);
	//}


	UNIT_INFO("-------------------- end--------------------");

}