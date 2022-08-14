
#include "pmController.h"

pmController::pmController(/* args */) : isFailed_(false), errText_("")
{
}

pmController::~pmController()
{
}

bool pmController::Failed() const
{
    return isFailed_;
}
std::string pmController::ErrorText() const
{
    return errText_;
}
void pmController::SetFailed(const std::string &reason)
{
    isFailed_ = true;
    errText_ = reason;
}

//暂未实现
void pmController::Reset() {}
void pmController::StartCancel() {}
bool pmController::IsCanceled() const { return false; }
void pmController::NotifyOnCancel(google::protobuf::Closure *callback) {}
