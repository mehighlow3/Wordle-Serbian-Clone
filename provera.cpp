#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <iostream>
#include <fstream>
#include <vector>

void insertDataIntoMySQL(const std::vector<std::string>& data) 
{
    try {

        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "miha907", "miha907miha907");

        con->setSchema("recko");

        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO provera (proveri) VALUES (?)")
        );

        for (const std::string& line : data) 
        {
            pstmt->setString(1, line);  
            pstmt->executeUpdate();     
        }

        delete con;
    } catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what() << std::endl;
    }
}

std::vector<std::string> readTxtFile(const std::string& filePath) 
{
    std::ifstream file(filePath);
    std::vector<std::string> data;
    std::string line;

    while (std::getline(file, line))
    {
        data.push_back(line);
    }

    return data;
}

int main() 
{
    std::string filePath = "reci.txt";

    std::vector<std::string> data = readTxtFile(filePath);

    insertDataIntoMySQL(data);

    return 0;
}
