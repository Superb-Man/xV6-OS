#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    struct syscall_stat _st ;

    if(argc == 2) {
        if(history(atoi(argv[1]), &_st) != 0) {
            printf("Invalid syscall number\n") ;

            return 1 ;
        }

        printf("%d\tsyscall %s, #: %d, #time: %d\n" , atoi(argv[1]), _st.syscall_name, _st.count, _st.accum_time) ;
    }
    else if(argc == 1) {
        for(int i = 1 ; i <= 23 ; i++) {
            history(i, &_st) ;
            printf("%d\tsyscall %s, #: %d, #time: %d\n" , i, _st.syscall_name, _st.count, _st.accum_time) ;
        }
    }
    else{
        printf("Invalid System call Command") ;
    }

    return 0 ;
}