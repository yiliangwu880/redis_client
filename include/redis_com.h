/*
组合使用接口，推荐用户直接用这个接口
这里的string支持任意二进制数据。
*/


#pragma once

#include "redis_client.h"
#include "redis_parse_cmd.h"
#include <list>

namespace redis_com
{
	//阻塞同步方式
	class RedisCom
	{
	public:
		RedisCon m_client;//获取RedisCon对象，用户就可以操作原生redis client命令.

	public:
		bool Init(const std::string &ip, uint16_t port, uint32_t wait_sec = 5);


		bool DelKey(const std::string &key);

		//字符串(String)
		//---------------------------------------------------------------------
		bool SetStr(const std::string &key, const std::string &value);
		//失败返回空字符串
		std::string GetStr(const std::string &key);

		//哈希(Hash)
		//---------------------------------------------------------------------
		bool SetHash(const std::string &key, const std::string &field, const std::string &value);
		bool SetHash(const std::string &key, const VecFieldValue &vec_field_value);
		bool GetHash(const std::string &key, const std::string &field, std::string &value);
		bool GetAllHash(const std::string &key, VecFieldValue &vec_field_value);

		//有序集合(sorted set)
		//---------------------------------------------------------------------
		bool SetZSet(const std::string &key, const std::string &member, int64_t score);
		bool SetZSet(const std::string &key, const VecMemberScore &vec_m_s);
		//获取区间[start_idx, stop_idx]成员 
		//@start_idx, stop_idx  指定索引
		bool GetZSetRange(const std::string &key, uint32_t start_idx, uint32_t stop_idx, VecMemberScore &vec_m_s);
		bool GetZSetAll(const std::string &key, VecMemberScore &vec_m_s);
		uint32_t GetZSetIdx(const std::string &key, const std::string &member); //获取排名索引。 0开始
		int64_t GetZSetScore(const std::string &key, const std::string &member);

		//列表(List)
		//---------------------------------------------------------------------
		bool PushList(const std::string &key, const std::string &value);
		//失败返回空字符串
		std::string PopList(const std::string &key);

		//集合(Set)
		//---------------------------------------------------------------------
		bool AddSet(const std::string &key, const std::vector<std::string> &vec_value);
		bool GetSet(const std::string &key, std::vector<std::string> &vec_value);
	};

	class RedisAsynCom;
	//适配RedisAsynCon用，避免RedisAsynCom多层继承带来的复杂问题。
	class RedisAsynConAdapter : public RedisAsynCon
	{
		RedisAsynCom &m_com;
	public:
		RedisAsynConAdapter(RedisAsynCom &com);

	private:
		virtual void OnCon();
		virtual void OnDisCon();
		virtual void OnRev(const redisReply *reply, void *privdata); //reply 回调函数结束后，自动释放

	};

	//异步访问方法，用来区分 RedisAsynCom 不同函数回调
	//RAM= redis asyn method
	enum class RAM
	{
		None,
		DelKey,
		SetStr,
		GetStr,
	};

	//记录每个请求的信息。
	struct ReqInfo
	{
		uint64_t id = 0; //ReqInfo  唯一标识
		RAM method = RAM::None;
		void *privdata = nullptr;//用户的自定义参数
	};

	//libevent异步方式
	class RedisAsynCom
	{
		friend class RedisAsynConAdapter;
	private:
		RedisAsynConAdapter m_client;
		std::list<ReqInfo> m_req_info_list; //记录每个请求的信息, 等接受响应的时候获取信息。
		uint64_t m_id_seed = 0;

	public:
		~RedisAsynCom();
		bool Init(event_base *base, const std::string &ip, uint16_t port);
		void Dicon();

		//测试用，缓存 ReqInfo 过大表示可能泄漏内存了。
		size_t GetReqInfoSize() const;

		//@privdata 用户自定义数据，不推荐传指针，生存期难控制。
		bool Command(void *privdata, const char *format, ...);
		bool CommandArgv(void *privdata, int argc, const char **argv, const size_t *argvlen);

		bool DelKey(void *privdata, const std::string &key);

		//字符串(String)
		//---------------------------------------------------------------------
		bool SetStr(void *privdata, const std::string &key, const std::string &value);
		bool GetStr(void *privdata, const std::string &key);

	private:
		virtual void OnCon() = 0;
		virtual void OnDisCon() = 0;
		virtual void OnRev(const redisReply *reply, void *privdata) = 0; //reply 回调函数结束后，自动释放
		virtual void OnDelKey(void *privdata, bool is_success) = 0;

		uint32_t PushReqInfo(void *privdata, RAM method);//返回新加req info id
		bool PopReqInfo(uint64_t id, ReqInfo &req_info);

	};
}