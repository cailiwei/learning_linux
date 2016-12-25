#include<stdio.h>
#include<stdlib.h>

#define GLOBAL_LEN		200
struct param_dev {
	int size; /* the param size */
	unsigned char *param; /* store the param */
};

static unsigned char extend_cmd_enable[4] = {0xB9, 0xFF, 0x83, 0x69};
static unsigned char display_setting[16] = {
	0xB2, 0x00, 0x23, 0x62,
	0x62, 0x70, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0x00,
	0x03, 0x03, 0x00, 0x01,
};
static unsigned char wave_cycle_setting[6] = {0xB4, 0x00, 0x1D, 0x5F, 0x0E, 0x06};
static unsigned char gip_setting[27] = {
	0xD5, 0x00, 0x04, 0x03,
	0x00, 0x01, 0x05, 0x1C,
	0x70, 0x01, 0x03, 0x00,
	0x00, 0x40, 0x06, 0x51,
	0x07, 0x00, 0x00, 0x41,
	0x06, 0x50, 0x07, 0x07,
	0x0F, 0x04, 0x00,
};
static unsigned char power_setting[20] = {
	0xB1, 0x01, 0x00, 0x34,
	0x06, 0x00, 0x0F, 0x0F,
	0x2A, 0x32, 0x3F, 0x3F,
	0x07, 0x3A, 0x01, 0xE6,
	0xE6, 0xE6, 0xE6, 0xE6,
};
static unsigned char vcom_setting[3] = {0xB6, 0x56, 0x56};
static unsigned char pannel_setting[2] = {0xCC, 0x02};
static unsigned char gamma_setting[35] = {
	0xE0, 0x00, 0x1D, 0x22,
	0x38, 0x3D, 0x3F, 0x2E,
	0x4A, 0x06, 0x0D, 0x0F,
	0x13, 0x15, 0x13, 0x16,
	0x10, 0x19, 0x00, 0x1D,
	0x22, 0x38, 0x3D, 0x3F,
	0x2E, 0x4A, 0x06, 0x0D,
	0x0F, 0x13, 0x15, 0x13,
	0x16, 0x10, 0x19,
};
static unsigned char mipi_setting[14] = {
	0xBA, 0x00, 0xA0, 0xC6,
	0x00, 0x0A, 0x00, 0x10,
	0x30, 0x6F, 0x02, 0x11,
	0x18, 0x40,
};

static struct param_dev global_param[] ={
	{ sizeof(extend_cmd_enable),  extend_cmd_enable },
	{ sizeof(display_setting),       display_setting},
	{ sizeof(wave_cycle_setting), wave_cycle_setting },
	{ sizeof(gip_setting),              gip_setting },
	{ sizeof(power_setting),          power_setting },
	{ sizeof(vcom_setting),            vcom_setting },
	{ sizeof(pannel_setting),        pannel_setting },
	{ sizeof(gamma_setting),          gamma_setting },
	{ sizeof(mipi_setting),            mipi_setting },
};

static unsigned char *param_put(struct param_dev *param, int cnt)
{
	int i, j, current = 0;
	struct param_dev *parp = NULL;
	unsigned char temp[GLOBAL_LEN] = { 0 };

	parp = param;
	for (i = 0; i < cnt; i++) {
		for (j = 0; j < (parp + i)->size; j++) {
			temp[current++] = (parp + i)->param[j];
		}	
	}
	
	printf("clw: %d\n", current);

	for (i = 0; i < current; i++)
	{
		printf("0x%02x,", temp[i]);
	}
	return temp;
}

static struct param_dev *param_get(struct param_dev *dst, int lenth, unsigned char *src)
{
	int i, j, current = 0;
	struct param_dev *parp;

	parp = dst;
	for (j = 0; j < lenth; j++) {
		for (i = 0; i < dst->size; i++) {
			dst->param[i] = *src++;
		}
		dst++;
	}

	return parp;
}

int main()
{
	int lenth = 0;
	struct param_dev *parp = NULL;
	unsigned char *src;

	printf("First time put param: \n");

	src = param_put( global_param, sizeof(global_param) );

	printf("\n");
	
	parp = param_get( global_param, sizeof(global_param), src);

	printf("Second time put param: \n");

	param_put( parp, sizeof(global_param) );
	
	printf("\n");
}
