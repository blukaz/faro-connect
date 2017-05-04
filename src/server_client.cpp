#include <ros/ros.h>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sensor_msgs/Joy.h>

bool start = false;

int i = 0;

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
            myfile.open ("/home/bare/SCANS/scan.xyz");

            boost::asio::streambuf sb;
            boost::system::error_code ec;
            while (boost::asio::read(socket, sb, ec)) {

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

void joyCallback(const sensor_msgs::Joy::ConstPtr& joy){
	ROS_INFO("I heard: [%d]", joy->buttons[1]);	
	if ( joy->buttons[1] == 1 )
		start = true;
}

/*
void spinThread()
{
  ros::spin();
}
*/

int main(int argc, char **argv) {	

	ros::init(argc, argv, "faro_server");

	ros::NodeHandle n;

	ros::Subscriber sub = n.subscribe("joy", 1, joyCallback);	

	while(ros::ok()){
		if ( start ){
			std::cout << "Sending request for scan..." << std::endl;
			ip_adresa = "192.168.111.7";
			port = "25000";
			parametri = "start";
	
			boost::thread_group tg;
			tg.create_thread(client_thread);
			std::cout << "receiving... " << "\n";
			boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
			tg.create_thread(server_thread);
			tg.join_all();
			start = false;
			std::ostringstream os;
			if ( i < 10 )
				os << "/home/bare/SCANS/scan00" << i << ".3d";
			else
				os << "/home/bare/SCANS/scan0" << i << ".3d";
			std::string s = os.str();
			char path[28];
			strcpy(path, s.c_str());
			std::cout << path << std::endl;
			rename("/home/bare/SCANS/scan.xyz", path);
			i++;
		}
		ros::spinOnce();

	}
	
	//ros::spin();    	

	return 0;
}
