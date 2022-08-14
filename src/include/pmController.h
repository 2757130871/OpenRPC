#pragma once

#include <google/protobuf/service.h>
#include <string>

class pmController : public google::protobuf::RpcController
{
private:
    bool isFailed_;
    std::string errText_;

public:
    /* data */
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string &reason);

    // 未实现
    void Reset();
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure *callback);

    pmController();
    ~pmController();
};
