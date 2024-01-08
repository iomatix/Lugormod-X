

#ifndef LMD_ACCOUNTS_CORE_H
#define LMD_ACCOUNTS_CORE_H

#include "Lmd_Accounts_Public.h"

#include "gentity_t.h"

typedef struct Account_s Account_t;

#define ACCFLAGS_NOPROFCRLOSS 0x0000001 //dont loose cr when profession changed.  Cleared on prof change and on levelup.
#define ACCFLAGS_NOSECCODE	  0x0000002 // Disable security code for this account.

Account_t *Accounts_GetById(int id);
Account_t *Accounts_GetByUsername(char *str);
Account_t *Accounts_GetByName(char *str);

unsigned int Accounts_Count();
Account_t* Accounts_Get(unsigned int i);

void Accounts_Delete(Account_t *acc);

int Accounts_GetId(Account_t *acc);
#define PlayerAcc_GetId(ent) Accounts_GetId(ent->client->pers.Lmd.account)

char* Accounts_GetUsername(Account_t *acc);
#define PlayerAcc_GetUsername(ent) Accounts_GetUsername(ent->client->pers.Lmd.account)

char* Accounts_GetName(Account_t *acc);
#define PlayerAcc_GetName(ent) Accounts_GetName(ent->client->pers.Lmd.account)

void Accounts_SetName(Account_t *acc, char *name);
#define PlayerAcc_SetName(ent, value) Accounts_SetName(ent->client->pers.Lmd.account, value)



int Accounts_GetPassword(Account_t *acc);

void Accounts_SetPassword(Account_t *acc, char *password);
#define PlayerAcc_SetPassword(ent, password) Accounts_SetPassword(ent->client->pers.Lmd.account, password)

//iomatix
void GiveExperience(gentity_t *ent, int cr, char *reason);
void GiveCredits(gentity_t *ent, int cr, char *reason);
void GiveSkillPoints_Bonus(gentity_t* ent, float sp, char* reason);
void GiveLootboxes(gentity_t *ent, int cr, char *reason);
////////////
int Accounts_GetLevel_jedi(Account_t *acc);
#define PlayerAcc_GetLevel_jedi(ent) Accounts_GetLevel_jedi(ent->client->pers.Lmd.account)

void Accounts_SetLevel_jedi(Account_t *acc, int value);
#define PlayerAcc_SetLevel_jedi(ent, value) Accounts_SetLevel_jedi(ent->client->pers.Lmd.account, value)
////
int Accounts_GetLevel_merc(Account_t *acc);
#define PlayerAcc_GetLevel_merc(ent) Accounts_GetLevel_merc(ent->client->pers.Lmd.account)

void Accounts_SetLevel_merc(Account_t *acc, int value);
#define PlayerAcc_SetLevel_merc(ent, value) Accounts_SetLevel_merc(ent->client->pers.Lmd.account, value)


///////
int Accounts_GetLootboxes(Account_t *acc);
#define PlayerAcc_GetLootboxes(ent) Accounts_GetLootboxes(ent->client->pers.Lmd.account)

void Accounts_SetLootboxes(Account_t *acc, int value);
#define PlayerAcc_SetLootboxes(ent, value) Accounts_SetLootboxes(ent->client->pers.Lmd.account, value)
///////////
int Accounts_GetExperience(Account_t *acc);
#define PlayerAcc_GetExperience(ent) Accounts_GetExperience(ent->client->pers.Lmd.account)

void Accounts_SetExperience(Account_t *acc, int value);
#define PlayerAcc_SetExperience(ent, value) Accounts_SetExperience(ent->client->pers.Lmd.account, value)
///////////
int Accounts_GetNewGamePlus_Counter(Account_t *acc);
#define PlayerAcc_GetNewGamePlus_count(ent) Accounts_GetNewGamePlus_Counter(ent->client->pers.Lmd.account)

void Accounts_SetNewGamePlus_count(Account_t* acc, int value);
#define PlayerAcc_SetNewGamePlus_Counter(ent, value) Accounts_SetNewGamePlus_count(ent->client->pers.Lmd.account, value)

