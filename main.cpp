#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <set>

using namespace std;

map<string, int> codes;
map<string, string> code_type;
map<int, string> codes_nums;
map<string, int> labels;

int regs[16];
int mem[1024 * 1024];
int flags;

vector<unsigned int> dbl_to_int(double a) {
    vector<unsigned int> res(2);
    unsigned int x = 0, y = 0;
    double A = a;
    int B = 0;
    if (a < 0) {
        A *= -1;
    } else if (a == 0) {
        res[0] = 0;
        res[1] = 0;
        return res;
    }
    while (A >= 2) {
        A /= 2;
        ++B;
    }
    while (A < 1) {
        A *= 2;
        --B;
    }
    B += 511;
    if (B > 1022) {
        x += (1023 << 21);
        y = 0;
    } else {
        x += B;
        for (int i = 0; i < 21; ++i) {
            x = (x << 1);
            if (A >= 1) {
                ++x;
                --A;
            }
            A *= 2;
        }
    }
    for (int i = 0; i < 32; ++i) {
        y = (y << 1);
        if (A >= 1) {
            ++y;
            --A;
        }
        A *= 2;
    }
    if (a < 0) {
        x += (1 << 31);
    }
    res[0] = x;
    res[1] = y;
    return res;
}

double int_to_dbl(vector<unsigned int> a) {
    double res;
    double A = 0;
    int B = 0;
    unsigned int x = a[1];
    unsigned int y = a[0];
    for (int i = 0; i < 32; ++i) {
        A = A / 2;
        A += x % 2;
        x = x >> 1;
    }
    for (int i = 0; i < 21; ++i) {
        A = A / 2;
        A += y % 2;
        y = y >> 1;
    }
    unsigned int z = (y >> 10);
    if (z == 1) {
        y -= (1 << 10);
        A *= -1;
    }
    B = (int) y - 511;
    res = A * pow(2, B);
    if ((y >> 31) == 1) {
        res *= -1;
    }
    return res;

}

vector<string> string_to_vector(const string *line) {
    stringstream x;
    vector<string> res;
    string word;
    x << *line;
    while (x >> word) {
        res.push_back(word);
    }
    return res;
}

int cmp(int a, int b) {
    if (a < b) {
        return -1;
    } else if (a == b) {
        return 0;
    } else {
        return 1;
    }
}

int cmpd(double a, double b) {
    if (a < b) {
        return -1;
    } else if (a == b) {
        return 0;
    } else {
        return 1;
    }
}

void codes_nums_fill() {
    codes_nums[0] = "halt";
    codes_nums[1] = "syscall";
    codes_nums[2] = "add";
    codes_nums[3] = "addi";
    codes_nums[4] = "sub";
    codes_nums[5] = "subi";
    codes_nums[6] = "mul";
    codes_nums[7] = "muli";
    codes_nums[8] = "div";
    codes_nums[9] = "divi";
    codes_nums[12] = "lc";
    codes_nums[13] = "shl";
    codes_nums[14] = "shli";
    codes_nums[15] = "shr";
    codes_nums[16] = "shri";
    codes_nums[17] = "and";
    codes_nums[18] = "andi";
    codes_nums[19] = "or";
    codes_nums[20] = "ori";
    codes_nums[21] = "xor";
    codes_nums[22] = "xori";
    codes_nums[23] = "not";
    codes_nums[24] = "mov";
    codes_nums[32] = "addd";
    codes_nums[33] = "subd";
    codes_nums[34] = "muld";
    codes_nums[35] = "divd";
    codes_nums[36] = "itod";
    codes_nums[37] = "dtoi";
    codes_nums[38] = "push";
    codes_nums[39] = "pop";
    codes_nums[40] = "call";
    codes_nums[41] = "calli";
    codes_nums[42] = "ret";
    codes_nums[43] = "cmp";
    codes_nums[44] = "cmpi";
    codes_nums[45] = "cmpd";
    codes_nums[46] = "jmp";
    codes_nums[47] = "jne";
    codes_nums[48] = "jeq";
    codes_nums[49] = "jle";
    codes_nums[50] = "jl";
    codes_nums[51] = "jge";
    codes_nums[52] = "jg";
    codes_nums[64] = "load";
    codes_nums[65] = "store";
    codes_nums[66] = "load2";
    codes_nums[67] = "store2";
    codes_nums[68] = "loadr";
    codes_nums[69] = "load2";
    codes_nums[70] = "loadr2";
    codes_nums[71] = "storer2";
}

