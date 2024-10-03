# Mprpc
基于muduo库的Rpc通信框架

功能
1.实现基于muduo库的Rpc通信框架
2.利用protobuf进行Rpc方法和调用参数的序列化与反序列化
3.实现基于异步缓冲队列的日志系统
4.实现zookeeper进行Rpc方法的注册和分发

使用
bash ./autobuild.sh # 可一键编译
Rpc方法消费者和提供者用例见example/
需绑定配置文件，配置文件有Rpc方法提供者ip、端口号，zk的ip、端口号
