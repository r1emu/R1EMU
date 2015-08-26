/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file random.h
 * @brief Random related functions
 *
 */

/**
 * @brief Return a unique seed for the random number generator
 * @param customData A user data
 * @return a unique seed
 */
uint32_t r1emuSeedRandom(uint32_t customData);

/**
 * @brief Generates random 32 bits value
 */
uint32_t r1emuGenerateRandom(uint32_t *seed);

/**
 * @brief Generates random 64 bits value
 */
uint64_t r1emuGenerateRandom64(uint32_t *seed);

int randR(unsigned int *seed);
