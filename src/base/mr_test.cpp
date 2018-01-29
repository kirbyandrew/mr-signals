#include "mr_test.h"

void Rectangle::set_values(int w,int h)
{
	width = w;
	height = h;
}

int Rectangle::area(void)
{
	return(width*height);
}
