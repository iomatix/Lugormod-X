
#include "g_local.h"
#include "Lmd_Prof_Core.h"

void Spawn_NoProf(gentity_t *ent){
	ent->client->ps.trueNonJedi = qfalse;
	ent->client->ps.trueJedi = qfalse;
	//
	ent->client->pers.Lmd.killstreak = 0;
	//
	ent->client->ps.weapon = WP_SABER;
	ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_SABER);
	trap_SendServerCommand(ent->s.number, "chat \"^3You haven't chosen a profession yet.\"");
	Disp(ent, "^3Use '^2profession^3' command for available professions list.\n^3Use '^2profession <profession_name>^3' command to choose the profession.");

}

profession_t noProf = {
	"None",
	NULL,
	{0, 0, NULL},
	{0, NULL},
	Spawn_NoProf,
};

void Spawn_GodProf(gentity_t *ent){
	int i;
	//klling streak:
	ent->client->pers.Lmd.killstreak = 0; //reset on spawn
	//
	ent->client->ps.weapon = WP_SABER;
	ent->client->ps.stats[STAT_WEAPONS] = (1 << (LAST_USEABLE_WEAPON+1))  - ( 1 << WP_NONE );
	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		ent->client->ps.ammo[i] = ammoData[i].max;
	}
	ent->client->ps.stats[STAT_HOLDABLE_ITEMS] = (1 << (HI_NUM_HOLDABLE)) - 1;
}

profession_t godProf = {
	"God",
	NULL,
	{0, 0, NULL},
	{0, NULL},
	Spawn_GodProf,
};

profession_t botProf = {
	"Bot",
	NULL,
	{0, 0, NULL},
	{0, NULL},
	Spawn_NoProf,
};