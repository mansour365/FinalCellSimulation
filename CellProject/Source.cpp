//Class COMP 426 – NN
//Instructor : Professor R.Jayakumar
//Student ID : 27739656
//Name : Said-Mansour Maqsoudi
//Cancer Cell Simulation using OpenCL

#include <windows.h>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>//for printing
#include <fstream> //for the kernel source located outside the host
#include <vector> //for storing location of medicine cells
#include <thread> //for multithreading
#include <array> //for modern array implementation
#include <queue> //for storing points of injection
#define CL_ENABLE_EXCEPTIONS

//OpenCL libraries 
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <stdio.h>
#include <CL/cl.hpp> //for openCL


using namespace std;

void timer(int);
void keyboard(unsigned char key, int x, int y); //first argument is key press other two arguments are position of mouse
void generateInitialCancerCells();
void cellCounter();
void display();
void moveMedicineCell();
void trackMedicineCells();
void cellHealthyCheck();

//Stacks that hold location of each type of medicine cells 
vector<int> twoCoordinates;
vector<int> threeCoordinates;
vector<int> fourCoordinates;
vector<int> fiveCoordinates;
vector<int> sixCoordinates;
vector<int> sevenCoordinates;
vector<int> eightCoordinates;
vector<int> nineCoordinates;

//macros for grid
#define ROWS 500
#define COLUMNS 500
#define TOTAL 500*500

#define GLOBAL_SIZE 20

//macro for fps
#define FPS 30

//create a 2D array to hold the cell positions (initially all elements in the array are 0's which represent healthy cells)
int cell[ROWS][COLUMNS];

//add cancer cells (represented by integer 1) to 25% of the array
int totalCells = ROWS * COLUMNS;
int cancerCellCount = totalCells * 0.35;

//Queue for medicine cell injections where injection number is > 5
queue<int> medQueue;


//Done serially on the host
void setInitialCellColor()
{
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {

			if (cell[i][j] == 0) { //if cell value is 0 change to green
				glPointSize(10.0f);
				glColor3f(0.0f, 0.5f, 0.0f); //green
				glBegin(GL_POINTS);
				glVertex2i(i, j);
				glEnd();
			}
			else if (cell[i][j] == 1) { //if cell value is 1 change to red
				glPointSize(10.0f);
				glColor3f(0.85f, 0.07f, 0.2f); //red
				glBegin(GL_POINTS);
				glVertex2i(i, j);
				glEnd();
			}
			else {					//if cell value is any other number change to yellow
				glPointSize(10.0f);
				glColor3f(1.0f, 1.0f, 0.0f); //yellow
				glBegin(GL_POINTS);
				glVertex2i(i, j);
				glEnd();
			}
		}
	}
}

//Done serially on the host
void generateInitialCancerCells()
{
	int count = 0;
	while (count != cancerCellCount)
	{
		int row = rand() % ROWS; //generate random row
		int column = rand() % COLUMNS; //generate random column
		cell[row][column] = 1;

		count++;
	}
}

