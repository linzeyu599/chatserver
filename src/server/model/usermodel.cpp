#include "usermodel.hpp"
#include "db.hpp"
#include <iostream>
using namespace std;

//User表的增加方法
bool UserModel::insert(User &user)
{
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    MySQL mysql;//定义一个mysql对象 
    if (mysql.connect())//连接成功了 
    {
        if (mysql.update(sql))//更新这个sql语句传进去 
        {
            //获取插入成功的用户数据生成的主键id
            user.setId(mysql_insert_id(mysql.getConnection()));//拿到数据库生成的id作为用户的id号 
            return true;
        }
    }

    return false;
}

//根据用户号码查询用户信息
User UserModel::query(int id)
{
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);

    MySQL mysql;//创建mysql对象 
    if (mysql.connect())//连接成功 
    {
        MYSQL_RES *res = mysql.query(sql);//调用mysql数据库的查询 
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);//获取行，用主键查的，查一行 
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));//atoi把字符串转为int 
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);//释放资源 
                return user;
            }
        }
    }

    return User();
}

//更新用户的状态信息
bool UserModel::updateState(User user)
{
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());

    MySQL mysql;//创建mysql对象 
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

//重置用户的状态信息
void UserModel::resetState()
{
    //1.组装sql语句
    char sql[1024] = "update user set state = 'offline' where state = 'online'";

    MySQL mysql;//定义mysql对象 
    if (mysql.connect())//连接成功 
    {
        mysql.update(sql);//更新 
    }
}
