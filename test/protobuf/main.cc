#include <iostream>
#include <string>
#include "test.pb.h"
using namespace fixbug;

int main()
{

    GetFriendListsResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    User *user1 = rsp.add_friend_list();
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(User::MAN);//定义在类的里面

    User *user2 = rsp.add_friend_list();
    user2->set_name("li si");
    user2->set_age(22);
    user2->set_sex(User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;

    return 0;

}
#if 0
    // 封装了login请求对象的数据
    LoginRequest reg;
    reg.set_name("zhang san");
    reg.set_pwd("123");

    std::string send_str;
    if (reg.SerializeToString(&send_str))
    {
        std::cout << send_str.c_str() << std::endl;
    } 

    LoginRequest res1;
    if(res1.ParseFromString(send_str)){
        std::cout << res1.name() << std::endl;
        std::cout << res1.pwd() << std::endl;
    }
    return 0;
    
#endif


/*
g++ main.cc test.pb.cc  -lprotobuf -lpthread


terminate called after throwing an instance of 'std::system_error'
  what():  Unknown error -1
Aborted (core dumped)

基本是编译的时候忘了加 -pthread或者-lpthread了。
*/