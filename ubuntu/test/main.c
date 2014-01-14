#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

typedef unsigned char ElemType;

typedef struct Node1 {
	ElemType data;
	struct Node1  *next;
} DList;

typedef struct Node2 {
	int data_lenth;
	ElemType cmd;
	DList *next;
} HList;

struct param_dev {
	int dlen; 				/* the param size */
	char *payload; 			/* store the param */
};

static char display_off[2] = {0x28, 0x00};
static char enter_sleep[2] = {0x10, 0x00};

static char sleep_in_cmd1[5]= {
    0xFF, 0xAA, 0x55, 0x25,
    0x01,
};
static char sleep_in_cmd2[8]= {
    0xF3, 0x02, 0x00, 0x00,
    0x45, 0x88, 0xD1, 0x0C,
};
static char sleep_in_cmd3[2]= {
    0xF4, 0x58,
};
static char sleep_in_cmd4[5]= {
    0xF8, 0x08, 0x03, 0x00,
    0x00,
};
static char sleep_in_cmd5[21]= {
    0xFA, 0x80, 0x80, 0x20,
    0x6F, 0x00, 0x0F, 0x00,
    0x00, 0xD0, 0x0F, 0x00,
    0x00, 0x03, 0x21, 0x14,
    0x20, 0xA0, 0x00, 0x00,
    0x00,
};
static char sleep_in_cmd6[24]= {
    0xFC, 0x00, 0x20, 0x25,
    0x0A, 0x17, 0x77, 0x00,
    0x00, 0x11, 0x11, 0x0F,
    0x03, 0x01, 0x55, 0x67,
    0x09, 0x00, 0x00, 0x00,
    0x05, 0x40, 0x00, 0x0F,
};
static char sleep_in_cmd7[6]= {
    0xF0, 0x55, 0xAA, 0x52,
    0x08, 0x00,
};
static char sleep_in_cmd8[4]= {
    0xBB, 0x11, 0x01, 0x11,
};
static char sleep_in_cmd9[6]= {
    0xBD, 0x01, 0xFF, 0x08,
    0x40, 0x03,
};
static char sleep_in_cmd10[6]= {
    0xF0, 0x55, 0xAA, 0x52,
    0x08, 0x01,
};
static char sleep_in_cmd11[2]= {
    0xB3, 0x00,
};
static char sleep_in_cmd12[2]= {
    0xB6, 0x11,
};
static char sleep_in_cmd13[2]= {
    0xB7, 0x11,
};
static char sleep_in_cmd14[2]= {
    0xB8, 0x11,
};
static char sleep_in_cmd15[2]= {
    0xBA, 0x11,
};

static struct param_dev nt35516_display_off_cmds[] = {
    {sizeof(display_off), display_off},
    {sizeof(sleep_in_cmd1), sleep_in_cmd1},
    {sizeof(sleep_in_cmd2), sleep_in_cmd2},
    {sizeof(sleep_in_cmd3), sleep_in_cmd3},
    {sizeof(sleep_in_cmd4), sleep_in_cmd4},
    {sizeof(sleep_in_cmd5), sleep_in_cmd5},
    {sizeof(sleep_in_cmd6), sleep_in_cmd6},
    {sizeof(sleep_in_cmd7), sleep_in_cmd7},
    {sizeof(sleep_in_cmd8), sleep_in_cmd8},
    {sizeof(sleep_in_cmd9), sleep_in_cmd9},
    {sizeof(sleep_in_cmd10), sleep_in_cmd10},
    {sizeof(sleep_in_cmd11), sleep_in_cmd11},
    {sizeof(sleep_in_cmd12), sleep_in_cmd12},
    {sizeof(sleep_in_cmd13), sleep_in_cmd13},
    {sizeof(sleep_in_cmd14), sleep_in_cmd14},
    {sizeof(sleep_in_cmd15), sleep_in_cmd15},
    {sizeof(enter_sleep), enter_sleep},
};

static HList * List [ARRAY_SIZE(nt35516_display_off_cmds)];

/*
 * CreateTable: put the data into the ListTable
 * @param: HList * &Header globalfifo ListTable 
 * @param: unsigned *DataArray
 * @param: int data_lenth
 */
void CreateTable(HList * *Header, int data_lenth, ElemType *DataArray)
{	
#if 0
	int j = 0;
	printf("clw: dlen:%d ",data_lenth);
	while(j < data_lenth)
		printf("0x%02x, ", DataArray[j++]);
	j = 0;
	printf("\n");
#else
	int i = 0;
	DList *DataList = NULL, *temp = NULL;
	*Header = (HList *)malloc(sizeof(HList));
	if (!(*Header)) {
		printf("malloc failed\n");
		return;
	}
	(*Header)->data_lenth = data_lenth;
	(*Header)->cmd = DataArray[0];
	printf("%s-->data_lenth:%02d ",__func__, (*Header)->data_lenth);
	while (i < data_lenth) {
		DataList = (DList *)malloc(sizeof(DList));
		DataList->data = DataArray[i++];
		printf("0x%02x, ", DataList->data);
		if (temp == NULL) {
			temp = DataList;
			(*Header)->next = temp;
		} else {
			temp->next = DataList;
			/* temp must initail be DataList */
			temp = DataList;
		}
	}
	temp->next = NULL;
	printf("\n");
#endif
}

void DispTable(HList *Header)
{
	DList *DataList = NULL;
	if (!Header) {
		printf("clw-->%s: get data error\n", __func__);
		return;
	}
	DataList = Header->next;
	printf("Lenth: %02d, Cmd: 0x%02x Param:", Header->data_lenth, Header->cmd);
	while (DataList) {
		printf("0x%02x ", DataList->data);
		DataList = DataList->next;
	}
	printf("\n");
}

static void testFunc(struct param_dev *cmd, int size)
{
	int i = 0;
	for (i = 0; i < size; i++) {
		CreateTable(&(List[i]), cmd[i].dlen, cmd[i].payload);
#if 0
		printf("clw: dlen:%d ",cmd[i].dlen);
		while(j < cmd[i].dlen)
			printf("0x%02x, ", cmd[i].payload[j++]);
		j = 0;
		printf("\n");
#endif
	}

	for (i = 0; i < size; i++) {
		DispTable(List[i]);
	}
}

void main()
{
	testFunc(nt35516_display_off_cmds, ARRAY_SIZE(nt35516_display_off_cmds));
}
