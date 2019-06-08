
#include "Lmd_Force.h"

typedef struct forceData_s {
	unsigned int size;
	void *levels;
}forceData_t;

typedef struct forcePower_s{
	qboolean (*available)(gentity_t *self, const void *data);
	qboolean (*start)(gentity_t *self, const void *data);
	qboolean (*run)(gentity_t *self, const void *data);
	void (*stop)(gentity_t *self, const void *data);
	qboolean hold;
	forceData_t data; //void* levelData[5];
	int hearDistance;
}forcePower_t;

void* Force_GetPlayerForceData(gentity_t *ent, int power);

#define GETFORCEDATA(type_t) type_t *data = (type_t *)vData
#define FORCELEVELDATA(data, type) {sizeof(type), (void **)data}

//iomatix cooldown:
extern void Force_Cooldown(gentity_t *self, int cd_ms, int FP_FORCEPOWER);
extern qboolean isForce_Cooldown(gentity_t *self, int FP_FORCEPOWER);
