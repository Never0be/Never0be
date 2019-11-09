#include "StringParse.h"
#include <cstring>
#include <sstream>
int Randomize(char *data, int TotalLen){
    int NewLength = rand()%TotalLen;
    if (NewLength <0){
        NewLength = 0;
        data[NewLength] = '\0';
        return NewLength;
    }
    int bond1 = (RAND_MAX/4), bond2 = ((3.0/4.0) * RAND_MAX);
    for (int i=0; i < 100; i++){
        int val = rand()%(NewLength-1);
        if (rand() < bond1)
           data[val] = '1';
        else if (rand() > bond2)
            data[val] = ' ';
    }
    return NewLength;

}
bool Test_FindDigits(char *Buf, int size, string ans){
    vector <char> result;
    result = FindDigits(Buf, size);
    cout << "Digits (" << result.size() << ")" << endl;
    string strRes ="";
    for (unsigned int i=0; i<result.size(); i++){
        strRes+= to_string(result[i]);
        strRes += ";\t";
    }
    cout << strRes << endl;
    if (ans.length()){
        if (ans.compare(strRes)==0)
            return 1;
        else
            return 0;
    }

    return 1;
}
bool Test_FindNumbers(char *Buf, int size, string ans){
    vector <float> result;
    result = FindNumbers(Buf, size);

    cout << "Numbers (" << result.size() << ")" << endl;
    string strRes ="";
    for (unsigned int i=0; i<result.size(); i++){
        strRes += to_string(result[i]);
        strRes += ";\t";
    }
    cout << strRes << endl;
    if (ans.length()){
        if (ans.compare(strRes)==0)
            return 1;
        else
            return 0;
    }
    return 1;
}

int Tests(char *buf, int len, string ans1, string ans2){
    int TestPassed = 0;
    cout << "Input string\n" << buf << endl;

    if (!Test_FindNumbers(buf, len, ans1)){
        cout << "1. Test Find Numbers Failed" << endl;
    }
    else{
        cout << "1. Test Find Numbers OK" << endl;
        TestPassed++;
    }

    if (!Test_FindDigits(buf, len, ans2)){
        cout << "2. Test Find Digits Failed" << endl;
    }
    else{
        cout << "2. Test Find Digits OK" << endl;
        TestPassed++;
    }


    return TestPassed;
}

int main(int argc, char* argv[]){
    int TestPassed = 0;
    cout<<"StartTests:" << endl;
    int Len = 1024;
    char *Buf;
    Buf = new char[Len];
    memset(Buf, 'a', Len);
    int messSize = Randomize(Buf, Len);

    TestPassed += Tests(Buf, messSize, "", "");
    delete Buf;

    string str = "dfdf -12, 3.3, hhd 333 3.2 df df 66 d 6";
    string ans1 = "-12.000000;\t3.300000;\t333.000000;\t3.200000;\t66.000000;\t6.000000;\t";
    string ans2 = "1;\t2;\t3;\t3;\t3;\t3;\t3;\t3;\t2;\t6;\t6;\t6;\t";
    TestPassed += Tests(&str[0], str.length(), ans1, ans2);
    cout << "##################" << endl;
    cout << "Total results: "<< TestPassed << "/4 ends OK" << endl;

    return 0;
}
