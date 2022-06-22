#include "rpcprovider.h"
#include "mprpcapplication.h"

void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    //

    const ::google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    const std::string service_name = pserviceDesc->name();
    int methodCnt = pserviceDesc->method_count();

    for(int i = 0; i < methodCnt; ++i){
        const google::protobuf::MethodDescriptor *pmethodDesc = pserviceDesc->method(i);
        const std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}
// 启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run()
{
    //
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("repcserverip");
    u_int16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);
    // sever对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");

    //
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, 
            std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4);

    // rpc服务端准备启动，打印信息
    std::cout << "RpcProvider start service at ip:" << ip << " port:" << port << std::endl;

    //start
    server.start();
    m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                            muduo::net::Buffer *buffer,
                            muduo::Timestamp)
{
}