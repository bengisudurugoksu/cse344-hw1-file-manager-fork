#include <stdio.h>    // write() fonksiyonu için
#include <stdlib.h>   // _exit() için
#include <unistd.h>   // fork(), pid_t için
#include <sys/types.h> // pid_t için
#include <sys/wait.h> // wait() için

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        write(2, "Fork başarısız oldu!\n", 22);
        return 1;
    }

    if (pid == 0) {
        write(1, "Çocuk süreç çalışıyor!\n", 24);
        _exit(0);
    } else {
        write(1, "Ana süreç çalışıyor!\n", 22);
        wait(NULL);
    }

    return 0;
}
