#include "servclass.h"

#define PRINT(s) cout << (s) << endl

using namespace std;


int main(int argc, char* argv[])
{
    //setlocale(LC_ALL, "Russian");

    Serv_Socket Serv;
    if (argc > 1){
      for (int i=1; i < argc; i++)
        Serv.ParseInp(argv[i]);
    }
    else {
       PRINT("No argsuments provided. Default: UDP, port 1235");
    }

    int newMess = 0;
    if(Serv.Open() < 0)
    {
//        if(!ErrorUserResponse())
            exit(1);
    }

    while(1)
    {

        //Update Clients List
        Serv.UpdateClients();

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
    }
    PRINT("Server is halted");
    exit(0);
}
