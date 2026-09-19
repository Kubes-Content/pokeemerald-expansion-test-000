#include "../../../include/mygame/patches/new_game/new_game_init_data.h"
#include "../../../include/global.h"
#include "../../../include/event_data.h"
#include "../../../include/random.h"
#include "../../../include/constants/flags.h"
#include "../../../include/constants/opponents.h"

static int GetRandomSpecies()
{
    return Random() % (NUM_SPECIES - 1);
}

static void GivePlayerRandomMon() {
    struct Pokemon mon;
        CreateRandomMon(&mon, GetRandomSpecies(), 5);
    GiveScriptedMonToPlayer(&mon, PARTY_SIZE);
    FlagSet(FLAG_SYS_POKEMON_GET);
}

void NewGameInitDataPatch_FnEnd()
{
    GivePlayerRandomMon(); // give the player a pokemon upon starting a new game
}
