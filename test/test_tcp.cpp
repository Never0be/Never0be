#include <iostream>
#include <cstring>

#include "servclass.h"
#include "SocketClient.h"

int main(int argc, char* argv[]){
    Serv_Socket Serv;
    Client_Prot Client;
    string TestString = "123 56 89 -6 3.3 dfd 66dd df d2";
    if (argc == 3){
        Serv.Port(atoi(argv[1]));
        Client.SetPort(atoi(argv[1]));
        TestString = (argv[2]);
    }
    //init Server
    Serv.SetType(Serv_Socket::CONNECTION_TYPE::TCP);
    int newMess = 0;
    if(Serv.Open() < 0)
    {
        exit(1);
    }

    //init Client
    //Wait for Events
    if(Serv.Polling(1) < 0 ){
        exit(2);
    }
    Client.SetType(Client_Prot::CONNECTION_TYPE::TCP);
    if (Client.Open()){
        cout << "Client Connect to " << to_string(Client.ServPort) << endl;
    }
    else {
        PRINT("Cant connect to server");
        PRINT("Exit");
        return 0;
    }
    //Wait for Events
    if(Serv.Polling(1) < 0 ){
        exit(2);
    }
    //Recieve data
    newMess = Serv.Process_Recieve_Info();

    if (newMess < 0){
        exit(3);
    }
    //Make server response

    if (newMess){
        if (Serv.Process_Response(newMess) < 0){
            exit(4);
        }
    }

    //Update Clients List
    Serv.UpdateClients();

    int bsend = Client.Send(&TestString[0], TestString.length());
    if (bsend == (TestString.length()+HEADER_MEASSAGE_LENGTH))
        cout <<"Send OK. Sended " << bsend << endl;
    else
        cout <<"Send failed. Sended " << bsend << endl;


    //Wait for Events
    if(Serv.Polling(1) < 0 ){
        exit(2);
    }
    //Recieve data
    newMess = Serv.Process_Recieve_Info();

    if (newMess < 0){
        exit(3);
    }

    //Make server response

    if (newMess){
        if (Serv.Process_Response(newMess) < 0){
            exit(4);
        }
    }
    Serv.CleanHistory();

    char *rec;
    rec = new char[TestString.length()+HEADER_MEASSAGE_LENGTH];
    PRINT("Wait for answer");
    recv(Client.Handle, rec, TestString.length()+HEADER_MEASSAGE_LENGTH, 0);
    cout << string(&rec[HEADER_MEASSAGE_LENGTH]) << endl;
    string recStr = &rec[HEADER_MEASSAGE_LENGTH];
    if (recStr == TestString)
        PRINT("Echo test  PASSED");
    else
        PRINT("Echo test FAILED");


    PRINT("Server is halted");
    exit(0);
}