//Done on GPU using openCL
void cellCounter()
{
	//-----------------------------------------------------------------
	//Input and output arguments for kernel
	//-----------------------------------------------------------------
	int* A = new int[TOTAL]; //this will be the flattened 2d cell array
	int* H = new int[TOTAL];
	int* C = new int[TOTAL];
	int* M = new int[TOTAL];

	int ith = 0;
	for (int i = 0; i < ROWS; i++)//initialize the A array with cell 2d array
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			A[ith] = cell[i][j];
			ith++;
		}
	}

	for (int i = 0; i < TOTAL; i++)	//initialize the other arrays with 0's
	{
		H[i] = 0;
		C[i] = 0;
		M[i] = 0;
	}


	//-----------------------------------------------------------------
	//Get the platform
	//-----------------------------------------------------------------
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	auto platform = platforms.front();


	//-----------------------------------------------------------------
	//Get the GPU device
	//-----------------------------------------------------------------
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	auto device = devices.front();


	//-----------------------------------------------------------------
	//Create the context with the device
	//-----------------------------------------------------------------
	cl::Context context(device);


	//-----------------------------------------------------------------
	//create a command queue for the device
	//-----------------------------------------------------------------
	cl::CommandQueue gpuQueue(context, device);


	//-----------------------------------------------------------------
	// Create memory buffers for input and output 
	//-----------------------------------------------------------------
	cl::Buffer bufferA(context, CL_MEM_READ_WRITE, sizeof(int) * TOTAL, NULL);
	cl::Buffer bufferH(context, CL_MEM_WRITE_ONLY, sizeof(int) * TOTAL, NULL);
	cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, sizeof(int) * TOTAL, NULL);
	cl::Buffer bufferM(context, CL_MEM_WRITE_ONLY, sizeof(int) * TOTAL, NULL);


	//-----------------------------------------------------------------
	//copy the input data to the input buffer using command queue for first device
	//-----------------------------------------------------------------
	gpuQueue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, sizeof(int) * TOTAL, A);


	//-----------------------------------------------------------------
	//Read kernel file then create program
	//-----------------------------------------------------------------
	std::ifstream KernelFile("kernelSource.cl");
	std::string src(std::istreambuf_iterator<char>(KernelFile), (std::istreambuf_iterator<char>()));	//write that into a string
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));	//load that into an opencl structure
	cl::Program program(context, sources);	//create a program with this source code
	auto err = program.build("-cl-std=CL1.2"); //build the program for the devices


	//-----------------------------------------------------------------
	//create the kernel
	//-----------------------------------------------------------------
	cl::Kernel kernel(program, "cellCounter", &err);

	//set kernel arguments
	err = kernel.setArg(0, bufferA); //set kernel arguments
	err = kernel.setArg(1, bufferH);
	err = kernel.setArg(2, bufferC);
	err = kernel.setArg(3, bufferM);


	//Execute the kernel
	err = gpuQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(TOTAL)); 	//enqueue task only executes the kernel once, so have to use enqueueNDRangeKernel

	//Copy the output back to the host
	err = gpuQueue.enqueueReadBuffer(bufferH, CL_TRUE, 0, sizeof(int) * TOTAL, H);
	err = gpuQueue.enqueueReadBuffer(bufferC, CL_TRUE, 0, sizeof(int) * TOTAL, C);
	err = gpuQueue.enqueueReadBuffer(bufferM, CL_TRUE, 0, sizeof(int) * TOTAL, M);

	cl::finish();


	int hCount = 0;
	int cCount = 0;
	int mCount = 0;
	for (int i = 0; i < TOTAL; i++)
	{
		if (H[i] == 1)
		{
			hCount++;
		}
		if (C[i] == 1)
		{
			cCount++;
		}
		if (M[i] == 1)
		{
			mCount++;
		}

	}
	cout << "healthyCellCount: " << hCount << ", cancerCellCount: " << cCount << ", medCellCount: " << mCount << endl;
	cout << endl;
}

