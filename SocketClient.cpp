//#include <iostream>
#include <iostream>
#include <cstring>

#include "SocketClient.h"
#define PRINT(s) cout << (s) << endl
char buf[MAX_LENGTH_MESSAGE+1]; // message+NULL+header

using namespace std;

int main(int argc, char* argv[])
{
    PRINT("Client Start");
    memset(buf, 0, MAX_LENGTH_MESSAGE+1);
    Client_Prot Client;
    if (argc > 1){
        for (int i=1; i < argc; i++)
            Client.ParseInp(argv[i]);
    }
    else {
        PRINT("No argsuments provided. Default type - TCP client");
    }

    PRINT("Enter your message: ");

    cin.getline(buf, MAX_LENGTH_MESSAGE);
    int len = strlen(buf);
    cout << "Get message length " << to_string(len) << ". Sending it to server" << endl;

    if (Client.Open()){
        cout << "Client Connect to " << to_string(Client.ServPort) << endl;
    }
    else {
        PRINT("Cant connect to server");
        PRINT("Exit");
        return 0;
    }

    int bsend = Client.Send(buf, len);
    if (bsend == (len+HEADER_MEASSAGE_LENGTH))
        cout <<"Send OK. Sended " << bsend << endl;
    else
        cout <<"Send failed. Sended " << bsend << endl;
    char *rec;
    rec = new char[len+HEADER_MEASSAGE_LENGTH];
    PRINT("Wait for answer");
    recv(Client.Handle, rec, len+HEADER_MEASSAGE_LENGTH, 0);
    cout << string(&rec[HEADER_MEASSAGE_LENGTH]) << endl;

    PRINT("Exit");
    return 0;
}
