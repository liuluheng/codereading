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

    op_select("pulse");

    player_init(NULL);

    sleep(1);
    player_play();

    while(1)
        sleep(1);

    return 0;
}
