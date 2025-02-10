#include "SignalHandle.hpp"

void handleSigInt(int sig){
	(void)sig;
	sigIntReceived = 1;
}

void handleSignals(){
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, handleSigInt);
}