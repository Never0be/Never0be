#ifndef SERVCLASS_H
#define SERVCLASS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <algorithm>
#include <set>
#include <vector>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>

#include "StringParse.h"
using namespace std;

#define PRINT(s) cout << (s) << endl

#define MAX_LENGTH_MESSAGE 65537
#define HEADER_MEASSAGE_LENGTH 8

class Message_History{

public:
    Message_History(){
        Mess = NULL;
        Length = 0;
        Sourse = -1;
        TimeStamp = time(0);
        Done = false;
    }
    ~Message_History(){
        if(Mess)
            delete Mess;
    }
    Message_History(const Message_History &other){
        if (other.Length){
            Copy(other.Mess, other.Length);
        }
        else{
            Mess = NULL;
            Length = 0;
        }
        Sourse = other.Sourse;
        TimeStamp = other.TimeStamp;
        Done = other.Done;
    }
    char *Mess;
    int Length;
    int Sourse;
    time_t TimeStamp;
    bool Done;
    void Copy(char *src, int len){
        if (!Length){
            delete Mess;
        }
        Mess = new char[len];
        Length = len;
        memcpy(Mess, src, len);
    }
};

set<int> clients;

class Serv_Socket{
private:
    bool Active;
    int handle;
    int port;
    char rec_buf[MAX_LENGTH_MESSAGE];
public:
    enum CONNECTION_TYPE {TCP = 0, UDP = 1};
    //vars
    CONNECTION_TYPE Type;
    vector<Message_History> Messages;

    fd_set ConnectList;
    timeval timeout;

