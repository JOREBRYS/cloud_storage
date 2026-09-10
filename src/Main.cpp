#include "Server.h"
#include "FileManager.h"
#include <stdexcept>
#include <iostream>


using namespace std;

int main(){
    try{
        FileManager filemanager("storage");
        ostream& logger = cout; 
        DatabaseAccessor db(logger);
        boost::asio::io_context io;
        const short port = 7007;
        
        Server server(
            io,
            port,
            db,
            logger,
            filemanager
        );
        io.run();
    }
    catch(const std::exception& e){

    }

    return 0;
}