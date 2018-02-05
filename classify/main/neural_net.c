#include "neural_net.h"
#include "neural_net_params.h"
#include "adc_collector.h"

#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))

void neural_net_forward()
{
	// Convolution1
	for(int i=0; i<181; i++)
	{
		for(int j=0; j<12; j++)
		{
			h_0[i][j] = b_0[j];
			for(int x=0; x<20; x++)
			{
				for(int y=0; y<4; y++)
				{
					h_0[i][j] += W_0[x][y][j] * window[i+x][y];
				}
			}
		}
	}

/*
	for(int i=0; i<181; i++)
	{
		for(int j=0; j<12; j++)
		{
			printf("%f, ", h_0[i][j]);
		}
		printf("\n");
	}
*/

	for(int i=0; i<90; i++)
	{
		for(int j=0; j<12; j++)
		{
			h_1[i][j] = max(h_0[2*i][j], h_0[2*i+1][j]);
			printf("%f, ", h_1[i][j]);
		}
		printf("\n");
	}

	// Convolution, Pooling and ReLU 1

	for(int i=0; i<90; i++)
	{
		for(int j=0; j<12; j++)
		{
			float x0 = b_0[j];
			float x1 = b_0[j];

			for(int x=0; x<20; x++)
			{
				for(int y=0; y<4; y++)
				{
					x0 += W_0[x][y][j] * (float) window[2*i+x][y];
					x1 += W_0[x][y][j] * (float) window[2*i+x+1][y];
				}
			}

			h_1[i][j] = max(max(x0, x1), 0.0);
		}
	}


	// Convolution, Pooling and ReLU 2

	for(int i=0; i<41; i++)
	{
		for(int j=0; j<15; j++)
		{
			float x0 = b_1[j];
			float x1 = b_1[j];

			for(int x=0; x<8; x++)
			{
				for(int y=0; y<12; y++)
				{
					x0 += W_1[x][y][j] * h_1[2*i+x][y];
					x1 += W_1[x][y][j] * h_1[2*i+x+1][y];
				}
			}

			h_3[i][j] = max(max(x0, x1), 0.0);
		}
	}


	// Convolution, Pooling and ReLU 2

	for(int i=0; i<18; i++)
	{
		for(int j=0; j<20; j++)
		{
			float x0 = b_2[j];
			float x1 = b_2[j];

			for(int x=0; x<5; x++)
			{
				for(int y=0; y<15; y++)
				{
					x0 += W_1[x][y][j] * h_3[2*i+x][y];
					x1 += W_1[x][y][j] * h_3[2*i+x+1][y];
				}
			}

			h_5[i][j] = max(max(x0, x1), 0.0);
		}
	}

	// Convolution, Pooling and ReLU 2

	for(int i=0; i<8; i++)
	{
		for(int j=0; j<25; j++)
		{
			float x0 = b_3[j];
			float x1 = b_3[j];

			for(int x=0; x<3; x++)
			{
				for(int y=0; y<20; y++)
				{
					x0 += W_1[x][y][j] * h_5[2*i+x][y];
					x1 += W_1[x][y][j] * h_5[2*i+x+1][y];
				}
			}

			h_7[i][j] = max(max(x0, x1), 0.0);
		}
	}

	// Fully connected
	for(int i=0; i<25; i++)
	{
		fc[i] = b_fc[i];

		for(int x=0; x<8; x++)
		{
			for(int y=0; y<25; y++)
			{
				int idx = 25*x+y;
				fc[i] += W_fc[idx][i] * h_7[x][y];
			}
		}

		fc[i] = max(fc[i], 0.0);
	}


	// Softmax

	for(int i=0; i<4; i++)
	{
		terrain_pdf[i] = b_sm[i];
		for(int j=0; j<25; j++)
		{
			terrain_pdf[i] += W_sm[j][i] * fc[j];
		}
	}

}