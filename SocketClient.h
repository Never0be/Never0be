#ifndef CLIENT_TEST_H
#define CLIENT_TEST_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define MAX_LENGTH_MESSAGE 65537
#define HEADER_MEASSAGE_LENGTH 8

using namespace std;

class Client_Prot{
    unsigned char OutBuf[MAX_LENGTH_MESSAGE+HEADER_MEASSAGE_LENGTH+1];
public:

    enum CONNECTION_TYPE {TCP = 0, UDP = 1};
    CONNECTION_TYPE Type;
    int Handle;
    bool Active;
    int ServPort;
    struct sockaddr_in addr;

    Client_Prot(){
        Handle = -1;
        Active = false;
        Type = TCP;
        SetPort(1235);
        memset(OutBuf, 0, MAX_LENGTH_MESSAGE+HEADER_MEASSAGE_LENGTH+1);

    }
    Client_Prot(int port){
        Handle = -1;
        Active = false;
        Type = TCP;
        SetPort(port);
        memset(OutBuf, 0, MAX_LENGTH_MESSAGE+HEADER_MEASSAGE_LENGTH+1);

    }
    ~Client_Prot(){
        if (Handle>0)
            Close();
    }
    void Close(void){
        memset(OutBuf, 0, MAX_LENGTH_MESSAGE+HEADER_MEASSAGE_LENGTH+1);
        close(this->Handle);
        Active = false;
    }
    bool SetPort(int port){
        if (Active)
            return false;
        ServPort = port;
        addr.sin_port = htons(port);
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        return true;
    }
    void SetType (CONNECTION_TYPE type){if (!Active) this->Type = type;}

    void ParseInp(char* str){
        if ((strcmp(str, "UDP")==0)||(strcmp(str, "-UDP")==0)){
            Type = UDP;
            cout << "Select client UDP" <<endl;
        }
        else if ((strcmp(str,"TCP")==0)||(strcmp(str,"-TCP")==0)){
            Type = TCP;
            cout << "Select client TCP" << endl;
        }
        else if (strstr(str,"port=")!=0){
            string tmp = string(strstr(str,"port=")+5);
            ServPort = stoi(tmp);
            cout << "Set Sever connection port to " << to_string(ServPort) << endl;
        }
        return;
    }

    bool Open(int Port){
        SetPort(Port);
        cout << "Try connect to server port " << to_string(ServPort) << endl;
        if (Type == TCP)
            Handle = socket(AF_INET, SOCK_STREAM, 0);
        else
            Handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(Handle < 0)
        {
            cout <<"Client socket open error " << endl;
            return false;
        }

        if(connect(Handle, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            cout << "Client connect error" << endl;
            return false;
        }
        Active = true;
        return true;

    }
    bool Open(void){
        return this->Open(ServPort);
    }
    //separator
    int Send(char *mes, int len){
        memset(OutBuf,'#',4);
        memcpy(&OutBuf[4], &len, sizeof(int));
        memcpy(&OutBuf[HEADER_MEASSAGE_LENGTH], mes, len);
        return(send(Handle, OutBuf, len+HEADER_MEASSAGE_LENGTH, 0));
    }

};

#endif // CLIENT_TEST_H
