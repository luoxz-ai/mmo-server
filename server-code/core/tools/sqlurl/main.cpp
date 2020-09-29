#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>

#include "NormalCrypto.h"
#include "StringAlgo.h"
#include "fmt/format.h"
#include "fmt/printf.h"
#include "get_opt.h"

int main(int argc, char** argv)
{

    auto crypto = NormalCrypto::default_instance();

    get_opt     opt(argc, (const char**)argv);
    std::string txt     = opt["--txt"];
    bool        bDecode = opt.has("--decode");
    if(bDecode)
    {
        auto str = crypto.Decode(txt);
        std::cout << "recovered text: " << str << std::endl;
    }
    else
    {
        auto str = crypto.Encode(txt);

        std::cout << "cipher text: " << str << std::endl;
    }
}