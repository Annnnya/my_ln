#include <iostream>
#include <cstring>
#include <cstdlib> // for realpath
#include <sys/stat.h>
#include <unistd.h>

void printHelp() {
    std::cout << "Usage: my_ln [--soft|--hard|--help] source dest\n"
                 "Options:\n"
                 "  --soft : create a symbolic link\n"
                 "  --hard : create a hard link\n"
                 "  --help : display this help message\n";
}

bool fileExists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int createLink(const char* source, const char* dest, bool isSymbolic) {
    if (fileExists(dest)) {
        std::cerr << "Error: Destination file already exists.\n";
        return 1;
    }

    char* absoluteSource = realpath(source, nullptr);

    if (!absoluteSource) {
        std::perror("Error getting absolute path");
        free(absoluteSource);
        return 4;
    }

    if (!fileExists(absoluteSource)) {
        std::cout << "Error: Source file does not exist." << std::endl;
    }

    if (isSymbolic) {
        if (symlink(absoluteSource, dest) != 0) {
            std::perror("Error creating symbolic link");
            free(absoluteSource);
            return 2;
        }
    } else {
        if (link(absoluteSource, dest) != 0) {
            std::perror("Error creating hard link");
            free(absoluteSource);
            return 3;
        }
    }

    free(absoluteSource);

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printHelp();
        return 0;
    }
    if (argc != 4) {
        std::cerr << "Error: Invalid number of arguments.\n";
        printHelp();
        return 1;
    }

    const char* source = argv[2];
    const char* dest = argv[3];

    std::cout << "Source: " << source << std::endl;
    std::cout << "Destination: " << dest << std::endl;

    if (strcmp(argv[1], "--soft") == 0) {
        return createLink(source, dest, true);
    } else if (strcmp(argv[1], "--hard") == 0) {
        return createLink(source, dest, false);
    } else if (strcmp(argv[1], "--help") == 0) {
        printHelp();
        return 0;
    } else {
        std::cerr << "Error: Unknown option.\n";
        printHelp();
        return 1;
    }
}
