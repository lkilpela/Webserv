#include "SignalHandle.hpp"

volatile sig_atomic_t sigIntReceived = 0;

void handleSigInt(int sig){
	(void)sig;
	sigIntReceived = 1;
}

void handleSignals(){
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, handleSigInt);
}
