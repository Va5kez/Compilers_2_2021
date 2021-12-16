#include "ast.h"
#include <iostream>
#include <sstream>
#include <set>
#include "asm.h"

const char * floatTemps[] = {"$f0",
                            "$f1",
                            "$f2",
                            "$f3",
                            "$f4",
                            "$f5",
                            "$f6",
                            "$f7",
                            "$f8",
                            "$f9",
                            "$f10",
                            "$f11",
                            "$f12",
                            "$f13",
                            "$f14",
                            "$f15",
                            "$f16",
                            "$f17",
                            "$f18",
                            "$f19",
                            "$f20",
                            "$f21",
                            "$f22",
                            "$f23",
                            "$f24",
                            "$f25",
                            "$f26",
                            "$f27",
                            "$f28",
                            "$f29",
                            "$f30",
                            "$f31"
                        };

#define FLOAT_TEMP_COUNT 32
set<string> intTempMap;
set<string> floatTempMap;

extern Asm assemblyFile;

int globalStackPointer = 0;
int countLables = 0;

string getFloatTemp(){
    for (int i = 0; i < FLOAT_TEMP_COUNT; i++)
    {
        if(floatTempMap.find(floatTemps[i]) == floatTempMap.end()){
            floatTempMap.insert(floatTemps[i]);
            return string(floatTemps[i]);
        }
    }
    cout<<"No more float registers!"<<endl;
    return "";
}

string getNewLabel(string temp) {
    stringstream ss;
    ss << temp << countLables;
    countLables++;
    return ss.str();
}

void releaseFloatTemp(string temp){
    floatTempMap.erase(temp);
}

void FloatExpr::genCode(Code &code){
    string floatTemporal = getFloatTemp();
    code.place = floatTemporal;
    stringstream ss;
    ss << "li.s " << floatTemporal << ", " << this->number << endl;
    code.code = ss.str();
}

void SubExpr::genCode(Code &code){
    Code left, right;
    code.place = getFloatTemp();
    stringstream ss;
    this->expr1->genCode(left);
    this->expr2->genCode(right);
    ss << left.code << endl << right.code << endl
    << "sub.s " << code.place << ", " << left.place << ", " << right.place << endl;
    releaseFloatTemp(left.place);
    releaseFloatTemp(right.place);
    code.code = ss.str();
}

void DivExpr::genCode(Code &code){
    Code left, right;
    code.place = getFloatTemp();
    stringstream ss;
    this->expr1->genCode(left);
    this->expr2->genCode(right);
    releaseFloatTemp(left.place);
    releaseFloatTemp(right.place);
    ss << left.code << endl << right.code << endl
    << "div.s " << code.place << ", " << left.place << ", " << right.place << endl;
    code.code = ss.str(); 
}

void IdExpr::genCode(Code &code){
    string floatTemporal = getFloatTemp();
    code.place = floatTemporal;
    code.code = "l.s " + floatTemporal + ", " + this->id + "\n";
}

string ExprStatement::genCode(){
    Code expr;
    this->expr->genCode(expr);
    releaseFloatTemp(expr.place);
    return expr.code;
}

string IfStatement::genCode(){
    string endLabel = getNewLabel("end");
    Code expr;
    this->conditionalExpr->genCode(expr);
    stringstream ss;
    ss << expr.code << endl;
    ss << "bc1f " << endLabel << endl;
    for (auto const& i : this->trueStatement)
    {
        ss << i->genCode() << endl;
    }
    ss << endLabel << " :" << endl;
    
    return ss.str();
}

void MethodInvocationExpr::genCode(Code &code){
    
}

string AssignationStatement::genCode(){

    return "Assignation statement code generation\n";
}

void GteExpr::genCode(Code &code){
    Code left, right;
    stringstream ss;
    this->expr1->genCode(left);
    this->expr2->genCode(right);
    ss << left.code << endl << right.code << endl;
    releaseFloatTemp(left.place);
    releaseFloatTemp(right.place);
    ss << "c.lt.s " << right.place << ", " << left.place << endl;
    code.code = ss.str(); 
}

void LteExpr::genCode(Code &code){
    Code left, right;
    stringstream ss;
    this->expr1->genCode(left);
    this->expr2->genCode(right);
    ss << left.code << endl << right.code << endl;
    releaseFloatTemp(left.place);
    releaseFloatTemp(right.place);
    ss << "c.lt.s " << left.place << ", " << right.place << endl;
    code.code = ss.str(); 
}

void EqExpr::genCode(Code &code){
    Code left, right;
    stringstream ss;
    this->expr1->genCode(left);
    this->expr2->genCode(right);
    ss << left.code << endl << right.code << endl;
    releaseFloatTemp(left.place);
    releaseFloatTemp(right.place);
    ss << "c.eq.s " << left.place << ", " << right.place << endl;
    code.code = ss.str(); 
}

void ReadFloatExpr::genCode(Code &code){
    
}

string PrintStatement::genCode(){
    stringstream ss;
    for (auto const& i : this->expressions)
    {
        Code expr;
        i->genCode(expr);
        releaseFloatTemp(expr.place);
        ss << expr.code << endl;
        ss << "move.s $f12, " << expr.place << endl;
        ss << "li $v0, 2" << endl;
        ss << "syscall" << endl; 
    }
    return ss.str();
}

string ReturnStatement::genCode(){
    Code expr1;
    stringstream ss;
    this->expr->genCode(expr1);
    releaseFloatTemp(expr1.place);
    ss << expr1.code << endl;
    ss << "mfc1 $v0, " << expr1.place << endl;
    return ss.str();
}

string MethodDefinitionStatement::genCode(){
    return "Method definition code generation\n";
}