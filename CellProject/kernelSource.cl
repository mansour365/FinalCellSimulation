__kernel void multipleInjection(
	__global float *output2, __global float *output3, __global float *output4, __global float *output5,
	__global float *output6, __global float *output7, __global float *output8, __global float *output9,
	int row1, int col1, int row2, int col2, int row3, int col3, int med1, int med2, int med3)
{
	int medArr[3] = {med1, med2, med3};
	for(int i = 0; i < 3 ; i++)
	{
		int med = medArr[i];

		if(med == 1)
		{
			if(i = 0)
			{
				output2[0] = row1 - 1;
				output2[1] = col1;\
			}
			else if(i = 1)
			{
				output2[2] = row2 - 1;
				output2[3] = col2;
			}
			else if(i = 2)
			{
				output2[4] = row3 - 1;
				output2[5] = col3;
			}
		}

		else if(med == 2)
		{
			if(i = 0)
			{
				output2[0] = row1 - 1;
				output2[1] = col1;
				output3[0] = row1 - 1;
				output3[1] = col1 + 1;
			}
			else if(i = 1)
			{
				output2[2] = row2 - 1;
				output2[3] = col2;
				output3[2] = row2 - 1;
				output3[3] = col2 + 1;
			}
			else if(i = 2)
			{
				output2[4] = row3 - 1;
				output2[5] = col3;
				output3[4] = row3 - 1;
				output3[5] = col3 + 1;
			}
		}

		else if(med == 3)
		{
			if(i = 0)
			{
				output2[0] = row1 - 1;
				output2[1] = col1;
				output3[0] = row1 - 1;
				output3[1] = col1 + 1;
				output4[0] = row1;
				output4[1] = col1 + 1;
			}
			else if(i = 1)
			{
				output2[2] = row2 - 1;
				output2[3] = col2;
				output3[2] = row2 - 1;
				output3[3] = col2 + 1;
				output4[2] = row2;
				output4[3] = col2 + 1;
			}
			else if(i = 2)
			{
				output2[4] = row3 - 1;
				output2[5] = col3;
				output3[4] = row3 - 1;
				output3[5] = col3 + 1;
				output4[4] = row3;
				output4[5] = col3 + 1;
			}
		}

		else if(med == 4)
		{
			if(i = 0)
			{
				output2[0] = row1 - 1;
				output2[1] = col1;
				output3[0] = row1 - 1;
				output3[1] = col1 + 1;
				output4[0] = row1;
				output4[1] = col1 + 1;
				output5[0] = row1 + 1;
				output5[1] = col1 + 1;
			}
			else if(i = 1)
			{
				output2[2] = row2 - 1;
				output2[3] = col2;
				output3[2] = row2 - 1;
				output3[3] = col2 + 1;
				output4[2] = row2;
				output4[3] = col2 + 1;
				output5[2] = row2 + 1;
				output5[3] = col2 + 1;
			}
			else if(i = 2)
			{
				output2[4] = row3 - 1;
				output2[5] = col3;
				output3[4] = row3 - 1;
				output3[5] = col3 + 1;
				output4[4] = row3;
				output4[5] = col3 + 1;
				output5[4] = row3 + 1;
				output5[5] = col3 + 1;
			}
		}

		else if(med == 5)
		{
			if(i = 0)
			{
				output2[0] = row1 - 1;
				output2[1] = col1;
				output3[0] = row1 - 1;
				output3[1] = col1 + 1;
				output4[0] = row1;
				output4[1] = col1 + 1;
				output5[0] = row1 + 1;
				output5[1] = col1 + 1;
				output6[0] = row1 + 1;
				output6[1] = col1;
			}
			else if(i = 1)
			{
				output2[2] = row2 - 1;
				output2[3] = col2;
				output3[2] = row2 - 1;
				output3[3] = col2 + 1;
				output4[2] = row2;
				output4[3] = col2 + 1;
				output5[2] = row2 + 1;
				output5[3] = col2 + 1;
				output6[2] = row2 + 1;
				output6[3] = col2;
			}
			else if(i = 2)
			{
				output2[4] = row3 - 1;
				output2[5] = col3;
				output3[4] = row3 - 1;
				output3[5] = col3 + 1;
				output4[4] = row3;
				output4[5] = col3 + 1;
				output5[4] = row3 + 1;
				output5[5] = col3 + 1;
				output6[4] = row3 + 1;
				output6[5] = col3;
			}
		}

		else if(med == 6)
		{
			output2[0] = row1 - 1;
			output2[1] = col1;
			output3[0] = row1 - 1;
			output3[1] = col1 + 1;
			output4[0] = row1;
			output4[1] = col1 + 1;
			output5[0] = row1 + 1;
			output5[1] = col1 + 1;
			output6[0] = row1 + 1;
			output6[1] = col1;
			output7[0] = row1 + 1;
			output7[1] = col1 - 1;
		}

		else if(med == 7)
		{
			output2[0] = row1 - 1;
			output2[1] = col1;
			output3[0] = row1 - 1;
			output3[1] = col1 + 1;
			output4[0] = row1;
			output4[1] = col1 + 1;
			output5[0] = row1 + 1;
			output5[1] = col1 + 1;
			output6[0] = row1 + 1;
			output6[1] = col1;
			output7[0] = row1 + 1;
			output7[1] = col1 - 1;
			output8[0] = row1;
			output8[1] = col1 - 1;
		}

		else if(med == 8)
		{
			output2[0] = row1 - 1;
			output2[1] = col1;
			output3[0] = row1 - 1;
			output3[1] = col1 + 1;
			output4[0] = row1;
			output4[1] = col1 + 1;
			output5[0] = row1 + 1;
			output5[1] = col1 + 1;
			output6[0] = row1 + 1;
			output6[1] = col1;
			output7[0] = row1 + 1;
			output7[1] = col1 - 1;
			output8[0] = row1;
			output8[1] = col1 - 1;
			output9[0] = row1 - 1;
			output9[1] = col1 - 1;
		}
	}
};

