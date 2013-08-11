#include "sys_functions.h"

#include <stdio.h>
//#include <unistd.h>


// TODO: unfinished
void setloggers(int stdout_fileno, int stderr_fileno)
{
    printf("setloggers: %i %i\n", stdout_fileno, stderr_fileno);

//    FILE* old_stdout = stdout;

    //FILE* f = fdopen(stdout_fileno, "ab");
//    if (!f) printf("setloggers stdout fdopen failed\n");
    //fclose(stdout);
    
//    dup2(stdout_fileno, stdout);

    //stdout = f; // not working
    //freopen();
    // TODO: errno output
    printf("this is a test\n");
}
