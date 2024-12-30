#include "SignalHandle.hpp"

void handleSigInt(int sig){
	sigintReceived = 1;
}

void handleSignals(){
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, handleSigInt);
}