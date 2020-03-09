/*

*/

#include <string>
#include "libevent_cpp/include/include_all.h"
#include "unit_test.h"
#include "redis_client.h"
#include "redis_com.h"

using namespace std;
using namespace lc;
using namespace redis_com;

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

UNITTEST(test_asyn)
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

namespace
{
	class AsynFreeMgr;

	enum State
	{
		S_WAIT_NEW,
		S_WAIT_CONNECT,
		S_WAIT_SEND_SET,
		S_WAIT_REV_SET,
		S_WAIT_SEND_GET,
		S_WAIT_REV_GET,
		S_WAIT_FREE,
		S_WAIT_DISCON,
	};
	class MyCom : public RedisAsynCom
	{
	public:
		AsynFreeMgr *m_mgr = nullptr;
		virtual ~MyCom() {}

	private:
		virtual void OnCon() override;

		virtual void OnDisCon()  override;

		virtual void OnDelKey(void *privdata, bool is_success) override;
		virtual void OnSetStr(void *privdata, bool is_success) override;
		virtual void OnGetStr(void *privdata, const std::string &value) override;
	};


	class AsynFreeMgr
	{
	public:
		lc::Timer tm;
		RedisAsynCom *com=nullptr;
		State state = S_WAIT_NEW;

	public:
		AsynFreeMgr();
		void OnTimer()
		{
			if (S_WAIT_NEW == state)
			{
				UNIT_ASSERT(nullptr == com);
				MyCom *p = new MyCom();
				p->m_mgr = this;
				com = p;
				bool b = com->Init(EventMgr::Obj().GetEventBase(), REDIS_IP, REDIS_PORT);
				UNIT_ASSERT(b);
				state = S_WAIT_CONNECT;
			}
			else if (S_WAIT_SEND_SET == state)
			{
				bool b = com->SetStr(nullptr, "test_free", "11");
				UNIT_ASSERT(b);
				state = S_WAIT_REV_SET;
			}
			else if (S_WAIT_SEND_GET == state)
			{
				bool b = com->GetStr(nullptr, "test_free");
				UNIT_ASSERT(b);
				state = S_WAIT_REV_GET;
			}
			else if (S_WAIT_FREE == state)
			{
				UNIT_INFO("free");
				delete com;
				com = nullptr;
				state = S_WAIT_NEW;

			}
			else
			{
			}
		}
		void Start()
		{
			static lc::Timer tm;
			bool b = tm.StartTimer(1, std::bind(&AsynFreeMgr::OnTimer, this), true);
			UNIT_ASSERT(b);
		}
	};

	AsynFreeMgr::AsynFreeMgr()
	{
	}


	void MyCom::OnCon()
	{
		UNIT_ASSERT(m_mgr->state == S_WAIT_CONNECT);
		m_mgr->state = S_WAIT_SEND_SET;
		//UNIT_INFO("connect ok");
	}


	void MyCom::OnDisCon()
	{
		UNIT_ASSERT(m_mgr->state == S_WAIT_DISCON);
		m_mgr->state = S_WAIT_NEW;
	}

	void MyCom::OnDelKey(void *privdata, bool is_success)
	{

	}


	void MyCom::OnSetStr(void *privdata, bool is_success)
	{
		UNIT_ASSERT(m_mgr->state == S_WAIT_REV_SET);
		UNIT_ASSERT(is_success);
		m_mgr->state = S_WAIT_SEND_GET;

	}


	void MyCom::OnGetStr(void *privdata, const std::string &value)
	{
		UNIT_ASSERT(m_mgr->state == S_WAIT_REV_GET);
		UNIT_ASSERT(value == "11");
		m_mgr->state = S_WAIT_FREE;
		//UNIT_INFO("get str ok");
	}

	AsynFreeMgr mgr;

}

//测试异步通讯，释放资源
UNITTEST(test_asyn_free)
{

//	EventMgr::Obj().FreeEventBase();
//	EventMgr::Obj().Init();
	mgr.Start();

	EventMgr::Obj().Dispatch();
	UNIT_INFO("-------------------- end--------------------");

}