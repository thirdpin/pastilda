#include "app.h"
#include "clock.h"

using namespace Application;

int main()
{
	clock_setup();
	systick_init();

	App *app = new App();
	while(1) {
		app->process();
	}

	return (0);
}