__kernel void cellCounter(__global int* A, __global int* H, __global int* C, __global int* M)
{

	
	int id = get_global_id(0);
		
	if(A[id] == 0)
	{
		H[id] = 1;
	}
	else if(A[id] == 1)
	{
		C[id] = 1;
	}
	else
	{
		M[id] = 1;
	}

};

__kernel void cellHealthyCheck(__global float *output1, __global float *output2, __global float *output3, __global float *output4,
 __global float *output5,__global float *output6, __global float *output7, __global float *output8, __global float *output9,
	int row1, int col1)
{
	output1[0] = row1;
	output1[1] = col1;

	output2[0] = row1 - 1;
	output2[1] = col1 + 1;

	output3[0] = row1 - 1;
	output3[1] = col1 + 1;

	output4[0] = row1;
	output4[1] = col1 + 1;

	output5[0] = row1 + 1;
	output5[1] = col1 + 1;

	output6[0] = row1 + 1;
	output6[1] = col1;

	output7[0] = row1 + 1;
	output7[1] = col1 - 1;

	output8[0] = row1;
	output8[1] = col1 - 1;

	output9[0] = row1 - 1;
	output9[1] = col1 - 1;
	
};

__kernel void trackMedicineCells(__global float *output2, __global float *output3, __global float *output4,
 __global float *output5,__global float *output6, __global float *output7, __global float *output8, __global float *output9,
 __global int* A, int two, int three, int four, int five, int six, int seven, int eight, int nine)
{

	
	int i = get_global_id(0);
	
		if(A[i] == 2)
		{
			output2[two] = i;
			two++;
		}
		if(A[i] == 3)
		{
			output2[three] = i;
			three++;
		}
		if(A[i] == 4)
		{
			output2[four] = i;
			four++;
		}
		if(A[i] == 5)
		{
			output2[five] = i;
			five++;
		}
		if(A[i] == 6)
		{
			output2[six] = i;
			six++;
		}
		if(A[i] == 7)
		{
			output2[seven] = i;
			seven++;
		}
		if(A[i] == 8)
		{
			output2[eight] = i;
			eight++;
		}
		if(A[i] == 9)
		{
			output2[nine] = i;
			nine++;
		}
};


__kernel void cellCancerCheck(__global int* A)
{
	int i = get_global_id(0);
	int ctr = 0;
	if(A[i] == 0)
	{
		for(int k = (i - 1); k < (i + 2); k++)
		{
			if(A[i - 1] == 1)
			{
				ctr++;
			}
			if(A[i + 1] == 1)
			{
				ctr++;
			}
		}
		if(A[i] == 1)
		{
			ctr++;
		}
		if(A[i + 2] == 1)
		{
			ctr++;
		}

		if(ctr > 5)
		{
			A[i] = 1;
		}
	}


};

__kernel void moveMedCell(int medCell)
{
	int output1[500];
	int i = get_global_id(0);
	int j = 0;
	if(output1[i] != 0 && output1[i] != 1)
	{
		if(medCell == 2)
		{
			output1[i] == output1[j - 1];
		}
		if(medCell == 3)
		{
			output1[i] == output1[i + 1];
			output1[j] == output1[j + 1];
		}
		if(medCell == 4)
		{
			output1[i] == output1[i + 1];
		}
		if(medCell == 5)
		{
			output1[i] == output1[i + 1];
			output1[j] == output1[j - 1];
		}
		if(medCell == 6)
		{
			output1[i] == output1[j + 1];
		}
		if(medCell == 7)
		{
			output1[i] == output1[i - 1];
			output1[j] == output1[j + 1];
		}
		if(medCell == 8)
		{
			output1[i] == output1[i - 1];
		}
		if(medCell == 9)
		{
			output1[i] == output1[i - 1];
			output1[j] == output1[j - 1];
		}

	}
};

