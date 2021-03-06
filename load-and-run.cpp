#include<iostream>
#include<vector>
#include<map>
#include <cmath>

using namespace std;

int mem[1024 * 1024];
int regs[16];
int flags;


map<int, string> codes_nums;
map<string, string> code_type;

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
} //заполнение словаря номер-команда

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
} //заполнение словаря команда-тип

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

} //перевод из двух регистров в число двойной точности

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
} //перевод из двочиной записи числа в два регистра int'ов

int cmp(int a, int b) {
    if (a < b) {
        return -1;
    } else if (a == b) {
        return 0;
    } else {
        return 1;
    }
} //сравнение целых чисел

int cmpd(double a, double b) {
    if (a < b) {
        return -1;
    } else if (a == b) {
        return 0;
    } else {
        return 1;
    }
} //сравнение действительных чисел


int main(){
    codes_nums_fill();
    type_fill();
    FILE *fp = fopen("input.bin", "rb");

    int start; //считывание адреса начала исполнения программы
    fseek(fp, 28, SEEK_SET);
    fread(&start, sizeof(int), 1, fp);
    regs[15] = start;

    fseek(fp , 0 , SEEK_END);
    long size = ftell(fp);

    regs[14] = 1024 * 1024 - 1;
    fseek(fp, 512, SEEK_SET); //ставим указатель на начало раздела кода
    int pointer = 0;
    int word;
    while (pointer * 4 + 512 < size) {
        fread(&word, sizeof(int), 1, fp);
        mem[pointer] = word;
        pointer++;
    }
    while (true) { //исполнение
        int comand;
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
                    cout << regs[r];
                } else if (arg == 103) {
                    double x;
                    vector<unsigned int> s(2);
                    s[0] = regs[r];
                    s[1] = regs[r + 1];
                    x = int_to_dbl(s);
                    cout << x;
                } else if (arg == 104) {
                    unsigned char x;
                    cin >> x;
                    regs[r] = (int) x;
                } else if (arg == 105) {
                    char x = (char) regs[r];
                    cout << x;
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
    fclose(fp);
    return 0;
}
