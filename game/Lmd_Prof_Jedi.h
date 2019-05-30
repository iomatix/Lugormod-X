int Jedi_GetAccSide(Account_t *acc);
int Jedi_GetSide(gentity_t *ent);

int Lmd_Prof_Jedi_GetThousandCutsSkill(Account_t *acc);
#define PlayerProf_Jedi_GetThousandCutsSkill(ent) Lmd_Prof_Jedi_GetThousandCutsSkill(ent->client->pers.Lmd.account)

int Lmd_Prof_Jedi_GetRage(Account_t *acc);
#define PlayerProf_Jedi_GetRageSkill(ent) Lmd_Prof_Jedi_GetRage(ent->client->pers.Lmd.account)

int Lmd_Prof_Jedi_GetDrain(Account_t *acc);
#define PlayerProf_Jedi_GetDrainSkill(ent) Lmd_Prof_Jedi_GetDrain(ent->client->pers.Lmd.account)