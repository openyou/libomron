#include "Omron.h"
Omron::~Omron()
{
    if (device) omron_close(device);
    device = 0;
}
