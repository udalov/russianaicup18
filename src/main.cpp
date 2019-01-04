#include "Runner.h"

#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using namespace model;
using namespace std;

enum class Opponent {
    EMPTY,
    QSG,
    ME,
    ME_OLD, // out/Old
};

const Opponent OPPONENT = Opponent::ME_OLD;
const bool VIS = false;
const bool DEBUG = true;
const int SEED = 424251;
const int DURATION = 5000;

const bool DISABLE_RANDOM = false;

const string HOME = "/Users/udalov/c/russianaicup18/";

template<typename... Args> pid_t runProcess(const string& path, Args... vararg) {
    auto pid = fork();
    if (pid == -1) {
        cerr << "fork() failed" << endl;
        terminate();
    }

    if (pid != 0) return pid;

    auto args = vector<string> { vararg... };
    const char **cargs = new const char*[args.size() + 2];
    int i = 0;
    cargs[i++] = path.c_str();
    for (auto& arg : args) {
        cargs[i++] = arg.c_str();
    }
    cargs[i++] = NULL;

    execv(path.c_str(), const_cast<char *const *>(cargs));

    cerr << "execv() failed" << endl;
    terminate();
}

void waitFor(pid_t pid) {
    int status;
    waitpid(pid, &status, 0);
    auto exitCode = WEXITSTATUS(status);
    if (exitCode != 0) {
        cout << "Process " << pid << " exited with " << exitCode << endl;
    }
}

template<typename... Args> void runProcessAndWait(const string& path, Args... args) {
    auto pid = runProcess(path, args...);
    waitFor(pid);
}

int main(/* int argc, char* argv[] */) {
    auto resultsFile = HOME + "out/result.txt";
    runProcessAndWait("/bin/rm", resultsFile);

    vector<string> args = {
        "--duration", to_string(DURATION),
        "--results-file", resultsFile,
        "--p1-name", "Me"
    };

    if (!VIS) {
        args.push_back("--noshow");
    }

    if (SEED != -1) {
        args.insert(args.end(), { "--seed", to_string(SEED) });
    }

    if (DISABLE_RANDOM) {
        args.push_back("--disable-random");
    }

    if (OPPONENT == Opponent::EMPTY) {
        args.insert(args.end(), { "--p2", "empty", "--p2-name", "Empty" });
    } else if (OPPONENT == Opponent::QSG) {
        args.insert(args.end(), { "--p2", "helper", "--p2-name", "QuickStartGuy" });
    } else if (OPPONENT == Opponent::ME) {
        args.insert(args.end(), { "--p2", "tcp-31002", "--p2-name", "Me2" });
    } else if (OPPONENT == Opponent::ME_OLD) {
        args.insert(args.end(), { "--p2", "tcp-31002", "--p2-name", "Me Old" });
    }

    auto localRunner = runProcess(HOME + "local-runner/codeball2018", args);
    cout << "Local runner pid " << localRunner << endl;

    auto me2 =
        OPPONENT == Opponent::ME ? runProcess(HOME + "out/Runner", "31002") :
        OPPONENT == Opponent::ME_OLD ? runProcess(HOME + "out/Old", "31002") : -1;

    Runner runner("127.0.0.1", "31001", "0000000000000000");
    runner.run(DEBUG);

    waitFor(localRunner);
    if (me2 != -1) waitFor(me2);

    runProcessAndWait("/bin/cat", resultsFile);

    return 0;
}
