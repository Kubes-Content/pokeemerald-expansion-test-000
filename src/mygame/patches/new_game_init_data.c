#include "mygame/patches/new_game/new_game_init_data.h"

#include "global.h"

#include "event_data.h"
#include "random.h"
#include "constants/flags.h"

#include "mygame/patches/new_game/new_game_init_data_impl.h"

static enum Species GetRandomSpecies()
{
    return RandomUniform(RNG_NONE, 0, NUM_SPECIES - 1);
}

static void GivePlayerRandomMon() {
    struct Pokemon mon;
    CreateRandomMon(&mon, GetRandomSpecies(), 5);

    GiveScriptedMonToPlayer(&mon, PARTY_SIZE);
    FlagSet(FLAG_SYS_POKEMON_GET);
}

void NewGameInitDataPatch_FnEnd()
{
    OnNewGameInitDataPatch_FnEnd();

    GivePlayerRandomMon(); // give the player a pokemon upon starting a new game
}
