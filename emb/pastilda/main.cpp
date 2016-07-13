#include "app.h"
#include "stdio.h"
using namespace Application;

#ifdef DEBUG
    #define DEBUG_PRINT(x) printf(x)
#else
    #define DEBUG_PRINT(x) do {} while (0)
#endif

extern "C" void initialise_monitor_handles(void);

int main()
{
	//initialise_monitor_handles();
	App *app = new App();
	while(1) {
		app->process();
	}

	return (0);
}
