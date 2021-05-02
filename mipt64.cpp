#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <set>

using namespace std;

char mem[1024 * 1024 * 2]; //модель памяти
long regs[32]; //модель регистров
int flag;

map<string, int> labels; //таблица меток

map<string, string> types;

set<string> comands = {"halt", "svc", "add", "sub", "mul", "and", "or", "xor", "nand", "addd",
                       "subd", "muld", "divd", "itod", "dtoi", "bl", "cmp", "cmpd", "cne",
                       "ceq", "cle", "clt", "cge", "cgt", "ld", "st"};

enum code {
    HALT = 0,
    SVC = 1,
    ADD = 2,
    SUB = 3,
    MUL = 4,
    DIV = 5,
    MOD = 6,
    AND = 7,
    OR = 8,
    XOR = 9,
    NAND = 10,
    ADDD = 12,
    SUBD = 13,
    MULD = 14,
    DIVD = 15,
    ITOD = 16,
    DTOI = 17,
    BL = 20,
    CMP = 22,
    CMPD = 23,
    CNE = 25,
    CEQ = 26,
    CLE = 27,
    CLT = 28,
    CGE = 29,
    CGT = 30,
    LD = 31,
    ST = 32,
};

void type_fill() {
    types["halt"] = "RR";
    types["svc"] = "RR";
    types["add"] = "RR";
    types["sub"] = "RR";
    types["mul"] = "RR";
    types["div"] = "RR";
    types["mod"] = "RR";
    types["and"] = "RR";
    types["or"] = "RR";
    types["xor"] = "RR";
    types["nand"] = "RR";
    types["shl"] = "RR";
    types["shr"] = "RR";
    types["addd"] = "RR";
    types["subd"] = "RR";
    types["muld"] = "RR";
    types["divd"] = "RR";
    types["itod"] = "RR";
    types["dtoi"] = "RR";
    types["bl"] = "B";
    types["cmp"] = "RR";
    types["cmpd"] = "RR";
    types["cne"] = "RR";
    types["ceq"] = "RR";
    types["cle"] = "RR";
    types["clt"] = "RR";
    types["cge"] = "RR";
    types["cgt"] = "RR";
    types["ld"] = "RM";
    types["st"] = "RM";
}

vector<char> int_for_mem(int n) {
    vector<char> r(4);
    int d = 1 << 8;
    for (int i = 0; i < 3; ++i) {
        r[i] = char(n % d);
        n /= d;
    }
    return r;
}

vector<char> long_for_mem(long n) {
    vector<char> r(8);
    int d = 1 << 8;
    for (int i = 0; i < 7; ++i) {
        r[i] = char(n % d);
        n /= d;
    }
    return r;
}

int main(){
    type_fill();
    set<int> words; //множество меток одинарного слова
    set<int> dwords; //множество меток двойного слова
    set<pair<int, int>> bytes; //словарь байтов
    int pc = 0;
    string a;
    cin >> a;
    while (a != "end") { //первый проход
        if (a[a.size() - 1] == ':') {
            string b = a.substr(0, a.size() - 1);
            labels[b] = pc;
        } else if (a == "word") {
            int n;
            cin >> n;
            vector<char> r = int_for_mem(n);
            for (int i = 0; i < 4; ++i) {
                mem[pc + i] = r[i];
            }
            words.insert(pc);
            pc += 4;
        } else if (a == "dword") {
            long n;
            cin >> n;
            vector<char> r = long_for_mem(n);
            for (int i = 0; i < 8; ++i) {
                mem[pc + i] = r[i];
            }
            dwords.insert(pc);
            pc += 8;
        } else if (a == "double") {
            double n;
            cin >> n;
        } else if (a == "bytes") {
            int n;
            cin >> n;
            n += (n % 4); //выравниваем данные на границу кратную 4
            for (int i = 0; i < n; ++i) {
                mem[pc + i] = 0;
            }
            pair<int, int> p;
            p.first = pc;
            p.second = n;
            bytes.insert(p);
            pc += n;
        } else if (comands.count(a) > 0) {
            ++pc;
        }
        cin >> a;
    }
    cin >> a;

    pc = 0; //второй проход
    cin >> a;
    while (a != "end") {
        if (a == "word") {

        }
    }

    regs[31] = labels[a]; //устанавливаем метку начала программы
    regs[29] = 1024 * 2048 - 1; //устанавливаем стек на начало


}

