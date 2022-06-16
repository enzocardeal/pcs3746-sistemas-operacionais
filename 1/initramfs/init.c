#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static inline void dsb_sev(void)
   {
            __asm__ __volatile__ (
            "dsb\n"
            "sev\n"
        );
    }

static inline void arch_spin_lock(int lock)
{
	unsigned long tmp;
	__asm__ __volatile__(
    "1:	ldrex	%0, [%1]\n"
    "	teq	%0, #0\n"
    "wfene\n"
    "	strexeq	%0, %2, [%1]\n"
    "	teqeq	%0, #0\n"
    "	bne	1b"
 	: "=&r" (tmp)
	: "r" (lock), "r" (1)
	: "cc");
	__asm__ __volatile__("": : :"memory");
}

static inline void arch_spin_unlock(int lock)
{
    __asm__ __volatile__("": : :"memory");
	__asm__ __volatile__(
    "	str	%1, [%0]\n"
	:
	: "r" (lock), "r" (0)
	: "cc");

    dsb_sev();
}


void panic(const char *msg)
{
	fprintf(stderr, "%s: %s (errno = %d)\n", msg, strerror(errno), errno);
	exit(-1);
}

int main()
{

	arch_spin_lock(1);
	sleep(5);
	arch_spin_unlock(1);
	while(1){
		printf("running");
		sleep(5);
	}
	return 0;
}
