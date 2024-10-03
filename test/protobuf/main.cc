#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main()
{
    // LoginRequest req;
    // req.set_name("chen maoyin");
    // req.set_pwd("km_cmy");

    // // 序列化
    // std::string send_str;
    // if(req.SerializeToString(&send_str))
    // {
    //     std::cout << send_str << std::endl;
    // }

    // // 反序列化
    // LoginRequest reqB;
    // if(reqB.ParseFromString(send_str))
    // {
    //     std::cout << reqB.name() << std::endl;
    //     std::cout << reqB.pwd() << std::endl;
    // }

    GetFriendListsRequest rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    User *user1 = rsp.add_friend_list();
    user1->set_name("wang liangsen");
    user1->set_age(30);
    user1->set_sex(User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;



    return 0;
}

