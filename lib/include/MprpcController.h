#pragma once

#include <google/protobuf/service.h>


// 父类是抽象类，需把抽象类方法全部实现，才能成为实现类
// 错误控制模块
class MprpcController : public google::protobuf::RpcController
{
public:
    MprpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string &reason);

    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);

private:
    bool m_failed;
    std::string m_errText;
};