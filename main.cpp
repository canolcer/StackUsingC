#include <iostream>
#include <stack>
#include <sstream>
#include <vector>
#include "Read.h"
#include "Write.h"
#include "algorithm"

std::string createDpda1Txt(std::string filename){
    Read* read = new Read();
    std::string dpda1Txt = read->read(filename);
    return dpda1Txt;
}

std::string createDpdaInput1Txt(std::string filename){
    Read* read = new Read();
    std::string dpda_input1Txt = read->read(filename);
    return dpda_input1Txt;
}

std::vector<std::string> split(const std::string& strReal, char splitter){
    std::vector<std::string> result;
    std::stringstream data(strReal);
    std::string value;
    while (getline(data, value, splitter)){
       result.push_back(value);
    }

    return result;
}

std::string leftTrim(const std::string &leftStr)
{
    size_t start = leftStr.find_first_not_of(" \n\r\t\f\v");
    return (start == std::string::npos) ? "" : leftStr.substr(start);
}

std::string rightTrim(const std::string &rightStr)
{
    size_t end = rightStr.find_last_not_of(" \n\r\t\f\v");
    return (end == std::string::npos) ? "" : rightStr.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return rightTrim(leftTrim(s));
}

bool contains(std::vector<std::string> controlVector, std::string& element){
    if (std::count(controlVector.begin(), controlVector.end(), element)){
        return true;
    }
    return false;
}

bool controlStates(const std::string& dpdaTxt){
    std::vector<std::string> statesV = {"e"};
    std::string states = split(split(split(dpdaTxt, '\n').at(0), '=').at(0), ':').at(1);
    for (std::string& state : split(states, ',')){
        statesV.push_back(trim(state));
    }

    for (std::string control : split(dpdaTxt, '\n')){
        if (control.at(0) == 'T'){
            if (!contains(statesV, split(split(control, ':').at(1), ',').at(0)) || !contains(statesV, split(split(control, ':').at(1), ',').at(3))){
                return false;
            }
        }
    }
    return true;
}

bool controlSigns(const std::string& dpdaTxt) {
    std::vector<std::string> forA = {"e"};
    std::vector<std::string> forZ = {"e"};
    for (std::string A: split(dpdaTxt, '\n')) {
        if (A.at(0) == 'A') {
            std::string afterColon = split(A, ':').at(1);
            for (std::string& comma: split(afterColon, ',')) {
                forA.push_back(comma);
            }
        }
    }

    for (std::string Z: split(dpdaTxt, '\n')) {
        if (Z.at(0) == 'Z') {
            std::string afterColon = split(Z, ':').at(1);
            for (std::string& comma: split(afterColon, ',')) {
                forZ.push_back(comma);
            }
        }
    }
    for (std::string control : split(dpdaTxt, '\n')){
        if (control.at(0) == 'T'){
            std::string afterColon = split(control, ':').at(1);
            if (!contains(forA, split(afterColon, ',').at(1)) || !contains(forZ, split(afterColon, ',').at(4))){
                return false;
            }
        }
    }
    return true;
}

std::string findStartStr(std::string filename){
    std::string dpda1Txt = createDpda1Txt(filename);
    std::string newLine = split(dpda1Txt, '\n').at(0); // For newline
    std::string startSide = split(newLine, '>').at(1); // This is step 2 and for parenthesise
    std::string valueSide = split(startSide, ')').at(0);
    std::string value = split(valueSide, '(').at(1);
    return value;
}

std::vector<std::string> findFinalStatement(std::string filename){
    std::vector<std::string> finalStatements;
    std::string dpda1Txt = createDpda1Txt(filename);
    std::string newLine = split(dpda1Txt, '\n').at(0); // For newline
    std::string startSide = split(newLine, '>').at(1); // This is step 2 and for parenthesise
    for (std::string& value : split(startSide, ',')){
        if (trim(value).at(0) == '['){
            finalStatements.push_back(split(split(value, '[').at(1), ']').at(0));
        }
    }
    return finalStatements;
}

