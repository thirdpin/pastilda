#include "app.h"
#include "stdio.h"
using namespace Application;

int main()
{
	App *app = new App();
	while(1) {
		app->process();
	}

	return (0);
}
