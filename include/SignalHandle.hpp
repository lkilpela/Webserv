#pragma once

#include <csignal>

extern volatile sig_atomic_t sigIntReceived;

void handleSigInt(int sig);
void handleSignals();