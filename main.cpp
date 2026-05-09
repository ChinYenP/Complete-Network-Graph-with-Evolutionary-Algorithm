#include "AI.hpp"
#include "binary_addition.hpp"
#include "trainer.hpp"
#include "global.hpp"
#include <iostream>
#include <atomic>
#include <csignal>
#include <string>
#include <thread>
#include <fstream>

using namespace std;

atomic<bool> keep_running(true);

void signal_handler(int sig_num) {
    cout << "Interrupt signal (" << sig_num << ") received." << endl;
    keep_running = false;
}

// char print_note(const bool normal, const bool start_hold, const bool hold, const bool end_hold) {
//     if (normal) return ('O');
//     if (start_hold) return ('[');
//     if (hold) return ('=');
//     if (end_hold) return (']');
//     return (' ');
// }

int main(void) {
    cout << "Insert a unique number." << endl;
    int file_num;
    cin >> file_num;
    const string file_name = "../result" + to_string(file_num) + ".txt";
    ofstream logfile(file_name);
    Trainer<Binary_Addition> trainer(8, 8, file_name);
    signal(SIGINT, signal_handler);
    while (keep_running) {
        trainer.one_evolution();
    }
    trainer.testing();
    return (EXIT_SUCCESS);
}