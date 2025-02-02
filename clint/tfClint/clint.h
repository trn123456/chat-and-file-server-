﻿#ifndef CLINT_H
#define CLINT_H

#include<iostream>
//#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include<thread>
#include<vector>
#include<queue>
#include<mutex>
#include<string>
#include<condition_variable>
using namespace boost::asio;
using boost::system::error_code;
using namespace std;
//io_service service;
#pragma pack(1)  //预编译时字节不对齐
struct Head {
    unsigned char type;//
    unsigned int length;
    unsigned int id;//如果是文件类型则把数据传给相应的id
    int          packid;//描述如果是文件是第几个数据包保证客户端若是多线程文件传输的顺序性
    unsigned int account;//账号
    unsigned int mima;//密码
    unsigned int sendto;//0给服务器 非0即要发给的账户
    unsigned int status=0; //0初始状态(server线程开始处理) 1//已处理(server直接跳过) //2//消息可丢弃(serer将该消息出队) packid为-1时表示文件大小
};
struct fHead {
    char   type;
    int    length;
    int    id;
    int    packid;
    int    offset= 0;
    int    status;
};
#pragma pack()  //恢复字节对齐
const int sizeofFhead = sizeof(fHead);
const int sizeofhead=sizeof(Head);
enum reback {
    readerror = 3,
    writeerror = 4,
    passerror = 5,       //密码错误
    linkerror = 6,    //连接断开等
    login = 7,        //已经在线
    logsucc = 8,    //登陆成功
    loginsucc = 9,  //注册成功
    loginfal=10,
    fitrfail=11,  //文件传输失败
    fitrbusy =12,    //文件空闲服务队列已满
    firetrst=13
};


class clint
{
public:
    queue<string>readqu;
    mutex re_mutex;
    condition_variable re_mutex_cond;
    atomic<bool>haveMes;
    queue<string>writequ;
    mutex wr_mutex;


    bool is_have_task=false;
    condition_variable wr_cond;

    unsigned int account=0;
    unsigned int pass=0;
private:
    shared_ptr<ip::tcp::socket>sock_;
    string re_buf;
    string wr_buf;
    Head he;
public:
    clint(shared_ptr<ip::tcp::socket>sock);
    void read_thread();
    void write_thread();
    int headanylize(Head &head);
    Head getHead(string &buff) {
        string headme(buff.begin(), buff.begin() + sizeof(Head));
        Head hh;
        memcpy(&hh, headme.c_str(), sizeof(Head));
        return hh;


    }
    static Head GetHead(string &buff) {
        Head hh;
        memcpy(&hh, buff.c_str(), sizeof(Head));
        return hh;
    }
};

#endif // CLINT_H
