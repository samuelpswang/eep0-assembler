#include <iostream>
#include <string>
#include <map>
#include <regex>
#include <vector>
#include <fstream>
using namespace std;


// Match Tables
string opcode_map(const string& opcode) {
    map<string, string> opcode_map;
    opcode_map["MOV"] = "000";
    opcode_map["ADD"] = "001";
    opcode_map["SUB"] = "010";
    opcode_map["ADC"] = "011";
    opcode_map["LDR"] = "100";
    opcode_map["STR"] = "101";
    opcode_map["JMP"] = "110";
    opcode_map["JNE"] = "110";
    opcode_map["JCS"] = "111";
    opcode_map["JMI"] = "111";
    if (!opcode_map.count(opcode)) throw invalid_argument(""); //add
    else return opcode_map[opcode];
}

string hex_map(char hexchar) {
    map<char, string> hex_map;
    hex_map['0'] = "0000";
    hex_map['1'] = "0001";
    hex_map['2'] = "0010";
    hex_map['3'] = "0011";
    hex_map['4'] = "0100";
    hex_map['5'] = "0101";
    hex_map['6'] = "0110";
    hex_map['7'] = "0111";
    hex_map['8'] = "1000";
    hex_map['9'] = "1001";
    hex_map['A'] = "1010";
    hex_map['B'] = "1011";
    hex_map['C'] = "1100";
    hex_map['D'] = "1101";
    hex_map['E'] = "1110";
    hex_map['F'] = "1111";
    if (!hex_map.count(hexchar)) throw invalid_argument("non-hexadecimal character");
    else return hex_map[hexchar];
}

string bin_map(const string& binstr) {
    map<string, string> bin_map;
    bin_map["0000"] = "0";
    bin_map["0001"] = "1";
    bin_map["0010"] = "2";
    bin_map["0011"] = "3";
    bin_map["0100"] = "4";
    bin_map["0101"] = "5";
    bin_map["0110"] = "6";
    bin_map["0111"] = "7";
    bin_map["1000"] = "8";
    bin_map["1001"] = "9";
    bin_map["1010"] = "A";
    bin_map["1011"] = "B";
    bin_map["1100"] = "C";
    bin_map["1101"] = "D";
    bin_map["1110"] = "E";
    bin_map["1111"] = "F";
    if (!bin_map.count(binstr)) throw invalid_argument("incorrect binary string");
    else return bin_map[binstr];
}


// Utility Functions
string dec_str_to_bin_str_8(const string& decstr) {
    int dec = stoi(decstr);
    string ans = "";
    for (int i = 0; i < 8; i++) {
        if (dec % 2 == 0) ans = "0" + ans;
        else ans = "1" + ans;
        dec = dec / 2;
    }
    return ans;
}

string hex_str_to_bin_str_8(const string& hexstr) {
    string binstr;
    for (int i = 0; i < hexstr.length(); i++) {
        binstr += hex_map(hexstr[i]);
    }
    return binstr;
}

string bin_str_to_hex_str_4(const string& binstr) {
    string parsing = binstr, result = "";
    for (int i = 0; i < 16; i += 4) {
        result += bin_map(parsing.substr(i, 4));
    }
    return result;
}

string flag_parser(const string& flagstr) {
    string result = flagstr;
    regex flag("-[x||d||h]+");
    if (regex_match(flagstr, flag)) return result.erase(0, 1);
    else throw invalid_argument("invalid flag");
}


vector<string> tokenize(const string& line, const string& delim) {
    string parsing = line;
    vector<string> result;
    while (parsing.find(" ") != string::npos) {
        result.push_back(parsing.substr(0, parsing.find(" ")));
        parsing.erase(0, parsing.find(" ")+1);
    }
    result.push_back(parsing);
    return result;
}


// Assembly Instruction Class
class Assembly {
    friend ostream& operator<<(ostream& os, const Assembly& assem) {
        os << assem.op << " " << assem.first;
        if (assem.second != "") os << ", " << assem.second;
        return os;
    }

public:
    Assembly(const string& op, const string& first, const string& second, const string& flag) :
        op(op), first(first), second(second), flag(flag)
    {}

