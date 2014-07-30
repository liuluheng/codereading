#include "channelmap.h"
#include "utils.h"

void channel_map_init_waveex(int channels, unsigned int mask, channel_position_t *map)
{
	/* http://www.microsoft.com/whdc/device/audio/multichaud.mspx#EMLAC */
	const channel_position_t channel_map_waveex[] = {
		CHANNEL_POSITION_FRONT_LEFT,
		CHANNEL_POSITION_FRONT_RIGHT,
		CHANNEL_POSITION_FRONT_CENTER,
		CHANNEL_POSITION_LFE,
		CHANNEL_POSITION_REAR_LEFT,
		CHANNEL_POSITION_REAR_RIGHT,
		CHANNEL_POSITION_FRONT_LEFT_OF_CENTER,
		CHANNEL_POSITION_FRONT_RIGHT_OF_CENTER,
		CHANNEL_POSITION_REAR_CENTER,
		CHANNEL_POSITION_SIDE_LEFT,
		CHANNEL_POSITION_SIDE_RIGHT,
		CHANNEL_POSITION_TOP_CENTER,
		CHANNEL_POSITION_TOP_FRONT_LEFT,
		CHANNEL_POSITION_TOP_FRONT_CENTER,
		CHANNEL_POSITION_TOP_FRONT_RIGHT,
		CHANNEL_POSITION_TOP_REAR_LEFT,
		CHANNEL_POSITION_TOP_REAR_CENTER,
		CHANNEL_POSITION_TOP_REAR_RIGHT
	};

	if (channels == 1) {
		map[0] = CHANNEL_POSITION_MONO;
	} else if (channels > 1 && channels < N_ELEMENTS(channel_map_waveex)) {
		int i, j = 0;

		if (!mask)
			mask = (1 << channels) - 1;

		for (i = 0; i < N_ELEMENTS(channel_map_waveex); i++) {
			if (mask & (1 << i))
				map[j++] = channel_map_waveex[i];
		}
		if (j != channels)
			map[0] = CHANNEL_POSITION_INVALID;
	} else {
		map[0] = CHANNEL_POSITION_INVALID;
	}
}
