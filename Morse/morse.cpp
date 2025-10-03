#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

static unordered_map<string, char> morse_map;

void init_morse_map(void)
{
    morse_map["._"]   = 'a';
    morse_map["_..."] = 'b';
    morse_map["_._."] = 'c';
    morse_map["_.."]  = 'd';
    morse_map["."]    = 'e';
    morse_map[".._."] = 'f';
    morse_map["__."]  = 'g';
    morse_map["...."] = 'h';
    morse_map[".."]   = 'i';
    morse_map[".___"] = 'j';
    morse_map["_._"]  = 'k';
    morse_map["._.."] = 'l';
    morse_map["__"]   = 'm';
    morse_map["_."]   = 'n';
    morse_map["___"]  = 'o';
    morse_map[".__."] = 'p';
    morse_map["__._"] = 'q';
    morse_map["._."]  = 'r';
    morse_map["..."]  = 's';
    morse_map["_"]    = 't';
    morse_map[".._"]  = 'u';
    morse_map["..._"] = 'v';
    morse_map[".__"]  = 'w';
    morse_map["_.._"] = 'x';
    morse_map["_.__"] = 'y';
    morse_map["__.."] = 'z';

    morse_map[".____"] = '1';
    morse_map["..___"] = '2';
    morse_map["...__"] = '3';
    morse_map["...._"] = '4';
    morse_map["....."] = '5';
    morse_map["_...."] = '6';
    morse_map["__..."] = '7';
    morse_map["___.."] = '8';
    morse_map["____."] = '9';
    morse_map["_____"] = '0';
}

string morse_decode(string input)
{
    string result;

    // 1 Tokenize input
    vector<string> morse_in;
    stringstream stream(input);
    string temp;

    while (getline(stream, temp, ' '))
        morse_in.push_back(temp);

    // 3 Process input
    for (string seq: morse_in)
        result.push_back(morse_map[seq]);

    return result;
}

int main(void)
{
    string test = "._ _... _._. .____ ..___ ...__";

    init_morse_map();

    cout << morse_decode(test) << endl;

    return 0;
}