    //funs
    Serv_Socket(){
        Active = false;
        port = 1235;
        Messages.reserve(1024);
        Type = TCP;
    }
    Serv_Socket(int NewPort){
        Active = false;
        port = NewPort;
        Messages.reserve(1024);
        Type = TCP;
    }
    ~Serv_Socket(){

        if(handle)
            Close();
    }
    void ParseInp(char* str){
        PRINT("Parse inputarguments");
        if ((strcmp(str, "UDP")==0)||(strcmp(str, "-UDP")==0)){
            Type = UDP;
            cout << "Select Server UDP" <<endl;
        }
        else if ((strcmp(str,"TCP")==0)||(strcmp(str,"-TCP")==0)){
            Type = TCP;
            cout << "Select Server TCP" << endl;
        }
        else if (strstr(str,"port=")!=0){
            string tmp = string(strstr(str,"port=")+5);
            port = stoi(tmp);
            cout << "Set Sever connection port to " << to_string(port) << endl;
        }
        return;
    }
    int GetHandle(){return handle;}
    void SetType (CONNECTION_TYPE type){if (!Active) this->Type = type;}
    void Port(int NewPort){if(!Active) port = NewPort;}
    int Port(void){return port;}
    int Open(){
        return(this->Open(port));
    }
    int Open(int iPort){
        this->port = iPort;
        Messages.clear();
        memset(this->rec_buf,0, MAX_LENGTH_MESSAGE);
        //socket open
        if (Type == TCP)
            handle = socket(AF_INET, SOCK_STREAM, 0);
        else
            handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(handle < 0)
        {
            Active = false;
            perror("Socket open error");
            return(-1);
        }
        //set parameters
        struct sockaddr_in addr;

        fcntl(handle, F_SETFL, O_NONBLOCK);

        addr.sin_family = AF_INET;
        addr.sin_port = htons(this->port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(handle, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("Secket bind error");
            return(-1);
        }
        cout << "Server: Port " << port <<" succesfuly opened" << endl;
        Active = true;
        listen(handle, 16);

        clients.clear();
        // Заполняем множество сокетов
        FD_ZERO(&ConnectList);
        FD_SET(handle, &ConnectList);

        return handle;
    }
    void Close(void){
        Messages.clear();
        cout << "Server: port "<< port << " closed" << endl;
        close(this->handle);
        Active = false;
    }
    int EchoMessage(Message_History &data){
        char *buf;
        int len = data.Length+HEADER_MEASSAGE_LENGTH;
        buf = new char[len];
        memset(buf, '#', 4);
        memcpy(&buf[4], &len, sizeof(int));
        memcpy(&buf[HEADER_MEASSAGE_LENGTH], data.Mess, data.Length);
        return SendAll(data.Sourse, buf, len, 0);

    }
    int SendAll(int s, char *buf, int len, int flags)
    {
        int total = 0;
        int n;

        while(total < len)
        {
            n = send(s, buf+total, len-total, flags);
            if(n == -1) { break; }
            total += n;
        }
        if (total != len){
            PRINT("Server: send failed");
            return -1;
        }
        cout << "Server: sended "<< total << " bytes" << endl;
        return (n==-1 ? -1 : total);
    }
    int UpdateClients(void){
        for(set<int>::iterator it = clients.begin(); it != clients.end(); it++)
            FD_SET(*it, &ConnectList);
        return clients.size();
        return 0;
    }
    int Polling(int TimeOut_sec){
        timeout.tv_sec = TimeOut_sec;
        timeout.tv_usec = 0;
        // Wait for Events
        int mx =  max(handle, *max_element(clients.begin(), clients.end()));
        int sel = select(mx+1, &ConnectList, NULL, NULL, &timeout);
        if( sel < 0)
        {
            //    perror("Server select error");
            return(-1);
        }

        return sel;

    }
    int Process_Recieve_Info(void){
        int CountNewMess=0;
        // Определяем тип события и выполняем соответствующие действия
        //if(FD_ISSET(handle, &ConnectList))
        //{

        // Поступил новый запрос на соединение, используем accept
        int sock = accept(handle, NULL, NULL);
        if(sock >0) {
            fcntl(sock, F_SETFL, O_NONBLOCK, 1);

            clients.insert(sock);

            cout << "Server: New client "<<sock << ". (Total " << clients.size() << ")" << endl;
        }


        for(set<int>::iterator it = clients.begin(); it != clients.end(); it++)
        {
            if(FD_ISSET(*it, &ConnectList))
            {
                // Recieve
                int bytes_read = recv(*it, rec_buf, MAX_LENGTH_MESSAGE, 0);
                if(bytes_read <= 0)
                {
                    PRINT("Client disconnected");
                    close(*it);
                    clients.erase(*it);
                    continue;
                }
                cout << "Server: Client " << (*it) << " send message length " << bytes_read << endl;
                // Put in Data Stack
                Message_History newMess;
                newMess.Copy(&rec_buf[HEADER_MEASSAGE_LENGTH], bytes_read - HEADER_MEASSAGE_LENGTH);
                newMess.Sourse = *it;
                newMess.TimeStamp = time(0);
                Messages.push_back(newMess);

                CountNewMess++;
            }
        }
        return CountNewMess;
    }
    int Process_Response(void){
        return Process_Response(Messages.size());
    }

    int Process_Response(int NumOfLastMessages){
        int BegIndex = Messages.size()-NumOfLastMessages;
        int EndIndex = Messages.size();
        for (int i=BegIndex; i<EndIndex; i++){
            if(Messages[i].Done)
                continue;
            // Echo
            if (EchoMessage(Messages[i])<0){
                return(-i-1);
            }
            //Some others calcs
            if (PostCalcs(Messages[i].Mess, Messages[i].Length) < 0)
                return(-i-1);
            Messages[i].Done = true;
        }
        return 0;
    }

    int PostCalcs(char *Data, int len){
        vector<float> AllNumbers;
        vector <char> AllDigits;
        try{
            AllDigits = FindDigits(Data, len, 0, 9);
        }
        catch(...){
            PRINT("Server Postcalc Error at converting string to digits");
            return -1;
        }
        try{
            AllNumbers = FindNumbers(Data, len);
        }
        catch(...){
            PRINT("Server Postcalc Error at converting string to numbers");
            return -1;
        }

        //sort
        sort(AllDigits.begin(), AllDigits.end());
        sort(AllNumbers.begin(), AllNumbers.end());
        //Sum of all digits 0-9;
        int TotalSum = std::accumulate(AllDigits.cbegin(), AllDigits.cend(),0);
        cout << "Message content " << (int)AllNumbers.size()<< " digits 0-9, total sum = " << TotalSum << endl;

        //All Numbers in decent order
        PRINT("All Digits in decent order");
        for (int i=AllDigits.size()-1; i>=0; i--){
            cout << to_string(AllDigits[i]) << ";\t";
        }
        cout << endl;
        cout << fixed << setprecision(3);
        PRINT("All Numbers in decent order");
        for (int i=AllNumbers.size()-1; i>=0; i--){
            cout << AllNumbers[i] << ";\t";
        }
        cout << endl;
        //min and max val
        if (AllDigits.size())
            cout << "Maximum digit = "<< to_string(*(AllDigits.end()-1))<<"; Minimum digit = " << to_string(AllDigits[0])<<endl;
        if (AllNumbers.size())
            cout << "Maximum number = "<< (*(AllNumbers.end()-1))<<"; Minimum number = " << AllNumbers[0]<<endl;
        return 0;
    }
    void CleanHistory(void){
        for(int i = Messages.size()-1; i>=0; i--){
            if (Messages[i].Done){
                Messages.erase(Messages.begin()+i);
            }
        }
    }
};

#endif // SERVCLASS_H
