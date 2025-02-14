#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>

// 变量结构体
struct Variable {
    std::string name;
    bool isNumber;
    union {
        int num;
        std::string str;
    };

    Variable(const std::string& n, bool numType, int val = 0) : name(n), isNumber(numType) {
        if (isNumber) {
            num = val;
        } else {
            new (&str) std::string();
        }
    }

    ~Variable() {
        if (!isNumber) {
            // 这里不需要手动调用析构函数，让 std::string 自动析构
            // str.~string(); 
        }
    }

    // 移动构造函数
    Variable(Variable&& other) noexcept : name(std::move(other.name)), isNumber(other.isNumber) {
        if (isNumber) {
            num = other.num;
        } else {
            new (&str) std::string(std::move(other.str));
        }
    }

    // 移动赋值运算符
    Variable& operator=(Variable&& other) noexcept {
        if (this != &other) {
            if (!isNumber) {
                // 手动调用析构函数的代码删除
                // str.~string(); 
            }
            name = std::move(other.name);
            isNumber = other.isNumber;
            if (isNumber) {
                num = other.num;
            } else {
                new (&str) std::string(std::move(other.str));
            }
        }
        return *this;
    }

    // 禁止拷贝构造函数和拷贝赋值运算符
    Variable(const Variable&) = delete;
    Variable& operator=(const Variable&) = delete;
};

// 查找变量
int findVariable(const std::vector<Variable>& vars, const std::string& name) {
    for (size_t i = 0; i < vars.size(); ++i) {
        if (vars[i].name == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// 计算表达式
int computeExpression(const std::string& expr, const std::vector<Variable>& vars) {
    std::istringstream iss(expr);
    int result = 0;
    char op = '+';
    std::string token;
    while (iss >> token) {
        int value;
        int varIndex = findVariable(vars, token);
        if (varIndex != -1 && vars[varIndex].isNumber) {
            value = vars[varIndex].num;
        } else {
            try {
                value = std::stoi(token);
            } catch (const std::invalid_argument&) {
                std::cerr << "Error: Invalid number in expression: " << token << std::endl;
                return 0;
            }
        }
        if (op == '+') {
            result += value;
        } else if (op == '-') {
            result -= value;
        }
        iss >> op;
    }
    return result;
}

// 执行命令
void executeCommand(const std::string& command, std::vector<Variable>& vars) {
    if (command.find("output=") == 0) {
        std::string varName = command.substr(7);
        int index = findVariable(vars, varName);
        if (index != -1) {
            if (vars[index].isNumber) {
                std::cout << vars[index].num << std::endl;
            } else {
                std::cout << vars[index].str << std::endl;
            }
        } else {
            std::cerr << "Error: Variable " << varName << " not found." << std::endl;
        }
    } else if (command.find("define(") == 0) {
        size_t pos1 = command.find('(');
        size_t pos2 = command.find(')');
        std::string varName = command.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string type = command.substr(pos2 + 2);
        if (findVariable(vars, varName) == -1) {
            if (type == "number") {
                vars.emplace_back(varName, true);
            } else if (type == "string") {
                vars.emplace_back(varName, false);
            } else {
                std::cerr << "Error: Invalid variable type: " << type << std::endl;
            }
        } else {
            std::cerr << "Error: Variable " << varName << " already defined." << std::endl;
        }
    } else if (command.find("comp(") == 0) {
        size_t pos1 = command.find('(');
        size_t pos2 = command.find(')');
        std::string expr = command.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string newVarName = command.substr(pos2 + 2);
        int result = computeExpression(expr, vars);
        int index = findVariable(vars, newVarName);
        if (index == -1) {
            vars.emplace_back(newVarName, true, result);
        } else {
            vars[index].isNumber = true;
            vars[index].num = result;
        }
    } else if (command.find("output=comp(") == 0) {
        size_t pos1 = command.find('(');
        size_t pos2 = command.find(')');
        std::string expr = command.substr(pos1 + 1, pos2 - pos1 - 1);
        int result = computeExpression(expr, vars);
        std::cout << result << std::endl;
    } else if (command.find("input(") == 0) {
        size_t pos1 = command.find('(');
        size_t pos2 = command.find(')');
        std::string varName = command.substr(pos1 + 1, pos2 - pos1 - 1);
        int index = findVariable(vars, varName);
        if (index != -1) {
            if (vars[index].isNumber) {
                std::cin >> vars[index].num;
            } else {
                std::cin >> vars[index].str;
            }
        } else {
            std::cerr << "Error: Variable " << varName << " not found." << std::endl;
        }
    } else if (command.find("import ") == 0) {
        std::string libName = command.substr(7);
        std::cout << "Importing library " << libName << " (Not fully implemented)." << std::endl;
    } else if (command.find("V(") == 0) {
        size_t pos1 = command.find('(');
        size_t pos2 = command.find(')');
        std::string varName = command.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string content = command.substr(pos2 + 2);
        int index = findVariable(vars, varName);
        try {
            int numVal = std::stoi(content);
            if (index == -1) {
                vars.emplace_back(varName, true, numVal);
            } else {
                vars[index].isNumber = true;
                vars[index].num = numVal;
            }
        } catch (const std::invalid_argument&) {
            if (index == -1) {
                vars.emplace_back(varName, false);
                vars.back().str = content;
            } else {
                vars[index].isNumber = false;
                vars[index].str = content;
            }
        }
    } else {
        std::cerr << "Error: Invalid command: " << command << std::endl;
    }
}

// 主函数
int main() {
    std::string filePath;
    std::cout << "请输入 .u 文件的地址: ";
    std::cin >> filePath;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: 无法打开文件 " << filePath << std::endl;
        return 1;
    }

    std::vector<Variable> variables;
    std::string line;
    while (std::getline(file, line)) {
        executeCommand(line, variables);
    }

    file.close();
    return 0;
}
