#include <ros/ros.h>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <fstream>

struct Client
{
    boost::asio::io_service& io_service;
    boost::asio::ip::tcp::socket socket;

    Client(boost::asio::io_service& svc, std::string const& host, std::string const& port)
        : io_service(svc), socket(io_service)
    {
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(boost::asio::ip::tcp::resolver::query(host, port));
        boost::asio::connect(this->socket, endpoint);
    };

    void send(std::string const& message) {
        socket.send(boost::asio::buffer(message));
    }
};

std::string ip_adresa, port, parametri;

void client_thread() {
    boost::asio::io_service svc;
    Client client(svc, ip_adresa, port);
    //std::string poruka = "start";
    //for(int i = 0; i < 1000000; i++ ){
    client.send(parametri);
    //usleep(20000);
    //}

}

void server_thread() {
    try
    {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::atoi(port.c_str())));

        {
            boost::asio::ip::tcp::socket socket(io_service);
            acceptor.accept(socket);

            std::ofstream myfile;
            myfile.open ("/home/luka/SCANS/scan.xyz");

            boost::asio::streambuf sb;
            boost::system::error_code ec;
            while (boost::asio::read(socket, sb, ec)) {
                std::cout << "receiving... " << "\n";

                myfile << &sb;

                if (ec) {
                    std::cout << "status: " << ec.message() << "\n";
                    break;
                }
            }
            std::cout << "Received: " << "\n";
            myfile.close();

        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void spinThread()
{
  ros::spin();
}

int main(int argc, char **argv) {

	std::cout << "Unesi parametre scan-a (<start> : pokretanje) :" << std::endl;
	std::cin >> parametri;
	std::cout << "Unesi IP adresu: " << std::endl;
	std::cin >> ip_adresa;
	std::cout << "Unesi port <15000:40000>: " << std::endl;
	std::cin >> port;

    boost::thread_group tg;
    tg.create_thread(client_thread);

    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    tg.create_thread(server_thread);

    tg.join_all();
}
