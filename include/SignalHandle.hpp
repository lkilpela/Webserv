#pragma once

#include <csignal>

extern volatile sig_atomic_t sigintReceived;

void handleSigInt(int sig);
void handleSignals();