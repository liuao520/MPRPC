#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <semaphore.h>
#include <iostream>

// 全局的watcher观察器   zkserver给zkclient的通知（之前zkclient给zkserver发过消息，收到处理以后）
void global_watcher(zhandle_t *zh, int type,
                   int state, const char *path, void *watcherCtx)
{ 
    if (type == ZOO_SESSION_EVENT)  // 回调的消息类型是和会话相关的消息类型 连接或者断开连接（状态）
	{
		if (state == ZOO_CONNECTED_STATE)  // zkclient和zkserver连接成功
		{
			sem_t *sem = (sem_t*)zoo_get_context(zh);//从指定的句柄上获取信号量（地址）
            sem_post(sem);//信号量加一
		}
	}
}

ZkClient::ZkClient() : m_zhandle(nullptr)
{
}

ZkClient::~ZkClient()
{
    if (m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle); // 关闭句柄，释放资源  MySQL_Conn
    }
}

// 连接zkserver
/*
整体异步连接过程：：：：：；
首先从配置文件中加载指定ip和port 组成指定格式
其次zookeeper_mt：多线程版本 
一个调用者线程zookeeper_init，这个起来了之后，调用两次pthread_create
一个专门负责网络I/O 收发 一个专门给客户端通知消息（zkserver给zkclient）回调
当zookeeper_init传入了连接字符串，回调等，句柄初始化成功，同时网络io也把消息发出去了
其次 给句柄绑定一个信号量，等待信号量（初始化信号量为0），
当zkserver连接成功，zkserver会给zkclient发一个通知（全局回调函数watcher），
当连接成功，给相关句柄绑定的semphre的加锁（sem_post）
信号量资源加一之后，上述client获取信号量，即连接成功
*/
void ZkClient::Start()
{
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr = host + ":" + port;
    
	/*
	zookeeper_mt：多线程版本
	zookeeper的API客户端程序提供了三个独立线程
	API调用线程 当前zkeeper__init调用线程
	网络I/O线程  pthread_create  poll  专门起一个线程做IO
	watcher回调线程 pthread_create
	*/
//这个不是说明连接了，只能说明创建本地资源的句柄 内存开辟初始化成功了，网络连接还没收到响应
//响应是通过注册的回调的响应
//是一个异步的过程，知道相关回调线程得到了响应才会起一个会话，建立连接
    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (nullptr == m_zhandle) 
    {
        std::cout << "zookeeper_init error!" << std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
	//给m_zhandle这个句柄传参数添加额外信息
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    std::cout << "zookeeper_init success!" << std::endl;
}

void ZkClient::Create(const char *path, const char *data, int datalen, int state)
{
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    int flag;
	// 先判断path表示的znode节点是否存在，如果存在，就不再重复创建了
	//同步的判断 指定的path节点是否再zookeeper上存在
	flag = zoo_exists(m_zhandle, path, 0, nullptr);
	if (ZNONODE == flag) // 表示path的znode节点不存在
	{
		// 创建指定path的znode节点了
		flag = zoo_create(m_zhandle, path, data, datalen,
			&ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
		if (flag == ZOK)
		{
			std::cout << "znode create success... path:" << path << std::endl;
		}
		else
		{
			std::cout << "flag:" << flag << std::endl;
			std::cout << "znode create error... path:" << path << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

// 根据指定的path，获取znode节点的值
//同步的凡是来获取znode节点的值
std::string ZkClient::GetData(const char *path)
{
    char buffer[64];
	int bufferlen = sizeof(buffer);
	int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
	if (flag != ZOK)
	{
		std::cout << "get znode error... path:" << path << std::endl;
		return "";
	}
	else
	{
		return buffer;
	}
}