//Done on CPU using openCL
void cellHealthyCheck()
{
	//If we are inside this function then the medQueue is not empty
	//We will pop a row and pop a column

	//-----------------------------------------------------------------
	//Input and output arguments for kernel
	//-----------------------------------------------------------------
	int row1 = medQueue.front();
	medQueue.pop();

	int col1 = medQueue.front();
	medQueue.pop();
	

	//-----------------------------------------------------------------
	//This is where the results will be placed
	//-----------------------------------------------------------------
	float resultArr1[2] = {};
	float resultArr2[2] = {};
	float resultArr3[2] = {};
	float resultArr4[2] = {};
	float resultArr5[2] = {};
	float resultArr6[2] = {};
	float resultArr7[2] = {};
	float resultArr8[2] = {};
	float resultArr9[2] = {};

	//Use kernel to calcualte locations of all cells that should be healthy
	//Give kernel row and column as input
	//Then update the main array

	//-----------------------------------------------------------------
	//Get the platform
	//-----------------------------------------------------------------
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	auto platform = platforms.front();


	//-----------------------------------------------------------------
	//Get the GPU device
	//-----------------------------------------------------------------
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
	auto device = devices.front();



	//-----------------------------------------------------------------
	//Create the context with the device
	//-----------------------------------------------------------------
	cl::Context context(device);


	//-----------------------------------------------------------------
	//Read kernel file then create program
	//-----------------------------------------------------------------
	std::ifstream KernelFile("kernelSource.cl");//write that into a string
	std::string src(std::istreambuf_iterator<char>(KernelFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));	//load that into an opencl structure
	cl::Program program(context, sources);	//create a program with this source
	auto err = program.build("-cl-std=CL1.2");


	//-----------------------------------------------------------------
	// Create memory buffers for input and output 
	//-----------------------------------------------------------------
	cl::Buffer bufferArr1(context, CL_MEM_WRITE_ONLY, sizeof(float) * 2);
	cl::Buffer bufferArr2(context, CL_MEM_WRITE_ONLY, sizeof(float) * 2);
	cl::Buffer bufferArr3(context, CL_MEM_WRITE_ONLY, sizeof(float) * 2);
	cl::Buffer bufferArr4(context, CL_MEM_WRITE_ONLY, sizeof(float) * 2);
	cl::Buffer bufferArr5(context, CL_MEM_WRITE_ONLY, sizeof(float) * 2);
	cl::Buffer bufferArr6(context, CL_MEM_WRITE_ONLY, sizeof(float) * 2);
	cl::Buffer bufferArr7(context, CL_MEM_WRITE_ONLY, sizeof(float) * 2);
	cl::Buffer bufferArr8(context, CL_MEM_WRITE_ONLY, sizeof(float) * 2);
	cl::Buffer bufferArr9(context, CL_MEM_WRITE_ONLY, sizeof(float) * 2);

	//-----------------------------------------------------------------
	//create the kernel
	//-----------------------------------------------------------------
	cl::Kernel kernel(program, "cellHealthyCheck", &err);


	//set kernel arguments
	kernel.setArg(0, bufferArr1);
	kernel.setArg(1, bufferArr2);
	kernel.setArg(2, bufferArr3);
	kernel.setArg(3, bufferArr4);
	kernel.setArg(4, bufferArr5);
	kernel.setArg(5, bufferArr6);
	kernel.setArg(6, bufferArr7);
	kernel.setArg(7, bufferArr8);
	kernel.setArg(8, bufferArr9);
	kernel.setArg(9, row1);
	kernel.setArg(10, col1);


	//-----------------------------------------------------------------
	//create a command queue for the device
	//-----------------------------------------------------------------
	cl::CommandQueue cpuQueue(context, device);


	//Execute the kernel
	cpuQueue.enqueueTask(kernel);
	

	//Copy Results from output buffer
	cpuQueue.enqueueReadBuffer(bufferArr1, CL_TRUE, 0, sizeof(float) * 2, resultArr1);
	cpuQueue.enqueueReadBuffer(bufferArr2, CL_TRUE, 0, sizeof(float) * 2, resultArr2);
	cpuQueue.enqueueReadBuffer(bufferArr3, CL_TRUE, 0, sizeof(float) * 2, resultArr3);
	cpuQueue.enqueueReadBuffer(bufferArr4, CL_TRUE, 0, sizeof(float) * 2, resultArr4);
	cpuQueue.enqueueReadBuffer(bufferArr5, CL_TRUE, 0, sizeof(float) * 2, resultArr5);
	cpuQueue.enqueueReadBuffer(bufferArr6, CL_TRUE, 0, sizeof(float) * 2, resultArr6);
	cpuQueue.enqueueReadBuffer(bufferArr7, CL_TRUE, 0, sizeof(float) * 2, resultArr7);
	cpuQueue.enqueueReadBuffer(bufferArr8, CL_TRUE, 0, sizeof(float) * 2, resultArr8);
	cpuQueue.enqueueReadBuffer(bufferArr9, CL_TRUE, 0, sizeof(float) * 2, resultArr9);
	

	//update the main simulation with new data
	int i1 = resultArr1[0]; int j1 = resultArr1[1]; cell[i1][j1] = 0;
	int i2 = resultArr2[0]; int j2 = resultArr2[1];	cell[i2][j2] = 0;
	int i3 = resultArr3[0]; int j3 = resultArr3[1];	cell[i3][j3] = 0;
	int i4 = resultArr4[0]; int j4 = resultArr4[1];	cell[i4][j4] = 0;
	int i5 = resultArr5[0]; int j5 = resultArr5[1];	cell[i5][j5] = 0;
	int i6 = resultArr6[0]; int j6 = resultArr6[1];	cell[i6][j6] = 0;
	int i7 = resultArr7[0]; int j7 = resultArr7[1];	cell[i7][j7] = 0;
	int i8 = resultArr8[0]; int j8 = resultArr8[1];	cell[i8][j8] = 0;
	int i9 = resultArr9[0]; int j9 = resultArr9[1];	cell[i9][j9] = 0;

	
}

