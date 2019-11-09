#ifndef STRINGPARSE_H
#define STRINGPARSE_H
#include <string>
#include <iostream>
#include <limits>
#include <locale>
#include <iostream>
#include <vector>
using namespace std;

vector<char> FindDigits(char *Data, int TotalChars, int MinVal, int MaxVal){
    vector<char> result;
    char LeftLim = '0', RightLim = '9';

    //Define Limits
    if (MinVal > MaxVal){
        int tmp = MaxVal;
        MaxVal = MinVal;
        MinVal = tmp;
    }
    (MinVal<0 ? MinVal=0: MinVal=MinVal);
    (MinVal>9 ? MinVal=9: MinVal=MinVal);
    LeftLim += MinVal;
    (MaxVal<0 ? MaxVal=0: MaxVal=MaxVal);
    (MaxVal>9 ? MaxVal=9: MaxVal=MaxVal);
    RightLim += MaxVal;

    //Count chars
    for(int i=0; i<TotalChars; i++){
        if((Data[i]>=LeftLim)&&(Data[i]<=RightLim)){
           result.push_back(Data[i]-0x30);
        }
    }

    return result;
}

vector<char> FindDigits(char *Data, int len){
    vector <char> result;
    result = FindDigits(Data, len, 0,9);
    return result;
}


int FindBeginOfNumber(char *Data, int TotalLength, int StartPoint){
    int result = -1;
    for(int i=StartPoint; i<TotalLength; i++){
        if ((Data[i]>='0')&&(Data[i]<'9')){
            result = i;
            break;
        }
    }
    if (result > 0){
        if (Data[result-1] == '-')
            result--;
    }
    return result;
}

int FindEndOfNumber(char *Data, int TotalLength, int StartPoint){
    int result = TotalLength;
    char point  = std::use_facet< std::numpunct<char> >(std::cout.getloc()).decimal_point();
    bool HasDecimalPoint = false;
    for(int i=StartPoint; i< TotalLength; i++){
        if ((Data[i]>='0')&&(Data[i]<'9'))
            continue;
        if ((Data[i]==point)&&(!HasDecimalPoint)){
            HasDecimalPoint = true;
            continue;
        }
        result = i;
        break;
    }
    if (Data[result-1] == point){
            result--;
    }
    return result;
}

vector<float> FindNumbers(char *Data, int TotalLength, float MinVal, float MaxVal){
    vector <float> result;
    int IndBeg=0;
    int IndEnd=0;
    do {
      IndBeg = FindBeginOfNumber(Data, TotalLength, IndBeg);
      if (IndBeg<0)
          break;

      IndEnd = FindEndOfNumber(Data, TotalLength, IndBeg+1);
      if (IndBeg==IndEnd){
          IndBeg++;
          continue;
      }
      string str = string(&Data[IndBeg], IndEnd-IndBeg);
      result.push_back(stof(str));
      IndBeg = IndEnd;
    }
    while(IndBeg < TotalLength);
    return result;
}

vector<float> FindNumbers(char *Data, int TotalLength){
    return FindNumbers(Data, TotalLength, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
}
#endif // STRINGPARSE_H
