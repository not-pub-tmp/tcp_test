#include <getopt.h>
#include <algorithm>
#include <iostream>
#include <thread>
#include <string.h>
#include <vector>
#include <functional>

#include "tcpsocketfactory.h"
#include "echo.h"

using namespace std;
using namespace TCP_TEST;

const int32_t MaxListenNumber = 5;

struct Options {
	string port = "12334";
};

void usage(const string &prog) {
	cout << prog << "[-p port]" << endl;
	cout << "-p port number. Default: 12334." << endl;
}

Options parseOptions(int32_t argc, char **argv) {
	Options options;
	int32_t para;
	while ((para = getopt(argc, argv, "hp:")) != -1) {
		switch (para) {
		case 'h':
		default:
			usage(argv[0]);
      exit(0);
		case 'p':
			options.port = optarg;
			break;
		}
	}
	return options;
}

void readWrite() {
  ECHO::getInstance().echo();
}

int32_t main(int32_t argc, char **argv) {

	ECHO::getInstance().setMax(MaxListenNumber);

	try{

		Options options = parseOptions(argc, argv);

    uniqueSock serverSock = TcpV4SocketFactory::getFactory().createSock();
    serverSock->bindSocket(options.port);

		thread t(readWrite);

		for(;;) {
			serverSock->listenConnection(MaxListenNumber);
			uniqueSock newSock = serverSock->acceptConnection();
			if(newSock)
				ECHO::getInstance().addSocket(std::move(newSock));
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

 		t.join();

	} catch (const SocketException& e) {
			cout << e.what() << endl;
	} catch (const exception& e) {
			cout << e.what() << endl;
	}
}

