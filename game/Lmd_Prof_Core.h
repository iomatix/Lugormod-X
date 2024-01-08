
#include "Lmd_Data.h"
#include "Lmd_Professions.h"

#include "Lmd_Professions_Public.h"

void* Accounts_Prof_GetFieldData(Account_t *acc);

void Accounts_Prof_SetModified(Account_t *acc);
#define PlayerAcc_Prof_SetModified(ent) Accounts_Prof_SetModified(ent->client->pers.Lmd.account);

qboolean IsValidPlayerName(char *name, gentity_t *ent, qboolean isRegister);


void Experience_Level_Up(gentity_t *ent);

int Professions_LevelCost_EXP(int prof, int playerLevel);
int Professions_AvailableSkillPoints(Account_t* acc, int prof, profSkill_t* skill, profSkill_t** parent);