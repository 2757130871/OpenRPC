
#include <iostream>
#include <string>

#include "test1.pb.h"

using namespace std;

void test1()
{
    fixbug::LoginRequest req;

    req.set_name("张三");
    req.set_pwd("cxr111");

    cout << endl
         << "------------" << endl;

    //序列化为string
    string str;
    if (req.SerializeToString(&str))
    {
        cout << str << endl;
    }

    //从string反序列化
    fixbug::LoginRequest req2;
    req2.ParseFromString(str);

    cout << req2.name() << endl;
    cout << req2.pwd() << endl;
}

void test2()
{

    fixbug::LoginResponse resp;

    resp.set_success(true);
    fixbug::ErrorCode *errcode = resp.mutable_errcode();

    errcode->set_errcode(1123);
    errcode->set_errmsg("ERROR");

    std::string str;
    resp.SerializeToString(&str);

    cout << str << endl;
}

void test3()
{
    fixbug::GetFriendListResponse resp;

    //此函数内部创建一个对象并返回其地址
    fixbug::User *user1 = resp.add_user_list();

    user1->set_age(11);
    user1->set_name("micro");
    user1->set_sex(fixbug::User::MAN);

    fixbug::User *user2 = resp.add_user_list();

    user2->set_age(22);
    user2->set_name("merry");
    user2->set_sex(fixbug::User::WOMAN);

    //通过下标访问user_list内的某个user 不允许修改
    const fixbug::User &user_index = resp.user_list(1);

    cout << endl
         << "------------" << endl;
    // user_index
    cout << user_index.age() << endl;
    cout << user_index.name() << endl;
    cout << endl
         << "------------" << endl;

    std::string str;
    resp.SerializeToString(&str);

    cout << str << endl;
    cout << user2->sex() << endl;
    cout << "user_list size: " << resp.user_list_size() << endl;
}

int main()
{
    test3();
}