void codes_fill() {
    codes["halt"] = 0;
    codes["syscall"] = 1;
    codes["add"] = 2;
    codes["addi"] = 3;
    codes["sub"] = 4;
    codes["subi"] = 5;
    codes["mul"] = 6;
    codes["muli"] = 7;
    codes["div"] = 8;
    codes["divi"] = 9;
    codes["lc"] = 12;
    codes["shl"] = 13;
    codes["shli"] = 14;
    codes["shr"] = 15;
    codes["shri"] = 16;
    codes["and"] = 17;
    codes["andi"] = 18;
    codes["or"] = 19;
    codes["ori"] = 20;
    codes["xor"] = 21;
    codes["xori"] = 22;
    codes["not"] = 23;
    codes["mov"] = 24;
    codes["addd"] = 32;
    codes["subd"] = 33;
    codes["muld"] = 34;
    codes["divd"] = 35;
    codes["itod"] = 36;
    codes["dtoi"] = 37;
    codes["push"] = 38;
    codes["pop"] = 39;
    codes["call"] = 40;
    codes["calli"] = 41;
    codes["ret"] = 42;
    codes["cmp"] = 43;
    codes["cmpi"] = 44;
    codes["cmpd"] = 45;
    codes["jmp"] = 46;
    codes["jne"] = 47;
    codes["jeq"] = 48;
    codes["jle"] = 49;
    codes["jl"] = 50;
    codes["jge"] = 51;
    codes["jg"] = 52;
    codes["load"] = 64;
    codes["store"] = 65;
    codes["load2"] = 66;
    codes["store2"] = 67;
    codes["loadr"] = 68;
    codes["storer"] = 69;
    codes["loadr2"] = 70;
    codes["storer2"] = 71;
}

void type_fill() {
    code_type["halt"] = "RI";
    code_type["syscall"] = "RI";
    code_type["add"] = "RR";
    code_type["addi"] = "RI";
    code_type["sub"] = "RR";
    code_type["subi"] = "RI";
    code_type["mul"] = "RR";
    code_type["muli"] = "RI";
    code_type["div"] = "RR";
    code_type["divi"] = "RI";
    code_type["lc"] = "RI";
    code_type["shl"] = "RR";
    code_type["shli"] = "RI";
    code_type["shr"] = "RR";
    code_type["shri"] = "RI";
    code_type["and"] = "RR";
    code_type["andi"] = "RI";
    code_type["or"] = "RR";
    code_type["ori"] = "RI";
    code_type["xor"] = "RR";
    code_type["xori"] = "RI";
    code_type["not"] = "RI";
    code_type["mov"] = "RR";
    code_type["addd"] = "RR";
    code_type["subd"] = "RR";
    code_type["muld"] = "RR";
    code_type["divd"] = "RR";
    code_type["itod"] = "RR";
    code_type["dtoi"] = "RR";
    code_type["push"] = "RI";
    code_type["pop"] = "RI";
    code_type["call"] = "RR";
    code_type["calli"] = "J";
    code_type["ret"] = "J";
    code_type["cmp"] = "RR";
    code_type["cmpi"] = "RI";
    code_type["cmpd"] = "RR";
    code_type["jmp"] = "J";
    code_type["jne"] = "J";
    code_type["jeq"] = "J";
    code_type["jle"] = "J";
    code_type["jl"] = "J";
    code_type["jge"] = "J";
    code_type["jg"] = "J";
    code_type["load"] = "RM";
    code_type["store"] = "RM";
    code_type["load2"] = "RM";
    code_type["store2"] = "RM";
    code_type["loadr"] = "RR";
    code_type["storer"] = "RR";
    code_type["loadr2"] = "RR";
    code_type["storer2"] = "RR";
}

int RM(const string *code, const string *r, const int *adress) {
    int x = codes[*code] << 24;
    int k = 1;
    if ((*r)[(*r).size() - 1] == ',') {
        k = 2;
    }
    int y = stoi((*r).substr(1, (*r).size() - k)) << 20;
    return x + y + *adress;
}

int RR(const string *code, const string *r1, const string *r2, const int *modifier) {
    int x = codes[*code] << 24;
    int k1 = 1;
    if ((*r1)[(*r1).size() - 1] == ',') {
        k1 = 2;
    }
    int k2 = 1;
    if ((*r2)[(*r2).size() - 1] == ',') {
        k2 = 2;
    }
    int y = stoi((*r1).substr(1, (*r1).size() - k1)) << 20;
    int z = stoi((*r2).substr(1, (*r2).size() - k2)) << 16;
    int w = *modifier;
    if (w < 0) {
        w *= -1;
        w += (1 << 15);
    }
    return x + y + z + w;
}