    Assembly(const string& line) {
        string parsing = line;
        vector<string> tokens = tokenize(line, " ");

        // Operation
        this->op = tokens[0];
        
        // First Operand
        regex comma("[0-9A-Z]+,");
        if (regex_match(tokens[1], comma)) tokens[1].erase(2, 1);
        this->first = tokens[1];
        
        // Second Operand & Flag
        if (tokens.size() == 4) {
            this->second = tokens[2];
            try {
                this->flag = flag_parser(tokens[3]);
            }
            catch (...) {
                this->flag = "";
            }
        }
        else if (tokens.size() == 3) {
            try {
                this->flag = flag_parser(tokens[2]);
                this->second = "";
            }
            catch (...) {
                this->second = tokens[2];
                this->flag = "";
            }
        }
        else if (tokens.size() == 2) {
            this->second = "";
            this->flag = "";
        }
        else {
            throw invalid_argument("invalid assembly");
        }
        
        // Kill Additional Second Operand For JUMP Instructions
        if (this->op.substr(0, 1) == "J" && this->second != "") {
            throw invalid_argument("invalid second operand");
        }
    }

    string assemble() {
        if (flag_contains("h")) {
            if (flag_contains("x")) {
                this->flag = this->flag.erase(this->flag.find("x"), 1);
            }
            if (flag_contains("d")) {
                this->flag = this->flag.erase(this->flag.find("d"), 1);
            }
            return "0x" + bin_str_to_hex_str_4(this->assemble_bin());
        }
        else {
            return this->assemble_bin();
        }
    }
            
    string assemble_bin() {
        try {
            string mc = "";
            mc += this->get_op() + (flag_contains("d") ? "|" : "");
            mc += this->get_i() + (flag_contains("d") ? "|" : "");
            if (this->op.substr(0, 1) == "J") {
                mc += this->get_ra(true) + (flag_contains("d") ? "|" : "");
                mc += this->get_rb(true) + (flag_contains("d") ? "|" : "");
                mc += this->get_imm8();
            }
            else {
                mc += this->get_ra() + (flag_contains("d") ? "|" : "");
                regex reg("R[0-3]");
                if (regex_match(this->second, reg)) {
                    mc += this->get_rb() + (flag_contains("d") ? "|" : "");
                    mc += this->get_imm8(true);
                }
                else {
                    mc += this->get_rb(true) + (flag_contains("d") ? "|" : "");
                    mc += this->get_imm8(false);
                }
            }
            return mc;
        }
        catch (...) {
            throw invalid_argument("invalid assemly");
        }
    }

    void set_flag(const string& flag) {
        this->flag = flag;
    }
    
private:
    // Data Members
    string op;
    string first;
    string second;
    string flag;

    // Getter Layer
    string get_op() {
        return opcode_map(this->op);
    }
    
    string get_i() {
        if (this->op == "JMP" || this->op == "JCS") return "0";
        else if (this->op == "JNE" || this->op == "JMI") return "1";
        else {
            if (this->second.substr(0, 1) == "R") return "0";
            else return "1";
        }
    }

    string get_ra(bool x = false) {
        if (x) {
            if (flag_contains("x")) return "XX";
            else return "00";
        }
        else {
            regex reg("R[0-3]");
            if (!regex_match(this->first, reg)) throw invalid_argument("invalid first operand");
            int pos = stoi(this->first.substr(1, 1));
            switch (pos) {
                case 0: return "00";
                case 1: return "01";
                case 2: return "10";
                case 3: return "11";
            }
        }
        throw invalid_argument("invalid first operand");
    }

    string get_rb(bool x = false) {
        if (x) {
            if (flag_contains("x")) return "XX";
            else return "00";
        }
        else {
            regex reg("R[0-3]");
            if (!regex_match(this->first, reg)) throw invalid_argument("invalid first operand");
            int pos = stoi(this->second.substr(1, 1));
            switch (pos) {
                case 0: return "00";
                case 1: return "01";
                case 2: return "10";
                case 3: return "11";
            }
        }
        throw invalid_argument("invalid first operand");
    }

    string get_imm8(bool x = false) {
        if (x) {
            if (flag_contains("x")) return "XXXXXXXX";
            else return "00000000";
        }
        else {
            string operand = (this->op.substr(0, 1) == "J" ? this->first : this->second);
            regex hex("0x[0-9A-F]{2}");
            regex dec("#[0-9]+");
            if (regex_match(operand, hex)) {
                operand.erase(0, 2);
                return hex_str_to_bin_str_8(operand);
            }
            else if (regex_match(operand, dec)) {
                operand.erase(0, 1);
                if (stoi(operand) > 255 || stoi(operand) < 0) throw invalid_argument("invalid second operand");
                else return dec_str_to_bin_str_8(operand);
            }
            else throw invalid_argument("invalid second operand");
        }
    };
    
