#pragma once
#include "Lmd_Data.h"
#include "Lmd_Professions.h"

#include "Lmd_Professions_Public.h"



void* Accounts_Prof_GetFieldData(Account_t *acc);
void Accounts_ClearData(Account_t* acc);

void Accounts_Prof_SetModified(Account_t *acc);
#define PlayerAcc_Prof_SetModified(ent) Accounts_Prof_SetModified(ent->client->pers.Lmd.account);

qboolean IsValidPlayerName(char *name, gentity_t *ent, qboolean isRegister);
void Accounts_SaveProfessionData(Account_t* acc, int prof);
unsigned int Accounts_ParseProfessionData(Account_t* acc, int prof);

void Experience_Level_Up(gentity_t *ent);

int Professions_LevelCost_EXP(int prof, int playerLevel);
int Professions_AvailableSkillPoints(Account_t* acc, int prof, profSkill_t* skill, profSkill_t** parent);


bool is_Gameplay_Proffesion(int prof);
int Accounts_Prof_GetProfession(Account_t* acc);
void Accounts_Prof_SetProfession(Account_t* acc, int prof);

int Accounts_Prof_GetLevel(Account_t* acc);
void Accounts_Prof_SetLevel(Account_t* acc, int value);

int Accounts_Prof_GetLastLevelup(Account_t* acc);

int Accounts_Prof_GetExperience(Account_t* acc);
void Accounts_Prof_SetExperience(Account_t* acc, int value);

int Accounts_Prof_GetNewGamePlus_Counter(Account_t* acc);
void Accounts_Prof_SetNewGamePlus_Counter(Account_t* acc, int value);
int Accounts_Prof_GetNewGamePlus_SkillPoints(Account_t* acc, int prof);