float Accounts_GetSkillPoints_Bonus(Account_t* acc);
#define PlayerAcc_GetSkillPoints_Bonus(ent) Accounts_GetSkillPoints_Bonus(ent->client->pers.Lmd.account)

void Accounts_SetSkillPoints_Bonus(Account_t* acc, float value);
#define PlayerAcc_SetSkillPoints_Bonus(ent, value) Accounts_SetSkillPoints_Bonus(ent->client->pers.Lmd.account, value)

////
int Accounts_GetCredits(Account_t *acc);
#define PlayerAcc_GetCredits(ent) Accounts_GetCredits(ent->client->pers.Lmd.account)

void Accounts_SetCredits(Account_t *acc, int value);
#define PlayerAcc_SetCredits(ent, value) Accounts_SetCredits(ent->client->pers.Lmd.account, value)

int Accounts_GetBountyReward(Account_t *acc);
#define PlayerAcc_GetBountyReward(ent) Accounts_GetBountyReward(ent->client->pers.Lmd.account)

void Accounts_SetBountyReward(Account_t *acc, int value);
#define PlayerAcc_SetBountyReward(ent, value) Accounts_SetBountyReward(ent->client->pers.Lmd.account, value)


int Accounts_GetMaxKillstreak(Account_t *acc);
#define PlayerAcc_GetMaxKillstreak(ent) Accounts_GetMaxKillstreak(ent->client->pers.Lmd.account)

void Accounts_SetMaxKillstreak(Account_t *acc, int value);
#define PlayerAcc_SetMaxKillstreak(ent, value) Accounts_SetMaxKillstreak(ent->client->pers.Lmd.account, value)


//
int Accounts_GetScore(Account_t *acc);
#define PlayerAcc_GetScore(ent) Accounts_GetScore(ent->client->pers.Lmd.account)

void Accounts_SetScore(Account_t *acc, int value);
#define PlayerAcc_SetScore(ent, value) Accounts_SetScore(ent->client->pers.Lmd.account, value)

int Accounts_GetTime(Account_t *acc);
#define PlayerAcc_GetTime(ent) Accounts_GetTime(ent->client->pers.Lmd.account)

void Accounts_SetTime(Account_t *acc, int value);
#define PlayerAcc_SetTime(ent, value) Accounts_SetTime(ent->client->pers.Lmd.account, value)

int Accounts_GetFlags(Account_t *acc);
#define PlayerAcc_GetFlags(ent) Accounts_GetFlags(ent->client->pers.Lmd.account)

void Accounts_AddFlags(Account_t *acc, int value);
#define PlayerAcc_AddFlags(ent, value) Accounts_AddFlags(ent->client->pers.Lmd.account, value)

int Accounts_GetLogins(Account_t *acc);
#define PlayerAcc_GetLogins(ent) Accounts_GetLogins(ent->client->pers.Lmd.account)

void Accounts_SetLogins(Account_t *acc, int value);
#define PlayerAcc_SetLogins(ent, value) Accounts_SetLogins(ent->client->pers.Lmd.account, value)

void Accounts_GetLastIp(Account_t *acc, IP_t ip);
#define PlayerAcc_GetLastIp(ent, value) Accounts_GetLastIp(ent->client->pers.Lmd.account, ip)

void Accounts_SetLastIp(Account_t *acc, IP_t value);
#define PlayerAcc_SetLastIp(ent, value) Accounts_SetLastIp(ent->client->pers.Lmd.account, value)

int Accounts_GetLastLogin(Account_t *acc);
#define PlayerAcc_GetLastLogin(ent) Accounts_GetLastLogin(ent->client->pers.Lmd.account)

void Accounts_SetLastLogin(Account_t *acc, int value);
#define PlayerAcc_SetLastLogin(ent, value) Accounts_SetLastLogin(ent->client->pers.Lmd.account, value)

char* Accounts_GetSeccode(Account_t *acc);
void Accounts_ClearSeccode(Account_t *acc);
char* Accounts_NewSeccode(Account_t *acc);

void ClientCleanName(const char *in, char *out, int outSize);

#endif