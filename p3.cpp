#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "main.hpp"
#include "diagnose.hpp"
#include <iostream>

int main(){
    Client c = Client(1, 0);


    while(true){
        int caso;
        cout << "Digite o caso de teste que você quer executar " << endl;
        cin >> caso;

        if(caso == 1){
            c.dont_request_resource();
        }else if(caso == 2){
            c.dont_request_resource();
            c.dont_request_resource();
        }else if(caso == 3){
            c.request_resource();
        }

    }

    return 0;
}