//Done on GPU using openCL
void cellCancerCheck()
{
	//-----------------------------------------------------------------
	//Input and output arguments for kernel
	//-----------------------------------------------------------------
	int* A = new int[TOTAL]; //this will be the flattened 2d cell array

	for (int x = 0; x < ROWS; x++)
		for (int y = 0; y < COLUMNS; y++) {
			if (cell[x][y] == 0) {int t = 0;
				for (int i = (x - 1); i < (x + 2); i++)
				{if (cell[i][y - 1] == 1) {t++; }
				if (cell[i][y + 1] == 1) {t++;}}
				if (cell[x - 1][y] == 1) {t++;}
				if (cell[x + 1][y] == 1) {t++;}
				if (t > 5) {cell[x][y] = -1;}
			}
		}

	//initialize the A array with cell 2d array
	int ith = 0;
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			A[ith] = cell[i][j];
			ith++;
		}
	}

	int results[ROWS] = {};

	//-----------------------------------------------------------------
	//Get the platform
	//-----------------------------------------------------------------
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	auto platform = platforms.front();

	//-----------------------------------------------------------------
	//Get the GPU device
	//-----------------------------------------------------------------
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	auto device = devices.front();

	//-----------------------------------------------------------------
	//Create the context with the device
	//-----------------------------------------------------------------
	cl::Context context(device);

	//-----------------------------------------------------------------
	//Read kernel file then create program
	//-----------------------------------------------------------------
	std::ifstream KernelFile("kernelSource.cl");//write that into a string
	std::string src(std::istreambuf_iterator<char>(KernelFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));	//load that into an opencl structure
	cl::Program program(context, sources);	//create a program with this source
	auto err = program.build("-cl-std=CL1.2");

	//-----------------------------------------------------------------
	// Create memory buffers for input and output 
	//-----------------------------------------------------------------
	cl::Buffer bufferA(context, CL_MEM_READ_ONLY, sizeof(int) * ROWS, NULL);


	//-----------------------------------------------------------------
	//create the kernel
	//-----------------------------------------------------------------
	cl::Kernel kernel(program, "cellCancerCheck", &err);

	//set kernel arguments
	kernel.setArg(0, bufferA);

	//-----------------------------------------------------------------
	//create a command queue for the device
	//-----------------------------------------------------------------
	cl::CommandQueue gpuQueue(context, device);


	//-----------------------------------------------------------------
	//copy the input data to the input buffer using command queue for first device
	//-----------------------------------------------------------------
	gpuQueue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, sizeof(int)* TOTAL, A);


	//Execute the kernel
	gpuQueue.enqueueTask(kernel);


	//Copy Results from output buffer
	gpuQueue.enqueueReadBuffer(bufferA, CL_TRUE, 0, sizeof(int) * TOTAL, results);

	//Update main simulation with new data
	int t = 0;
	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLUMNS; j++){
			if (cell[i][j] == -1){cell[i][j] = 1;}
		}

}