int RI(const string *code, const string *r, const int *operand) {
    int x = codes[*code] << 24;
    int k = 1;
    if ((*r)[(*r).size() - 1] == ',') {
        k = 2;
    }
    int y = stoi((*r).substr(1, (*r).size() - k));
    y = y << 20;
    int w = *operand;
    if (w < 0) {
        w *= -1;
        w += (1 << 19);
    }
    return x + y + w;
}

int J(const string *code, const int *operand) {
    int x = codes[*code] << 24;
    return *operand + x;
}

int main() {
    codes_nums_fill();
    codes_fill();
    type_fill();
    ifstream in;
    ofstream out;
    string a;
    vector<string> w;
    regs[14] = 1024 * 1024 - 1;
    int N;
    set<int> words;
    set<int> doubles;
    int pc = 0; //первый проход
    in.open("input.fasm");
    out.open("output.txt");

    while (a != "end") {
        if (a[a.size() - 1] == ':') {
            string b = a.substr(0, a.size() - 1);
            labels[b] = pc;
        }
        if (a == "word") {
            int x;
            in >> x;
            mem[pc] = x;
            words.insert(pc);
            ++pc;
        } else if (a == "double") {
            double x;
            in >> x;
            vector<unsigned int> r = dbl_to_int(x);
            mem[pc] = r[0];
            mem[pc + 1] = r[1];
            doubles.insert(pc);
            pc += 2;
        } else if (!code_type[a].empty()) {
            ++pc;
        }
        in >> a;
    }
    in >> a;
    in.close();
    regs[15] = labels[a];
    pc = 0; //второй проход
    in.open("input.fasm");
    getline(in, a);
    w = string_to_vector(&a);
    while (w[0] != "end") {
        int i = 0;
        int s = w.size();
        if (w[i][w[0].size() - 1] == ':'){
            ++i;
        }
        if (i < s && w[i] == "word") {
            i += 2;
            ++pc;
        }
        if (i < s && w[i] == "double") {
            i += 2;
            ++pc;
        }
        if (i < s) {
            string type = code_type[w[i]];
            if (type == "RM") {
                string r = w[i + 1];
                string link = w[i + 2];
                int adress;
                if ('0' <= link[0] && '9' >= link[0]) {
                    adress = stoi(link);
                } else {
                    adress = labels[link];
                }
                mem[pc] = RM(&w[i], &r, &adress);
            } else if (type == "RR") {
                string r1 = w[i + 1];
                string r2 = w[i + 2];
                int modifier;
                if (w.size() == i + 3) {
                    modifier = 0;
                } else {
                    string link = w[i + 3];
                    if (link[0] == '-' || ('0' <= link[0] && '9' >= link[0])) {
                        modifier = stoi(link);
                    } else {
                        modifier = labels[link];
                    }
                }
                mem[pc] = RR(&w[i], &r1, &r2, &modifier);
            } else if (type == "RI") {
                string r = w[i + 1];
                string link = w[i + 2];
                int operand;
                if (link[0] == '-' || ('0' <= link[0] && '9' >= link[0])) {
                    operand = stoi(link);
                } else {
                    operand = labels[link];
                }
                mem[pc] = RI(&w[i], &r, &operand);
            } else {
                int adress;
                string link = w[i + 1];
                if ('0' <= link[0] && '9' >= link[0]) {
                    adress = stoi(link);
                } else {
                    adress = labels[link];
                }
                mem[pc] = J(&w[i], &adress);
            }
            ++pc;
        }
        getline(in, a);
        w = string_to_vector(&a);
    }
    in.close();
    while (true) { //исполнение
        int comand;
        if (words.find(regs[15]) != words.end()) {
            ++regs[15];
        } else if (doubles.find(regs[15]) != doubles.end()) {
            regs[15] += 2;
        }
        comand = mem[regs[15]];
        int n = comand >> 24;
        string code = codes_nums[n];
        string type = code_type[code];
        if (type == "J") {
            int adress = comand ^(n << 24);
            if (n == 41) {
                --regs[14];
                mem[regs[14]] = regs[15] + 1;
                regs[15] = adress;
            } else if (n == 42) {
                regs[15] = mem[regs[14]];
                regs[14] += 1 + adress;
            } else if (n == 46) {
                regs[15] = adress;
            } else if (n == 47) {
                if (flags != 0) {
                    regs[15] = adress;
                } else {
                    ++regs[15];
                }
            } else if (n == 48) {
                if (flags == 0) {
                    regs[15] = adress;
                } else {
                    ++regs[15];
                }
            } else if (n == 49) {
                if (flags < 1) {
                    regs[15] = adress;
                } else {
                    ++regs[15];
                }
            } else if (n == 50) {
                if (flags == -1) {
                    regs[15] = adress;
                } else {
                    ++regs[15];
                }
            } else if (n == 51) {
                if (flags > -1) {
                    regs[15] = adress;
                } else {
                    ++regs[15];
                }
            } else if (n == 52) {
                if (flags == 1) {
                    regs[15] = adress;
                } else {
                    ++regs[15];
                }
            }
        } else if (type == "RM") {
            int adress, r;
            adress = ((comand >> 20) << 20) ^ (comand);
            r = ((comand >> 4) << 4) ^ (comand);
            if (n == 64) {
                regs[r] = mem[adress];
            } else if (n == 65) {
                mem[adress] = regs[r];
            } else if (n == 66) {
                regs[r] = mem[adress];
                regs[r + 1] = mem[adress + 1];
            } else if (n == 67) {
                mem[adress] = regs[r];
                mem[adress + 1] = regs[r + 1];
            }
            regs[15] += 1;
        } else if (type == "RI") {
            int arg, r;
            arg = ((comand >> 20) << 20) ^ (comand);
            comand = comand >> 20;
            r = ((comand >> 4) << 4) ^ (comand);
            if (n == 0) {
                return 1;
            } else if (n == 1) {
                if (arg == 100) {
                    cin >> regs[r];
                } else if (arg == 101) {
                    double x;
                    vector<unsigned int> d;
                    cin >> x;
                    d = dbl_to_int(x);
                    regs[r] = d[0];
                    regs[r + 1] = d[1];
                } else if (arg == 102) {
                    out << regs[r];
                } else if (arg == 103) {
                    double x;
                    vector<unsigned int> s(2);
                    s[0] = regs[r];
                    s[1] = regs[r + 1];
                    x = int_to_dbl(s);
                    out << x;
                } else if (arg == 104) {
                    unsigned char x;
                    in >> x;
                    regs[r] = (int) x;
                } else if (arg == 105) {
                    char x = (char) regs[r];
                    out << x;
                } else if (arg == 0) {
                    return 0;
                }
            } else if (n == 3) {
                regs[r] += arg;
            } else if (n == 5) {
                regs[r] -= arg;
            } else if (n == 7) {
                long long x = (regs[r] + (((long long) regs[r + 1]) << 32)) * arg;
                regs[r + 1] = (int) (x >> 32);
                regs[r] = (int) (((x >> 32) << 32) ^ x);
            } else if (n == 9) {
                long long x = (((long long) regs[r + 1]) << 32) + regs[r];
                regs[r] = (int) (x / (long long) arg);
                regs[r + 1] = (int) (x % (long long) arg);
            } else if (n == 12) {
                regs[r] = arg;
            } else if (n == 14) {
                regs[r] = regs[r] << arg;
            } else if (n == 16) {
                regs[r] = regs[r] >> arg;
            } else if (n == 18) {
                regs[r] = regs[r] & arg;
            } else if (n == 20) {
                regs[r] = regs[r] | arg;
            } else if (n == 22) {
                regs[r] = regs[r] ^ arg;
            } else if (n == 23) {
                regs[r] = !regs[r];
            } else if (n == 38) {
                --regs[14];
                mem[regs[14]] = regs[r] + arg;
            } else if (n == 39) {
                regs[r] = mem[regs[14]] + arg;
                ++regs[14];
            } else if (n == 44) {
                flags = cmp(regs[r], arg);
            }
            regs[15] += 1;
        } else if (type == "RR") {
            int r1, r2, modifier;
            modifier = ((comand >> 16) << 16) ^ (comand);
            comand = comand >> 16;
            r2 = ((comand >> 4) << 4) ^ (comand);
            comand = comand >> 4;
            r1 = ((comand >> 4) << 4) ^ (comand);
            if (n == 2) {
                regs[r1] = regs[r1] + regs[r2] + modifier;
            } else if (n == 4) {
                regs[r1] = regs[r1] - (regs[r2] + modifier);
            } else if (n == 6) {
                long long x = (long long) regs[r1] * ((long long) regs[r2] + modifier);
                regs[r1 + 1] = (int) (x >> 32);
                int z = (int) (((x >> 32) << 32) ^ x);
                regs[r1] = (int) z;
            } else if (n == 8) {
                long long int x = (((long long) regs[r1 + 1]) << 32) + regs[r1];
                regs[r1] = (int) (x / ((long long) regs[r2]));
                regs[r1 + 1] = (int) (x % ((long long) regs[r2]));
            } else if (n == 13) {
                regs[r1] = regs[r1] << regs[r2];
            } else if (n == 15) {
                if (regs[r2] >= 32) {
                    regs[r1] = 0;
                } else {
                    regs[r1] = regs[r1] >> regs[r2];
                }
            } else if (n == 17) {
                regs[r1] = regs[r1] & regs[r2];
            } else if (n == 19) {
                regs[r1] = regs[r1] | regs[r2];
            } else if (n == 21) {
                regs[r1] = regs[r1] ^ regs[r2];
            } else if (n == 24) {
                regs[r1] = regs[r2] + modifier;
            } else if (n == 32) {
                vector<unsigned int> R0(2);
                vector<unsigned int> R1(2);
                R0[0] = regs[r1];
                R0[1] = regs[r1 + 1];
                R1[0] = regs[r2];
                R1[1] = regs[r2 + 1];
                double x, y;
                x = int_to_dbl(R0);
                y = int_to_dbl(R1);
                x += y + modifier;
                R0 = dbl_to_int(x);
                regs[r1] = R0[0];
                regs[r1 + 1] = R0[1];
            } else if (n == 33) {
                vector<unsigned int> R0(2);
                vector<unsigned int> R1(2);
                R0[0] = regs[r1];
                R0[1] = regs[r1 + 1];
                R1[0] = regs[r2];
                R1[1] = regs[r2 + 1];
                double x, y;
                x = int_to_dbl(R0);
                y = int_to_dbl(R1);
                x -= y + modifier;
                R0 = dbl_to_int(x);
                regs[r1] = R0[0];
                regs[r1 + 1] = R0[1];
            } else if (n == 34) {
                vector<unsigned int> R0(2);
                vector<unsigned int> R1(2);
                R0[0] = regs[r1];
                R0[1] = regs[r1 + 1];
                R1[0] = regs[r2];
                R1[1] = regs[r2 + 1];
                double x, y;
                x = int_to_dbl(R0);
                y = int_to_dbl(R1);
                double z = x * y;
                R0 = dbl_to_int(z);
                regs[r1] = R0[0];
                regs[r1 + 1] = R0[1];

            } else if (n == 35) {
                vector<unsigned int> R0(2);
                vector<unsigned int> R1(2);
                R0[0] = regs[r1];
                R0[1] = regs[r1 + 1];
                R1[0] = regs[r2];
                R1[1] = regs[r2 + 1];
                double x, y;
                x = int_to_dbl(R0);
                y = int_to_dbl(R1);
                x = x / y;
                R0 = dbl_to_int(x);
                regs[r1] = R0[0];
                regs[r1 + 1] = R0[1];
            } else if (n == 36) {
                double x = regs[r2];
                vector<unsigned int> R(2);
                R = dbl_to_int(x);
                regs[r1] = R[0];
                regs[r1 + 1] = R[1];
            } else if (n == 37) {
                vector<unsigned int> R(2);
                R[0] = regs[r2];
                R[1] = regs[r2 + 1];
                double x = int_to_dbl(R);
                regs[r1] = floor(x);
            } else if (n == 40) {
                --regs[14];
                mem[regs[14]] = regs[15] + 1;
                regs[15] = regs[r2] - 1;
                regs[r1] = regs[r2];
            } else if (n == 43) {
                flags = cmp(regs[r1], regs[r2]);
            } else if (n == 45) {
                vector<unsigned int> R0(2);
                vector<unsigned int> R1(2);
                R0[0] = regs[r1];
                R0[1] = regs[r1 + 1];
                R1[0] = regs[r2];
                R1[1] = regs[r2 + 1];
                double x, y;
                x = int_to_dbl(R0);
                y = int_to_dbl(R1);
                flags = cmpd(x, y);
            } else if (n == 68) {
                if (r2 == 14) {
                    regs[r1] = mem[regs[14] + modifier];
                } else {
                    regs[r1] = mem[r2 + modifier];
                }
            } else if (n == 69) {
                mem[r2 + modifier] = regs[r1];
            } else if (n == 70) {
                regs[r1] = mem[r2 + modifier];
                regs[r1 + 1] = mem[r2 + modifier + 1];
            } else if (n == 71) {
                mem[r2 + modifier] = regs[r1];
                mem[r2 + modifier + 1] = regs[r1 + 1];
            }
            regs[15] += 1;
        }
    }
    out.close();
}
