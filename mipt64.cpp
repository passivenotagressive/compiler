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
    set<int> words; //словарь меток одинарного слова
    set<int> dwords; //словарь меток двойного слова
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
        }
        cin >> a;
    }
}

