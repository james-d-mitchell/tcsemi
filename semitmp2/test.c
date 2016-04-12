#include <unistd.h>

#include <stdio.h>

main()
{	printf("%d\n",sysconf(_SC_CHILD_MAX));
}
