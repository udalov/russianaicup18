#include "Runner.h"

#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace model;
using namespace std;

int main(/* int argc, char* argv[] */) {
    pid_t pid = fork();
    if (pid == -1) {
        cerr << "fork() failed" << endl;
        return 1;
    }
    if (pid == 0) {
        execl("/Users/udalov/c/russianaicup18/local-runner/codeball2018", NULL);
        cerr << "execl() failed" << endl;
        return 1;
    }

    // TODO: make this work

    cout << "Local runner pid " << pid << endl;
    int status;
    while (!WIFEXITED(status)) {
        waitpid(pid, &status, 0);
    }
    auto exitCode = WEXITSTATUS(status);
    if (exitCode != 0) {
        cout << "Local runner exited with " << exitCode << endl;
    }
    return 0;

    /*
    if (argc == 4) {
        Runner runner(argv[1], argv[2], argv[3]);
        runner.run();
    } else {
        Runner runner("127.0.0.1", "31001", "0000000000000000");
        runner.run();
    }
    */

    return 0;
}
