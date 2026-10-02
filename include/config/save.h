#ifndef GUARD_CONFIG_SAVE_H
#define GUARD_CONFIG_SAVE_H

// Menu configs
#define SKIP_SAVE_CONFIRMATION              FALSE   // If TRUE, skips the "There is already a saved file" confirmation when overwriting a save.

// SaveBlock1 configs
#define FREE_EXTRA_SEEN_FLAGS_SAVEBLOCK1    FALSE   // Free up unused Pokédex seen flags (52 bytes).
#define FREE_TRAINER_HILL                   FALSE   // Frees up Trainer Hill data (28 bytes).
#define FREE_TRAINER_TOWER                  FALSE   // Frees up Trainer Tower data (x bytes).
#define FREE_MYSTERY_EVENT_BUFFERS          FALSE   // Frees up ramScript (1104 bytes).
#define FREE_MATCH_CALL                     FALSE   // Frees up match call and rematch / VS Seeker data. (104 bytes).
#define FREE_UNION_ROOM_CHAT                FALSE   // Frees up union room chat (212 bytes).
#define FREE_ENIGMA_BERRY                   FALSE   // Frees up E-Reader Enigma Berry data (52 bytes).
#define FREE_LINK_BATTLE_RECORDS            FALSE   // Frees up link battle record data (88 bytes).
#define FREE_MYSTERY_GIFT                   FALSE   // Frees up Mystery Gift data (876 bytes).
#define FREE_POKEBLOCKS                     FALSE   // Frees up PokéBlock data (280 bytes).
#define FREE_BERRY_TREES                    FALSE   // Frees up berry tree data (1536 bytes).
#define FREE_SECRET_BASES                   FALSE   // Frees up secret base data (3200 bytes).
#define FREE_TV_SHOWS                       FALSE   // Frees up tv show data (900 bytes).
                                            // SaveBlock1 total: 8432 bytes
// SaveBlock2 configs
#define FREE_BATTLE_TOWER_E_READER          FALSE   // Frees up Battle Tower E-Reader data (188 bytes).
#define FREE_POKEMON_JUMP                   FALSE   // Frees up Pokémon Jump data (16 bytes).
#define FREE_RECORD_MIXING_HALL_RECORDS     FALSE   // Frees up hall records for record mixing (1032 bytes).
#define FREE_EXTRA_SEEN_FLAGS_SAVEBLOCK2    FALSE   // Free up unused Pokédex seen flags (108 bytes).
#define FREE_BATTLE_FRONTIER                FALSE   // Free up unused Battle Frontier data (2240 bytes).
                                            // SaveBlock2 total: 3584 bytes

                                            // Grand Total: 12016

#endif // GUARD_CONFIG_SAVE_H