//Done on GPU using OpenCL
void trackMedicineCells()
{
	//-----------------------------------------------------------------
	//Input and output arguments for kernel
	//-----------------------------------------------------------------
	int* A = new int[TOTAL]; //this will be the flattened 2d cell array

	//initialize the A array with cell 2d array
	int ith = 0;
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			A[ith] = cell[i][j];
			ith++;
		}
	}

	int two = 0;
	int three = 0;
	int four = 0;
	int five = 0;
	int six = 0;
	int seven = 0;
	int eight = 0;
	int nine = 0;

	//this is where the results of the kernel will be placed
	float resultArr2[6] = {};
	float resultArr3[6] = {};
	float resultArr4[6] = {};
	float resultArr5[6] = {};
	float resultArr6[6] = {};
	float resultArr7[6] = {};
	float resultArr8[6] = {};
	float resultArr9[6] = {};

	//-----------------------------------------------------------------
	//Get the platform
	//-----------------------------------------------------------------
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	auto platform = platforms.front();

	//-----------------------------------------------------------------
	//Get the GPU device
	//-----------------------------------------------------------------
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	auto device = devices.front();

	//-----------------------------------------------------------------
	//Create the context with the device
	//-----------------------------------------------------------------
	cl::Context context(device);

	//-----------------------------------------------------------------
	//create a command queue for the device
	//-----------------------------------------------------------------
	cl::CommandQueue gpuQueue(context, device);

	//-----------------------------------------------------------------
	// Create memory buffers for input and output 
	//-----------------------------------------------------------------
	cl::Buffer bufferArr2(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr3(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr4(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr5(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr6(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr7(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr8(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr9(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferA(context, CL_MEM_READ_ONLY, sizeof(int) * TOTAL, NULL);


	//-----------------------------------------------------------------
	//copy the input data to the input buffer using command queue for first device
	//-----------------------------------------------------------------
	gpuQueue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, sizeof(int) * TOTAL, A);


	//-----------------------------------------------------------------
	//Read kernel file then create program
	//-----------------------------------------------------------------
	std::ifstream KernelFile("kernelSource.cl");
	std::string src(std::istreambuf_iterator<char>(KernelFile), (std::istreambuf_iterator<char>()));	//write that into a string
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));	//load that into an opencl structure
	cl::Program program(context, sources);	//create a program with this source code
	auto err = program.build("-cl-std=CL1.2"); //build the program for the devices

	//-----------------------------------------------------------------
	//create the kernel
	//-----------------------------------------------------------------
	cl::Kernel kernel(program, "trackMedicineCells", &err);


	//Set the arguments for the kernel
	kernel.setArg(0, bufferArr2);
	kernel.setArg(1, bufferArr3);
	kernel.setArg(2, bufferArr4);
	kernel.setArg(3, bufferArr5);
	kernel.setArg(4, bufferArr6);
	kernel.setArg(5, bufferArr7);
	kernel.setArg(6, bufferArr8);
	kernel.setArg(7, bufferArr9);
	kernel.setArg(8, bufferA);
	kernel.setArg(9, two);
	kernel.setArg(10, three);
	kernel.setArg(11, four);
	kernel.setArg(12, five);
	kernel.setArg(13, six);
	kernel.setArg(14, seven);
	kernel.setArg(15, eight);
	kernel.setArg(16, nine);


	//Execute the kernel
	err = gpuQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(TOTAL));

	//Copy Results from output buffer
	gpuQueue.enqueueReadBuffer(bufferArr2, CL_TRUE, 0, sizeof(float) * 6, resultArr2);
	gpuQueue.enqueueReadBuffer(bufferArr3, CL_TRUE, 0, sizeof(float) * 6, resultArr3);
	gpuQueue.enqueueReadBuffer(bufferArr4, CL_TRUE, 0, sizeof(float) * 6, resultArr4);
	gpuQueue.enqueueReadBuffer(bufferArr5, CL_TRUE, 0, sizeof(float) * 6, resultArr5);
	gpuQueue.enqueueReadBuffer(bufferArr6, CL_TRUE, 0, sizeof(float) * 6, resultArr6);
	gpuQueue.enqueueReadBuffer(bufferArr7, CL_TRUE, 0, sizeof(float) * 6, resultArr7);
	gpuQueue.enqueueReadBuffer(bufferArr8, CL_TRUE, 0, sizeof(float) * 6, resultArr8);
	gpuQueue.enqueueReadBuffer(bufferArr9, CL_TRUE, 0, sizeof(float) * 6, resultArr9);

	//update new data on the main simulation
	int medType = 0;
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			medType = cell[i][j];
			if (medType == 2) { twoCoordinates.push_back(j); twoCoordinates.push_back(i); }
			if (medType == 3) { threeCoordinates.push_back(j); threeCoordinates.push_back(i); }
			if (medType == 4) { fourCoordinates.push_back(j); fourCoordinates.push_back(i); }
			if (medType == 5) { fiveCoordinates.push_back(j); fiveCoordinates.push_back(i); }
			if (medType == 6) { sixCoordinates.push_back(j); sixCoordinates.push_back(i); }
			if (medType == 7) { sevenCoordinates.push_back(j); sevenCoordinates.push_back(i); }
			if (medType == 8) { eightCoordinates.push_back(j); eightCoordinates.push_back(i); }
			if (medType == 9) { nineCoordinates.push_back(j); nineCoordinates.push_back(i); }
		}
	}
}

//Done on GPU using OpenCL
void injectMedicineCells() {

	//Input arguments for kernel
	int row1 = rand() % (ROWS - 3) + 2;
	int col1 = rand() % (COLUMNS - 3) + 2;
	int row2 = rand() % (ROWS - 3) + 2;
	int col2 = rand() % (COLUMNS - 3) + 2;
	int row3 = rand() % (ROWS - 3) + 2;
	int col3 = rand() % (COLUMNS - 3) + 2;

	int med1 = (rand() % 8) + 1;
	int med2 = (rand() % 8) + 1;
	int med3 = (rand() % 8) + 1;

	//If med1 > 5 store row1 and col1 in a queue (will be used by "health cell check")
	if (med1 > 5) {
		if (cell[row1][col1] == 1) {
			medQueue.push(row1);
			medQueue.push(col1);}}
	if (med2 > 5) {
		if (cell[row2][col2] == 1) {
			medQueue.push(row2);
			medQueue.push(col2);}}
	if (med3 > 5) {
		if (cell[row3][col3] == 1) {
			medQueue.push(row3);
			medQueue.push(col3);}}

	size_t global;

	//this is where the results of the kernel will be placed
	float resultArr2[6] = {};
	float resultArr3[6] = {};
	float resultArr4[6] = {};
	float resultArr5[6] = {};
	float resultArr6[6] = {};
	float resultArr7[6] = {};
	float resultArr8[6] = {};
	float resultArr9[6] = {};

	//Get the platform
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	auto platform = platforms.front();


	//Get the GPU device
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	auto device = devices.front();


	//Create a context with the device
	cl::Context context(device);


	//Read kernel file then create program
	std::ifstream KernelFile("kernelSource.cl");
	std::string src(std::istreambuf_iterator<char>(KernelFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));
	cl::Program program(context, sources);
	auto err = program.build("-cl-std=CL1.2");

	// Create memory buffers for input and output 
	cl::Buffer bufferArr2(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr3(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr4(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr5(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr6(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr7(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr8(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);
	cl::Buffer bufferArr9(context, CL_MEM_WRITE_ONLY, sizeof(float) * 6);

	//create the kernel
	cl::Kernel kernel(program, "multipleInjection", &err);

	//Set the arguments for the kernel
	kernel.setArg(0, bufferArr2);
	kernel.setArg(1, bufferArr3);
	kernel.setArg(2, bufferArr4);
	kernel.setArg(3, bufferArr5);
	kernel.setArg(4, bufferArr6);
	kernel.setArg(5, bufferArr7);
	kernel.setArg(6, bufferArr8);
	kernel.setArg(7, bufferArr9);
	kernel.setArg(8, row1);
	kernel.setArg(9, col1);
	kernel.setArg(10, row2);
	kernel.setArg(11, col2);
	kernel.setArg(12, row3);
	kernel.setArg(13, col3);
	kernel.setArg(14, med1);
	kernel.setArg(15, med2);
	kernel.setArg(16, med3);


	//Enqueue the kernel command on the command queue
	cl::CommandQueue gpuQueue(context, device);

	//only want kernel to be executed once, do that with enqueueTask
	gpuQueue.enqueueTask(kernel);
	

	//Copy Results from output buffer
	gpuQueue.enqueueReadBuffer(bufferArr2, CL_TRUE, 0, sizeof(float) * 6, resultArr2);
	gpuQueue.enqueueReadBuffer(bufferArr3, CL_TRUE, 0, sizeof(float) * 6, resultArr3);
	gpuQueue.enqueueReadBuffer(bufferArr4, CL_TRUE, 0, sizeof(float) * 6, resultArr4);
	gpuQueue.enqueueReadBuffer(bufferArr5, CL_TRUE, 0, sizeof(float) * 6, resultArr5);
	gpuQueue.enqueueReadBuffer(bufferArr6, CL_TRUE, 0, sizeof(float) * 6, resultArr6);
	gpuQueue.enqueueReadBuffer(bufferArr7, CL_TRUE, 0, sizeof(float) * 6, resultArr7);
	gpuQueue.enqueueReadBuffer(bufferArr8, CL_TRUE, 0, sizeof(float) * 6, resultArr8);
	gpuQueue.enqueueReadBuffer(bufferArr9, CL_TRUE, 0, sizeof(float) * 6, resultArr9);

	//display message of completion
	printf("\nPerformed computation using OpenCL on the GPU\n\n");
	printf("Updating main Simulation\n\n");


	//Update the main simulation with new data
	row1 = resultArr2[0]; col1 = resultArr2[1];
	row2 = resultArr2[2]; col2 = resultArr2[3];
	row3 = resultArr2[4]; col3 = resultArr2[5];
	if (row1 != 0 && col1 != 0){cell[row1][col1] = 2;}
	if (row2 != 0 && col2 != 0){cell[row2][col2] = 2;}
	if (row3 != 0 && col3 != 0){cell[row3][col3] = 2;}
		
	row1 = resultArr3[0]; col1 = resultArr3[1];
	row2 = resultArr3[2]; col2 = resultArr3[3];
	row3 = resultArr3[4]; col3 = resultArr3[5];
	if (row1 != 0 && col1 != 0){cell[row1][col1] = 3;}
	if (row2 != 0 && col2 != 0){cell[row2][col2] = 3;}
	if (row3 != 0 && col3 != 0){cell[row3][col3] = 3;}

	row1 = resultArr4[0]; col1 = resultArr4[1];
	row2 = resultArr4[2]; col2 = resultArr4[3];
	row3 = resultArr4[4]; col3 = resultArr4[5];
	if (row1 != 0 && col1 != 0){cell[row1][col1] = 4;}
	if (row2 != 0 && col2 != 0){cell[row2][col2] = 4;}
	if (row3 != 0 && col3 != 0){cell[row3][col3] = 4;}

	row1 = resultArr5[0]; col1 = resultArr5[1];
	row2 = resultArr5[2]; col2 = resultArr5[3];
	row3 = resultArr5[4]; col3 = resultArr5[5];
	if (row1 != 0 && col1 != 0){cell[row1][col1] = 5;}
	if (row2 != 0 && col2 != 0){cell[row2][col2] = 5;}
	if (row3 != 0 && col3 != 0){cell[row3][col3] = 5;}

	row1 = resultArr6[0]; col1 = resultArr6[1];
	row2 = resultArr6[2]; col2 = resultArr6[3];
	row3 = resultArr6[4]; col3 = resultArr6[5];
	if (row1 != 0 && col1 != 0){cell[row1][col1] = 6;}
	if (row2 != 0 && col2 != 0){cell[row2][col2] = 6;}
	if (row3 != 0 && col3 != 0){cell[row3][col3] = 6;}

	row1 = resultArr7[0]; col1 = resultArr7[1];
	row2 = resultArr7[2]; col2 = resultArr7[3];
	row3 = resultArr7[4]; col3 = resultArr7[5];
	if (row1 != 0 && col1 != 0){cell[row1][col1] = 7;}
	if (row2 != 0 && col2 != 0){cell[row2][col2] = 7;}
	if (row3 != 0 && col3 != 0){cell[row3][col3] = 7;}

	row1 = resultArr8[0]; col1 = resultArr8[1];
	row2 = resultArr8[2]; col2 = resultArr8[3];
	row3 = resultArr8[4]; col3 = resultArr8[5];
	if (row1 != 0 && col1 != 0){cell[row1][col1] = 8;}
	if (row2 != 0 && col2 != 0){cell[row2][col2] = 8;}
	if (row3 != 0 && col3 != 0){cell[row3][col3] = 8;}

	row1 = resultArr9[0]; col1 = resultArr9[1];
	row2 = resultArr9[2]; col2 = resultArr9[3];
	row3 = resultArr9[4]; col3 = resultArr9[5];
	if (row1 != 0 && col1 != 0){cell[row1][col1] = 9;}
	if (row2 != 0 && col2 != 0){cell[row2][col2] = 9;}
	if (row3 != 0 && col3 != 0){cell[row3][col3] = 9;}

};

//Done on CPU using openCL
void moveMedicineCell()
{
	//input
	int medCell = 0;

	//initialization
	while (!twoCoordinates.empty()) {
		int i = twoCoordinates.back(); twoCoordinates.pop_back();
		int j = twoCoordinates.back(); twoCoordinates.pop_back();
		if (j + 1 == 0 || i + 1 == 0 || i == ROWS - 1 || j == COLUMNS - 2) { cell[i][j] = 0; }
		else { int temp = cell[i - 1][j]; cell[i][j] = temp; cell[i - 1][j] = 2; }
	}
	while (!threeCoordinates.empty()) {
		int i = threeCoordinates.back(); threeCoordinates.pop_back();
		int j = threeCoordinates.back(); threeCoordinates.pop_back();
		if (j + 1 == 0 || i + 1 == 0 || i == ROWS - 1 || j == COLUMNS - 2) { cell[i][j] = 0; }
		else { int temp = cell[i - 1][j + 1]; cell[i][j] = temp; cell[i - 1][j + 1] = 3; }
	}
	while (!fourCoordinates.empty()) {
		int i = fourCoordinates.back(); fourCoordinates.pop_back();
		int j = fourCoordinates.back(); fourCoordinates.pop_back();
		if (j + 1 == 0 || i + 1 == 0 || i == ROWS - 1 || j == COLUMNS - 2) { cell[i][j] = 0; }
		else { int temp = cell[i][j + 1]; cell[i][j] = temp; cell[i][j + 1] = 4; }
	}
	while (!fiveCoordinates.empty()) {
		int i = fiveCoordinates.back(); fiveCoordinates.pop_back();
		int j = fiveCoordinates.back(); fiveCoordinates.pop_back();
		if (i + 1 == 0 || j + 1 == 0 || i == ROWS - 1 || j == COLUMNS - 1) { cell[i][j] = 0; }
		else { int temp = cell[i + 1][j + 1]; cell[i][j] = temp; cell[i + 1][j + 1] = 5; }
	}

	float resultArr1[2] = {};
	//-----------------------------------------------------------------
	//Get the platform
	//-----------------------------------------------------------------
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	auto platform = platforms.front();

	//-----------------------------------------------------------------
	//Get the GPU device
	//-----------------------------------------------------------------
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	auto device = devices.front();

	//-----------------------------------------------------------------
	//Create the context with the device
	//-----------------------------------------------------------------
	cl::Context context(device);


	//-----------------------------------------------------------------
	//Read kernel file then create program
	//-----------------------------------------------------------------
	std::ifstream KernelFile("kernelSource.cl");//write that into a string
	std::string src(std::istreambuf_iterator<char>(KernelFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));	//load that into an opencl structure
	cl::Program program(context, sources);	//create a program with this source
	auto err = program.build("-cl-std=CL1.2");


	//-----------------------------------------------------------------
	//create the kernel
	//-----------------------------------------------------------------
	cl::Kernel kernel(program, "moveMedCell", &err);


	//set kernel arguments
	kernel.setArg(0, medCell);


	//-----------------------------------------------------------------
	//create a command queue for the device
	//-----------------------------------------------------------------
	cl::CommandQueue cpuQueue(context, device);

	//Execute the kernel
	cpuQueue.enqueueTask(kernel);



	//Update main simulation with new data
	while (!sixCoordinates.empty()) {
		int i = sixCoordinates.back(); sixCoordinates.pop_back();
		int j = sixCoordinates.back(); sixCoordinates.pop_back();
		if (i + 1 == 0 || i == ROWS - 1 || j == COLUMNS - 1) { cell[i][j] = 0; }
		else { int temp = cell[i + 1][j]; cell[i][j] = temp; cell[i + 1][j] = 6; }
	}
	while (!sevenCoordinates.empty()) {
		int i = sevenCoordinates.back(); sevenCoordinates.pop_back();
		int j = sevenCoordinates.back(); sevenCoordinates.pop_back();
		if (i + 1 == 0 || j - 1 == 0 || i == ROWS - 1 || j == COLUMNS - 1) { cell[i][j] = 0; }
		else { int temp = cell[i + 1][j - 1]; cell[i][j] = temp; cell[i + 1][j - 1] = 7; }
	}
	while (!eightCoordinates.empty()) {
		int i = eightCoordinates.back(); eightCoordinates.pop_back();
		int j = eightCoordinates.back(); eightCoordinates.pop_back();
		if (i + 1 == 0 || j - 1 == 0 || i == ROWS - 1 || j == COLUMNS - 1) { cell[i][j] = 0; }
		else { int temp = cell[i][j - 1]; cell[i][j] = temp; cell[i][j - 1] = 8; }
	}
	while (!nineCoordinates.empty()) {
		int i = nineCoordinates.back(); nineCoordinates.pop_back();
		int j = nineCoordinates.back(); nineCoordinates.pop_back();
		if (i + 1 == 0 || j - 1 == 0 || i == ROWS - 1 || j == COLUMNS - 1) { cell[i][j] = 0; }
		else { int temp = cell[i - 1][j - 1]; cell[i][j] = temp; cell[i - 1][j - 1] = 9; }
	}

}



void display()
{
	//clear buffer at the begining of each frame
	glClear(GL_COLOR_BUFFER_BIT);

	//Serially on the host
	setInitialCellColor(); //set cell color given its current value 

	//using OpenCL on the GPU
	cellCancerCheck(); //check if cell should be converted to cancer cell

	//using OpenCL on the CPU
	if (!medQueue.empty()){	cellHealthyCheck();}//check if any cells should become healthy cells

	//using OpenCL on the GPU
	trackMedicineCells(); //track movement of each medicine cell
	
	//using OpenCL on the CPU
	if (!twoCoordinates.empty()) { moveMedicineCell(); } //move medicine cells to new cell

	//using OpenCL on the GPU
	cellCounter(); //count number of each type of cell, display results

	glutKeyboardFunc(keyboard); //check for user input

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(900, 900); //original 640 x 480
	glutCreateWindow("Cancer Cell Simulation using OpenCL");
	glClearColor(0.0, 0.0, 0.0, 0.0); //background color
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(-0.5f, COLUMNS - 0.5f, -0.5f, ROWS - 0.5f); //(left, right, bottom, top)

	//Serially on the host
	generateInitialCancerCells(); //randomly generate initial cancer cells in 2D array

	glutDisplayFunc(display);

	glutTimerFunc(0, timer, 0); //first argument is when the first frame is displayed, function, int value passed to timer (only executes once)

	glutMainLoop();

	return 0;
}


void keyboard(unsigned char key, int x, int y) //we dont need to know mouse position
{
	if (key == 27) // pressing escape key will end the program
	{
		exit(0);
	}

	if (key == 32) //pressing spacebar will generate a new random injection of medicine cells (32 == ascii code)
	{
		//random parallel medicine cell injections
		injectMedicineCells();
	}
}

void timer(int)
{
	glutPostRedisplay(); //tells opengl to call display function
	//re call the glutTimerFunc found in main
	//if we want 10 fps, 1s = 1000ms ,  1s / 10frames = 10 frames per second 
	glutTimerFunc(1000 / FPS, timer, 0);
}

