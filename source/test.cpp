#include <fstream>
#include <iostream>


bool moveToStartOfLine(std::ifstream& fs){
    fs.seekg(-1, std::ios_base::cur);
    for(long i = fs.tellg(); i > 0; i--)
    {
        if(fs.peek() == '\n')
        {
            fs.get();
            return true;
        }
        fs.seekg(i, std::ios_base::beg);
    }
    return false;
}

std::string getLastLineInFile(std::ifstream& fs){
    // Go to the last character before EOF
    fs.seekg(-1, std::ios_base::end);
    if (!moveToStartOfLine(fs))
        return "";

    std::string lastline = "";
    getline(fs, lastline);
    return lastline;
}

int main(){
    const std::string filename = "NY_att_10_fixed.txt";
    std::ifstream fs;
    fs.open(filename.c_str(), std::fstream::in);
    if(!fs.is_open()){
        std::cout << "Could not open file" << std::endl;
        return -1;
    }

    std::cout << getLastLineInFile(fs) << std::endl;

    return 0;
}
