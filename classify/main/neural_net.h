
#ifndef __NEURAL_NET_H__
#define __NEURAL_NET_H__

static float h_0[181][12];
static float h_1[90][12];
static float h_2[83][15];
static float h_3[41][15];
static float h_4[37][20];
static float h_5[18][20];
static float h_6[16][25];
static float h_7[8][25];
static float flatten[200];
static float fc[25];

void neural_net_forward();

#endif