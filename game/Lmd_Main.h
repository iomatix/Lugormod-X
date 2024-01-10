#pragma once
void Accounts_SaveAll(qboolean force);
void Accounts_Save(Account_t* acc);
void Accounts_SaveProfessionData(Account_t* acc, int prof);
void Factions_Save(qboolean full);

unsigned int Accounts_Load();
unsigned int Accounts_LoadProfessionData(Account_t* acc, int prof);
unsigned int Factions_Load(void);
