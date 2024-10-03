#include "MprpcProvider.h"
#include "RpcHeader.pb.h"


void RpcProvider::NotifyService(google::protobuf::Service* service)
{
    ServiceInfo service_info;

    // 获取服务对象的描述
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();

    std::string service_name = pserviceDesc->name();

    int methodCnt = pserviceDesc->method_count();

    // std::cout << "service_name: " << service_name << " is registered" << std::endl;
    LOG_INFO("service_name: %s ", service_name.c_str());

    for(int i = 0; i < methodCnt; i++)
    {
        // 获取服务对象指定服务方法的描述
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc});
        // std::cout << "method_name: " << method_name << " is registered" << std::endl;
        LOG_INFO("method_name: %s is registered", method_name.c_str());
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}

void RpcProvider::Run()
{
    // 读取配置文件中服务器ip、port信息
    std::string ip = MprpcApp::GetInstance()->GetConfig()->Load("rpcserver_ip");
    uint16_t port = atoi(MprpcApp::GetInstance()->GetConfig()->Load("rpcserver_port").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TCP对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");

    // 绑定连接回调和消息读写回调方法 分离网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1)); // 绑定对象方法
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));


    // 设置muduo库的线程数量
    server.setThreadNum(4);

    // 把当前rpc节点要发布的服务全部注册到zk上，让rpc client可以从zk上发现服务
    ZkClient zkCli;
    zkCli.Start();
    // service_name为永久性节点 method_name为临时性节点
    for(auto &sp : m_serviceMap)
    {
        // /service_name
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for(auto &mp : sp.second.m_methodMap)
        {
            // /service_name/method_name
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL表示znode是一个临时节点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    std::cout << "RpcProvider start service at ip:" << ip << " port:" << port << std::endl;

    // 启动网络服务
    server.start();
    m_eventLoop.loop(); // epoll_wait
}

// 新的socket连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if(!conn->connected())
    {
        // 和rpc_client的连接断开了
        conn->shutdown();
    }
}

// header_size(4 bytes) + header_str + args_str
// 已建立连接用户的读写事件回调
// 反序列化请求，调用本地方法
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp)
{
    // 网络上接收的远程rpc调用请求的字符流
    std::string recv_buf = buf->retrieveAllAsString();

    // 从字符流中读取前四个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    // 根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4, header_size);

    mprpc::RpcHeader rpcHeader;

    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    
    if(rpcHeader.ParseFromString(rpc_header_str))
    {
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
        std::cout << "rpc_header_std: " << rpc_header_str << " parse success!" << std::endl;
    }
    else
    {
        std::cout << "rpc_header_std: " << rpc_header_str << " parse error!" << std::endl;
    }

    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印调试信息
    std::cout << "=======================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "=======================================" << std::endl;

    // 获取service对象和method方法
    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end())
    {
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if(mit == it->second.m_methodMap.end())
    {
        std::cout << method_name << " is not exist!" << std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.m_service; // 获取service对象
    const google::protobuf::MethodDescriptor *method = mit->second; // 获取method方法

    // 生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
        std::cout << "request parse error, content: " << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                                                                    (this, &RpcProvider::SendRpcResponse, conn, response);

    service->CallMethod(method, nullptr, request, response, done);
}

// 序列化响应，发送给请求方
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str)) // 对response进行序列化
    {
        // 序列化成功后，通过网络把rpc方法响应的结果发送给rpc方法的请求方
        conn->send(response_str);
    }
    else
    {
        std::cout << "serialize response_str error!" << std::endl;
    }
    conn->shutdown(); // 模拟HTTP的短链接服务，由rpcprovider主动断开连接
}