    bool flag_contains(const string& match) {
        return (this->flag.find(match) != string::npos);
    }
};


// User Interface
int main() {
    // Instructions
    cout << "Type your command in the following format:" << endl;
    cout << "1. Assembly mnemonic commands: ADD R0, R1" << endl;
    cout << "2. File input (reads input.txt & writes output.txt): input -h" << endl;
    cout << "4. Exit: exit" << endl;
    cout << "3. Flags: -x (don't care), -d (divider), -h (hexadecimal)" << endl;
    cout << "** SEE README.MD FOR DETAILS **" << endl;

    // Command Line
    string line;
    while ((cout << "> ") && (getline(cin, line))) {
        if (line == "exit") {
            cout << "Program exited!" << endl;
            return 0;
        }
        else if (line.substr(0, line.find(" ")) == "input") {
            // Parsing Flags
            vector<string> tokens = tokenize(line, " ");
            string flag = "";
            try {
                if (tokens.size() > 1) flag = flag_parser(tokens[1]);
            }
            catch (...) {
                cout << "Input Error: invalid arguments after \"input\" detected" << endl << endl;
                continue;
            }

            // Opening Input File
            ifstream ifs;
            ifs.open("input.txt");
            if (!ifs) {
                cout << "File Not Found Error: input.txt does not exist" << endl << endl;
                continue;
            }

            // Parsing & Writing to Output
            ofstream ofs;
            ofs.open("output.txt");
            string file;
            int count = 0;
            while (getline(ifs, file)) {
                try  {
                    Assembly assem = Assembly(file);
                    assem.set_flag(flag);
                    ofs << count << "\t" << assem.assemble() << "\t" << file << endl;
                }
                catch (...) {
                    ofs << count << "\t" << "INPUT ERROR" << "\t" << file << endl;
                }
                count++;
            }

            // Cleanup
            cout << "Done: outpout.txt written" << endl << endl;
            ifs.close();
            ofs.close();
        }
        else {
            try {
                cout << Assembly(line).assemble() << endl << endl;
            }
            catch (...) {
               cout << "Input Error: input is not valid assembly code" << endl << endl;
            }
        }
    }
    
    // For Verification Purposes Only
    // * Create file called "possible_commands.txt".
    // * Will write line number, binary commande, and assembly code.
    // * Run also if you need a match table.

    // // Data Vectors
    // vector<string> op_arth = {"MOV", "ADD", "SUB", "ADC"};
    // vector<string> op_memy = {"LDR", "STR"};
    // vector<string> op_jump = {"JMP", "JNE", "JCS", "JMI"};
    // vector<string> reg = {"R0", "R1", "R2", "R3"};
    
    // // Arithmetic Operations
    // vector<string> all;
    // string temp;
    // for (int i = 0; i < op_arth.size(); i++) {
    //     for (int j = 0; j < reg.size(); j++) {
    //         for (int k = 0; k < reg.size(); k++) {
    //             temp = op_arth[i] + " " + reg[j] + ", " + reg[k];
    //             all.push_back(temp);
    //         }
    //         for (int k = 0; k < 256; k++) {
    //             temp = op_arth[i] + " " + reg[j] + ", #" + to_string(k);
    //             all.push_back(temp);
    //         }
    //     }
    // }
    // // Memory Operations
    // for (int i = 0; i < op_memy.size(); i++) {
    //     for (int j = 0; j < reg.size(); j++) {
    //         for (int k = 0; k < reg.size(); k++) {
    //             temp = op_memy[i] + " " + reg[j] + ", " + reg[k];
    //             all.push_back(temp);
    //         }
    //         for (int k = 0; k < 256; k++) {
    //             temp = op_memy[i] + " " + reg[j] + ", #" + to_string(k);
    //             all.push_back(temp);
    //         }
    //     }
    // }
    // // Jump Operations
    // for (int i = 0; i < op_jump.size(); i++) {
    //     for (int j = 0; j < 256; j++) {
    //         temp = op_jump[i] + " #" + to_string(j);
    //         all.push_back(temp);
    //     }
    // }
    
    // // Write to file & Assemble
    // ofstream ofs;
    // ofs.open("possible_commands.txt");
    // for (int i = 0; i < all.size(); i++) {
    //     Assembly assem = Assembly(all[i]);
    //     assem.set_flag("xd");
    //     ofs << i << "\t" << assem.assemble() << "\t" << all[i] << endl;
    // }
    // ofs.close();
}
