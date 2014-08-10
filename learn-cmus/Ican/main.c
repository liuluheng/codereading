#include "player.h"
#include "input.h"
#include "output.h"
#include "debug.h"

#include <unistd.h>

int main(void)
{
    debug_init();
    ip_load_plugins();
    op_load_plugins();

    op_select_any();
    player_init(NULL);

    player_play();

    while(1)
        sleep(1);

    return 0;
}
