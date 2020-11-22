#include "drako/file_formats/dson.hpp"

#include <iostream>
#include <string>

int main()
{
    using namespace drako;

    const std::string text = "<var1>=<val1>\n"
                             "<var2>=<val2>\n"
                             "<var3>=<val3>\n"
                             "<var4>=<val4>\n"
                             "<var5>=<val5>\n";

    dson::object obj{ text };
    std::cout << obj;
}