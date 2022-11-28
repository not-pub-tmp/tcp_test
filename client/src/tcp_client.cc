#include <getopt.h>
#include <algorithm>
#include <iostream>

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

    cout << "server =" << options.server << " port=" << options.port << endl;

    unique_ptr<Socket> clientSock = TcpV4SocketFactory::getFactory().createSock();

    clientSock->connectTo(options.server, options.port);

    string sendStr = "Hello client sent from and expect echo";

    clientSock->sendData((void*)sendStr.c_str(), sendStr.size());

    cout << "Client sent finished" << endl;

    auto len = clientSock->receiveData(recv_buf.data(), recv_buf.size());

    cout << "Got back from server = " << recv_buf.data() << endl;

	} catch (const SocketException& e) {
			cout << e.what() << endl;
	}
}