bool allIsEmpty(std::stack<std::string>& dpdaStack){
    std::vector<std::string> controlVector;
    while (!dpdaStack.empty()){
        controlVector.push_back(dpdaStack.top());
        dpdaStack.pop();
    }
    for (std::string i : controlVector){
        if (i != "$"){
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    std::stack<std::string> inputStack; // dpda_input
    std::stack<std::string> dpdaStack;


    std::string dpda1Txt = createDpda1Txt(argv[1]); // DPDA
    std::string dpda_input1Txt = createDpdaInput1Txt(argv[2]); //DPDA INPUT

    int fileNewline = -1;

    if (controlStates(dpda1Txt) && controlSigns(dpda1Txt)){
        std::string startStr = findStartStr(argv[1]);
        std::string result;
        while (true){
            if (inputStack.empty() && fileNewline == -1){
                fileNewline ++;
                std::string newline = split(dpda_input1Txt, '\n').at(fileNewline);
                if (newline.empty()){
                    continue;
                }
                for (std::string& comma : split(std::string(newline.rbegin(), newline.rend()), ',')){
                    inputStack.push(comma);
                }
            }
            bool foundTop_orE = false;
            if (inputStack.empty() && fileNewline > -1){
                bool found = false;
                for (std::string &T: split(dpda1Txt, '\n')) {
                    if ((T.at(0) == 'T') && split(split(T, ':').at(1), ',').at(0) == startStr){
                        if (split(split(T, ':').at(1), ',').at(1) == "e"){
                            found = true;
                            if (!dpdaStack.empty() && split(split(T, ':').at(1), ',').at(2) == dpdaStack.top()){
                                result += (startStr + "," + "e," + split(split(T, ':').at(1), ',').at(2) + " => " + split(split(T, ':').at(1), ',').at(3) + "," + split(split(T, ':').at(1), ',').at(4));
                                foundTop_orE = true;
                                dpdaStack.pop();
                                if (split(split(T, ':').at(1), ',').at(4) != "e"){
                                    foundTop_orE = true;
                                    dpdaStack.push(split(split(T, ':').at(1), ',').at(4));
                                }
                                startStr = split(split(T, ':').at(1), ',').at(3);
                                break;
                            }else if (split(split(T, ':').at(1), ',').at(2) == "e"){
                                result += (startStr + "," + "e," + split(split(T, ':').at(1), ',').at(2) + " => " + split(split(T, ':').at(1), ',').at(3) + "," + split(split(T, ':').at(1), ',').at(4));
                                foundTop_orE = true;
                                if (split(split(T, ':').at(1), ',').at(4) != "e"){
                                    dpdaStack.push(split(split(T, ':').at(1), ',').at(4));
                                }
                                startStr = split(split(T, ':').at(1), ',').at(3);
                                break;
                            } else {
                                found = false;
                                break;
                            }
                        }
                    }
                }
                if (found){
                    std::vector<std::string> a;
                    result += " [STACK]:";
                    while (!dpdaStack.empty()){
                        a.push_back(dpdaStack.top());
                        dpdaStack.pop();
                    }
                    for (std::string& i : a){
                        dpdaStack.push(i);
                    }
                    while (!dpdaStack.empty()){
                        if (dpdaStack.size() == 1){
                            result += dpdaStack.top();
                        }else {
                            result += dpdaStack.top() + ",";
                        }
                        dpdaStack.pop();
                    }
                    result += "\n";
                    std::reverse(a.begin(), a.end());
                    for (std::string& i : a){
                        dpdaStack.push(i);
                    }

                    a.clear();

                }
                if (!found){
                    if ((dpdaStack.empty() || allIsEmpty(dpdaStack)) && contains(findFinalStatement(argv[1]), startStr)){
                        result += "ACCEPT\n";
                    } else{
                        result += "REJECT\n";
                    }
                    while (!dpdaStack.empty()){
                        dpdaStack.pop();
                    }
                    fileNewline ++;
                    if (fileNewline == split(dpda_input1Txt, '\n').size()){
                        break;
                    }
                    std::string newline = split(dpda_input1Txt, '\n').at(fileNewline);
                    result += "\n";
                    if (newline.empty()){
                        continue;
                    }
                    for (std::string& comma : split(std::string(newline.rbegin(), newline.rend()), ',')){
                        inputStack.push(comma);
                    }
                    startStr = findStartStr(argv[1]);
                }
            }
            bool enter = false;
            bool existStatement = false;
            for (std::string& T : split(dpda1Txt, '\n')){
                if (inputStack.empty()){
                    break;
                }
                if ((T.at(0) == 'T') && split(split(T, ':').at(1), ',').at(0) == startStr){
                    existStatement = true;
                    if (split(split(T, ':').at(1), ',').at(1) == inputStack.top() && (!dpdaStack.empty() && (split(split(T, ':').at(1), ',').at(2) == dpdaStack.top()) || split(split(T, ':').at(1), ',').at(2) == "e")){
                        result += (startStr + "," + inputStack.top() + "," + split(split(T, ':').at(1), ',').at(2) + " => " + split(split(T, ':').at(1), ',').at(3) + "," + split(split(T, ':').at(1), ',').at(4));
                        if (!dpdaStack.empty() && split(split(T, ':').at(1), ',').at(2) == dpdaStack.top()){
                            enter = true;
                            foundTop_orE = true;
                            dpdaStack.pop();
                            if (split(split(T, ':').at(1), ',').at(4) != "e"){
                                dpdaStack.push(split(split(T, ':').at(1), ',').at(4));
                            }
                            startStr = split(split(T, ':').at(1), ',').at(3);
                            inputStack.pop();
                            break;
                        }
                        if (split(split(T, ':').at(1), ',').at(2) == "e"){
                            enter = true;
                            foundTop_orE = true;
                            if (split(split(T, ':').at(1), ',').at(4) != "e"){
                                dpdaStack.push(split(split(T, ':').at(1), ',').at(4));
                            }
                            startStr = split(split(T, ':').at(1), ',').at(3);
                            inputStack.pop();
                            break;
                        }
                    }
                }
            }
            if (!enter) {
                for (std::string &T: split(dpda1Txt, '\n')) {
                    if (inputStack.empty()){
                        break;
                    }
                    if ((T.at(0) == 'T') && split(split(T, ':').at(1), ',').at(0) == startStr) {
                        existStatement = true;
                        if (split(split(T, ':').at(1), ',').at(1) == "e" && ((!dpdaStack.empty() && split(split(T, ':').at(1), ',').at(2) == dpdaStack.top()) || split(split(T, ':').at(1), ',').at(2) == "e")){
                            result += (startStr + "," + "e," + split(split(T, ':').at(1), ',').at(2) + " => " + split(split(T, ':').at(1), ',').at(3) + "," + split(split(T, ':').at(1), ',').at(4));
                            if (!dpdaStack.empty() && split(split(T, ':').at(1), ',').at(2) == dpdaStack.top()){
                                foundTop_orE = true;
                                dpdaStack.pop();
                                if (split(split(T, ':').at(1), ',').at(4) != "e"){
                                    dpdaStack.push(split(split(T, ':').at(1), ',').at(4));
                                }
                                startStr = split(split(T, ':').at(1), ',').at(3);
                                break;
                            }
                            if (split(split(T, ':').at(1), ',').at(2) == "e"){
                                foundTop_orE = true;
                                if (split(split(T, ':').at(1), ',').at(4) != "e"){
                                    dpdaStack.push(split(split(T, ':').at(1), ',').at(4));
                                }
                                startStr = split(split(T, ':').at(1), ',').at(3);
                                break;
                            }
                        }
                    }
                }
            }
            std::vector<std::string> a;
            if (!existStatement){
                while (!inputStack.empty()){
                    inputStack.pop();
                }
                continue;
            }
            if (!foundTop_orE && fileNewline >= dpda_input1Txt.size()){
                exit(1);
            }
            result += " [STACK]:";
            while (!dpdaStack.empty()){
                a.push_back(dpdaStack.top());
                dpdaStack.pop();
            }
            for (std::string& i : a){
                dpdaStack.push(i);
            }
            while (!dpdaStack.empty()){
                if (dpdaStack.size() == 1){
                    result += dpdaStack.top();
                }else {
                    result += dpdaStack.top() + ",";
                }
                dpdaStack.pop();
            }
            result += "\n";
            std::reverse(a.begin(), a.end());
            for (std::string& i : a){
                dpdaStack.push(i);
            }
            a.clear();
        }
        Write::write(result, argv[3]);
    } else{
        Write::write("Error [1]:DPDA description is invalid!", argv[3]);
    }
    return 0;
}
