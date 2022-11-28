#include <getopt.h>
#include <algorithm>
#include <iostream>
#include <thread>
#include <sstream>

#include "tcpsocketfactory.h"


using namespace std;
using namespace TCP_TEST;

struct Options {
  string server = "127.0.0.1";
	string port = "12334";
};

void usage(const string &prog) {
	cout << prog << " [-s server] [-p port]" << endl;
	cout << "-p port number. Default: 12334." << endl;
	cout << "-s server addr. Default: 127.0.0.1 " << endl; 
}

Options parseOptions(int32_t argc, char **argv) {
	Options options;
	int32_t para;
	while ((para = getopt(argc, argv, "hp:s:")) != -1) {
		switch (para) {
		case 'h':
		default:
			usage(argv[0]);
      exit(0);
		case 'p':
			options.port = optarg;
			break;
		case 's':
			options.server = optarg;
			break;
		}
	}

	return options;
}

int32_t main(int32_t argc, char **argv) {
  std::array<uint8_t, 1500> recv_buf;

	try{
		Options options = parseOptions(argc, argv);

    unique_ptr<Socket> clientSock = TcpV4SocketFactory::getFactory().createSock();

    clientSock->connectTo(options.server, options.port);

		stringstream ss;
		ss << "Request from proc:" << getpid() << " expecting echo" << endl;

		for(auto loop = 0; loop < 10; ++loop) {

			std::this_thread::sleep_for(std::chrono::milliseconds(1500));

			clientSock->sendData((void*)(ss.str().c_str()), ss.str().size());

			auto len = clientSock->receiveData(recv_buf.data(), recv_buf.size());
			if(0 < len)
				cout << "Proc:" << getpid() << " received message is:" << recv_buf.data() << endl;
		}

	} catch (const SocketException& e) {
			cout << e.what() << endl;
	} catch (const exception& e) {
			cout << e.what() << endl;
	}
}

