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


// ----------- Functions ------------
/**
 * @brief Return a unique seed for the random number generator.
 * @param customData A user data
 * @return a unique seed
 */
uint32_t
R1EMU_seed_random (
    uint32_t customData
);

/**
 * @brief Generates random 32 bits value.
 */
uint32_t
R1EMU_generate_random (
    uint32_t *seed
);

/**
 * @brief Generates random 64 bits value.
 */
uint64_t
R1EMU_generate_random64 (
    uint32_t *seed
);

int
rand_r (
    unsigned int *seed
);
