#ifndef DBTXT_H
#define DBTXT_H
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

class DBTxt {
private:
  std::string m_filename;
  std::unordered_map<std::string, std::string> m_users;

public:
    ~DBTxt();
    DBTxt(const std::string& filename = "users.txt");
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
};

#endif // DBTXT_H
