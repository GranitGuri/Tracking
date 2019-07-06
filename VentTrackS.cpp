//
//  VentTrackS.cpp
//  VentTrackS
//
//  Created by Tobias Lang on 07.04.17.
//
//

#include "VentTrackS.h"
#include <iterator>
#include <algorithm>
#include <iostream>
#include <vector>
#include <VMainWindow.h>

using namespace std;
VolumeData vd;

int FEATURELENGTH = 10;
int SEARCHDISTANCE = 5;
int FRAMEDISTANCE = 5;
int pos;
int KERNELSIZE = 3;
int NEIGHBORDISTANCE = 5;
bool SSDorNCC = false;
bool neigbors = true;
int REALNULL = 1337;

void VolumeData::readPhilipsDicomFile()
{
    /* Read data header */
    
    // Create structures
    double deltaX, deltaY, deltaZ;
    
    // Open file
    inFile.open("C:\\150430-KV-patched.dcm", std::ios::binary);
    if (!inFile.good()) {std::cerr << "Error reading file!\n"; exit(1);}
    
    // Read tags
    inFile.seekg(132);
    unsigned short tagA = 0, tagB = 0, dataTagA = 0x7FE0, dataTagB = 0x0010;
    unsigned int loopCounter = 0;
    while ((tagA != dataTagA || tagB != dataTagB) && loopCounter < 200) {
        loopCounter += 1;
        char code[2];
        inFile.read((char*) &tagA, 2);
        inFile.read((char*) &tagB, 2);
        inFile.read(code, 2);
        unsigned short sn;
        inFile.read((char*) &sn, 2);
        switch (tagA) {
            case 0x0018:
                if (tagB == 0x602c)
                    inFile.read((char*) &deltaX, 8);
                else
                    if (tagB == 0x0602e)
                        inFile.read((char*) &deltaY, 8);
                    else
                        inFile.seekg(sn, std::ios_base::cur);
                break;
            case 0x0028:
                if (tagB == 0x0008) {
                    std::string tmp(sn, ' ');
                    inFile.read(&tmp[0], sn);
                    numVolumes = std::stoi(tmp);
                }
                else
                    if (tagB == 0x0010)
                        inFile.read((char*) &height, 2);
                    else
                        if (tagB == 0x0011)
                            inFile.read((char*) &width, 2);
                        else
                            inFile.seekg(sn, std::ios_base::cur);
                break;
            case 0x3001:
                if (tagB == 0x1001)
                    inFile.read((char*) &depth, 4);
                else
                    if (tagB == 0x1003)
                        inFile.read((char*) &deltaZ, 8);
                    else
                        inFile.seekg(sn, std::ios_base::cur);
                break;
            default:
                inFile.seekg(sn, std::ios_base::cur);
                break;
        }
        if (std::strncmp(code, "OB", 2) == 0)
            inFile.seekg(6, std::ios_base::cur);
    }
    if (loopCounter >= 200) std::cerr << "Error reading tags!\n";
    
    // Read data size
    unsigned int n;
    inFile.read((char*) &n, 4);
    if (n!=height*width*depth*numVolumes) {
        std::cerr << "Matrix dimensions don't fit!\n";
        exit(2);
    }
    
    /* Read frames */
	fHeight = FEATURELENGTH; fWidth = FEATURELENGTH; fDepth = FEATURELENGTH;
    volumeSize = height * width * depth;
	featureSize = fHeight * fWidth * fDepth;
	neighborPositions = new int*[7];
	for (unsigned int i = 0; i < 7; i++) {
		neighborPositions[i] = new int[3];
	}
	gradFrame = new unsigned char*[numVolumes];
	feat = new unsigned int[numVolumes];
	backfeat = new unsigned int[numVolumes];
    frame = new unsigned char*[numVolumes];
	backframe = new unsigned char*[numVolumes];
	showFrame = new unsigned char*[numVolumes];
	showBackframe = new unsigned char*[numVolumes];
    fro = new unsigned char*[numVolumes];
	froFeature = new unsigned char*[numVolumes];
    for (unsigned int i=0; i<numVolumes; i++) {
		gradFrame[i] = new unsigned char[volumeSize];
        frame[i] = new unsigned char[featureSize];
		backframe[i] = new unsigned char[featureSize];
		showFrame[i] = new unsigned char[featureSize];
		showBackframe[i] = new unsigned char[featureSize];
        fro[i] = new unsigned char[volumeSize];
		froFeature[i] = new unsigned char[volumeSize];
        inFile.read((char*) fro[i], volumeSize);
    }
	pos = idx(120, 100, 110);
	FilterCreation(FEATURELENGTH);
	FilterCreation2(KERNELSIZE);
	gradientMagnitude();
//	gaussianBlur();
//	bit5map();
	forward();
	backward();
//	pos = idx(100, 100, 100);
//	nearestNeighbors(0, false);
//	Print3D();
	cout << endl;
    inFile.close();
}

/*Make 3D Vector
Input x,y,z Dimensions of a 3D Matrix, value Type of Input(Int, Char, ...)
Returns a 3D Matrix filled with Values of your Choice
*/
template<typename T>
std::vector<vector<vector<T>>> make_3d_vector(int z, int y, int x, T value = T{})
{
	return vector<vector<vector<T>>>(z, vector<vector<T>>(y, vector<T>(x, value)));
}

vector<vector<vector<double>>> GKernel = make_3d_vector(FEATURELENGTH, FEATURELENGTH, FEATURELENGTH, 0.0);
vector<vector<vector<double>>> GKernel2 = make_3d_vector(3, 3, 3, 0.0);

void VolumeData::showFeature(int x, int y, int z, int f) {
	for (int i = z-1; i <= z + FEATURELENGTH; i++)
		for (int j = y-1; j <= y + FEATURELENGTH; j++)
			for (int k = x-1; k <= x + FEATURELENGTH; k++)
				if(i == z-1 || i == FEATURELENGTH + z || j == y-1 || j == FEATURELENGTH + y || k == x-1 || k == FEATURELENGTH + x)
					fro[f][idx(k, j, i)] = 255;
}

/**
Creates a Gaussian Kernel of odd size
Input: int size(Odd)
Output: Overwrites the Global 3D Vector GKernel
**/
void VolumeData::FilterCreation(int size)
{
	// intialising standard deviation to 1.0 
	double sigma = 1.0;
	double r, s = 2.0 * sigma * sigma;

	// sum is for normalization 
	double sum = 0.0;
	int ksize = (size - 1) / 2;

	// generating kernel 
	for (int x = -ksize; x <= ksize; x++) {
		for (int y = -ksize; y <= ksize; y++) {
			for (int z = -ksize; z <= ksize; z++) {
				r = x * x + y * y + z * z;
				GKernel[x + ksize][y + ksize][z + ksize] = (exp(-r / s));
				sum += GKernel[x + ksize][y + ksize][z + ksize];
			}
		}
	}

	// normalising the Kernel 
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			for (int k = 0; k < size; ++k)
				GKernel[i][j][k] /= sum;
}

void VolumeData::FilterCreation2(int size)
{
	// intialising standard deviation to 1.0 
	double sigma = 1.0;
	double r, s = 2.0 * sigma * sigma;

	// sum is for normalization 
	double sum = 0.0;
	int ksize = (size - 1) / 2;

	// generating kernel 
	for (int x = -ksize; x <= ksize; x++) {
		for (int y = -ksize; y <= ksize; y++) {
			for (int z = -ksize; z <= ksize; z++) {
				r = x * x + y * y + z * z;
				GKernel2[x + ksize][y + ksize][z + ksize] = (exp(-r / s));
				sum += GKernel2[x + ksize][y + ksize][z + ksize];
			}
		}
	}

	// normalising the Kernel 
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			for (int k = 0; k < size; ++k)
				GKernel2[i][j][k] /= sum;
}

void VolumeData::Print3D() {

	for (int y = 0; y < GKernel[0].size(); y++)
	{
		for (int z = 0; z < GKernel[0][0].size(); z++)
		{
			for (int x = 0; x < GKernel.size(); x++)
			{
				cout << GKernel[x][y][z] << " ";
			}
			cout << "    ";
		}
		cout << endl;
	}
	for (int y = 0; y < 3; y++)
	{
		for (int z = 0; z < 3; z++)
		{
			for (int x = 0; x < 3; x++)
			{
				cout << +frame[0][idxf(x, y, z)] << " ";
			}
			cout << "    ";
		}
		cout << endl;
	}

	int x = 100;
	int y = 100;
	int z = 100;
	for (int j = 0; j < 3; j++)
	{
		for (int k = 0; k < 3; k++)
		{
			for (int i = 0; i < 3; i++)
			{
				cout << +fro[0][idx(i+x, j+y, k+z)] << " ";
			}
			cout << "    ";
		}
		cout << endl;
	}

}
/***************************************************************************************************************************************************/
/********************************SUM OF SQARE DIFFERENCE FUNCTIONS**********************************************************************************/
/***************************************************************************************************************************************************/


/*
Sum of the cubes Values
Input: x, y, z, frame, length
Output: Integer
*/
double VolumeData::sumOfSqareDifference(int sx, int sy, int sz, int f, bool b) {
	double sum = 0;
	double diff = 0;
	for (int i = 0; i < FEATURELENGTH; i++)
	{
		for (int j = 0; j < FEATURELENGTH; j++)
		{
			for (int k = 0; k <FEATURELENGTH; k++)
			{
				//cout << "fro+1" << +fro[f+1][idx(k, j, i)] << endl;
				//cout << "fro" << +fro[f][idx(k, j, i)] << endl;
				//cout << "frame" << +frame[f][idx(k-x, j-y, i-z)] << endl;
				if (f < numVolumes - 1 && b == false) {
					diff = GKernel[i][j][k] * (frame[f][idxf(i, j, k)] - gradFrame[f + 1][idx(i + sx, j + sy, k + sz)]);
				}
				else if (f == numVolumes - 1 && b == false) {
					diff = GKernel[i][j][k] * (frame[f][idxf(i, j, k)] - gradFrame[f][idx(i + sx, j + sy, k + sz)]);
				}
				if (f > 0 && b == true) {
					diff = GKernel[i][j][k] * (backframe[f][idxf(i, j, k)] - gradFrame[f - 1][idx(i + sx, j + sy, k + sz)]);
				}
				if (f == 0 && b == true) {
					diff = GKernel[i][j][k] * (backframe[f][idxf(i, j, k)] - gradFrame[f][idx(i + sx, j + sy, k + sz)]);
				}
				sum += diff * diff;
			}
		}
	}
	return sum;
}

/*
Sum of X Sqares
Input: x, y, z, frame, length
Output: Position of feature
*/
int VolumeData::minDifferences(int startPos, int f, bool b, bool SSDorNCC){
	int x = idx_get_x(startPos);
	int y = idx_get_y(startPos);
	int z = idx_get_z(startPos);
	int nextPos = 0;
	double min = 1000000;
	for (int i = z - SEARCHDISTANCE; i <= z + SEARCHDISTANCE; i++)
	{
		for (int j = y - SEARCHDISTANCE; j <= y + SEARCHDISTANCE; j++)
		{
			for (int k = x - SEARCHDISTANCE; k <= x + SEARCHDISTANCE; k++)
			{
				double s = 0;
				if (SSDorNCC == true) {
					s = sumOfSqareDifference(k, j, i, f, b);
				}
				else {
					s = NCC(k, j, i, f, b);
					if (s < 1) {
						s = 1 - s;
					}
					if (s > 1) {
						s = s - 1;
					}
				}
				if (min > s)
				{
					min = s;
					nextPos = idx(k, j, i);
				}
			}
		}
	}
	cout << "from StartPos:   ";
	cout << idx_get_x(startPos) << " ";
	cout << idx_get_y(startPos) << " ";
	cout << idx_get_z(startPos) << "   ";
	cout << " mit Count: " << min << "   ";
	cout << "to   ";
	cout << idx_get_x(nextPos) - idx_get_x(startPos) << " ";
	cout << idx_get_y(nextPos) - idx_get_y(startPos) << " ";
	cout << idx_get_z(nextPos) - idx_get_z(startPos) << endl;
	return nextPos;
}

/*
forward Algotythm
overwrites the positions and frame of feature tracking and runs from 0 to FRAMEDISTANCE
*/
void VolumeData::forward() {
	//loop throug frames
	for (unsigned int f = 0; f < FRAMEDISTANCE; f++) {
		//first fill the feature
		fillFeat(f, pos, false);
		feat[f] = pos;
		//if end is reached just show the current feature and quit
		if (f == FRAMEDISTANCE-1) {
			showFeature(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), f);
			break; 
		}
		//in case we dont take in account the neighbors
		if(neigbors == false){
			showFeature(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), f);
			pos = minDifferences(pos, f, false, SSDorNCC);
		}
		//with neighbors
		else {
			showFeature(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), f);
			//fill the array of resulting neighbor vectors
			nearestNeighbors(f, true);
			double vecX = 0;
			double vecY = 0;
			double vecZ = 0;
			int count = 1;
			//sum all vector x,y,z positions
			for (int i = 0; i < 7; i++) {
				if (neighborPositions[i][0] != REALNULL) {
					vecX += neighborPositions[i][0];
					vecY += neighborPositions[i][1];
					vecZ += neighborPositions[i][2];
					if (i > 0) {
						count++;
					}
					cout << i << "( " << neighborPositions[i][0] << " " << neighborPositions[i][1] << " " << neighborPositions[i][2] << " )";
				}
			}
			//divide and round them
			vecX = round(vecX / count);
			vecY = round(vecY / count);
			vecZ = round(vecZ / count);
			cout << "  =  (" << vecX << " " << vecY << " " << vecZ << ")";
			cout << endl;
			pos = idx(vecX, vecY, vecZ) + pos;
		}
	}
}

/*
backward Algotythm
overwrites the positions and backframe of feature tracking and runs from FRAMEDISTANCE to 0
*/
void VolumeData::backward() {
	for (unsigned int f = FRAMEDISTANCE-1; f >= 0; f--) {
		//first fill the feature
		fillFeat(f, pos, true);
		backfeat[f] = pos;
		//if start is reached just show the current feature and quit
		if (f == 0) {
			showFeature(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), f); 
			break; 
		}
		//in case we dont take in account the neighbors
		if (neigbors == false) {
			showFeature(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), f);
			pos = minDifferences(pos, f, false, SSDorNCC);
		}
		//with neighbors
		else {
			showFeature(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), f);
			//fill the array of resulting neighbor vector
			nearestNeighbors(f, false);
			double vecX = 0;
			double vecY = 0;
			double vecZ = 0;
			int count = 1;
			//sum all vector x,y,z positions
			for (int i = 0; i < 7; i++) {
				if (neighborPositions[i][0] != REALNULL) {
					vecX += neighborPositions[i][0];
					vecY += neighborPositions[i][1];
					vecZ += neighborPositions[i][2];
					if (i > 0) {
						count++;
					}
					cout << i << "(" << neighborPositions[i][0] << " " << neighborPositions[i][1] << " " << neighborPositions[i][2] << " )";
				}
			}
			//divide and round them
			vecX = round(vecX / count);
			vecY = round(vecY / count);
			vecZ = round(vecZ / count);
			cout << "  =  (" << vecX << " " << vecY << " " << vecZ << ")";
			cout << endl;
			pos = idx(vecX + idx_get_x(pos), vecY + idx_get_y(pos), vecZ + idx_get_z(pos));
		}
	}
}

void VolumeData::fillFeat(int f, int pos, bool b) {
	int x = idx_get_x(pos);
	int y = idx_get_y(pos);
	int z = idx_get_z(pos);
	for (int i = 0; i < FEATURELENGTH; i++) {
		for (int j = 0; j < FEATURELENGTH; j++) {
			for (int  k = 0; k < FEATURELENGTH; k++) {
				if (b == false) {
					frame[f][idxf(i, j, k)] = gradFrame[f][idx(x + i, y + j, z + k)];
					showFrame[f][idxf(i, j, k)] = froFeature[f][idx(x + i, y + j, z + k)];
				}
				else {
					backframe[f][idxf(i, j, k)] = gradFrame[f][idx(x + i, y + j, z + k)];
					showBackframe[f][idxf(i, j, k)] = froFeature[f][idx(x + i, y + j, z + k)];
				}
			}
		}
	}
}

/******************************************************************************************/
/***********************************Bildverbesserung***************************************/
/******************************************************************************************/

void VolumeData::gradientMagnitude() {
	int dx = 0;
	int dy = 0;
	int dz = 0;
	//			y1			y2			y3
	//		z1 z2 z3	z1 z2 z3	z1 z2 z3
	//x1	-  -  -		-  -  -		-  -  -
	//x2	-  -  -		-  -  -		-  -  -
	//x3	-  -  -		-  -  -		-  -  -
	int mask[3][3][3] = {	{ { 1,0,-1 }, { 3,0,-3 }, { 1,0,-1 } },
							{ { 3,0,-3 }, { 6,0,-6 }, { 3,0,-3 } },
							{ { 1,0,-1 }, { 3,0,-3 }, { 1,0,-1 } } };
	for (int f = 0; f < numVolumes; f++) 
	{
		for (int i = 0; i < depth; i++)
		{
			for (int j = 0; j < width; j++)
			{
				for (int k = 0; k < height; k++)
				{
					if (i == 0 || i == depth-1 || j == 0 || j == width-1 || k == 0 || k == height-1) {
						gradFrame[f][idx(k, j, i)] = 0;
					}
					else {
						dx = (fro[f][idx(k - 1, j, i)] - fro[f][idx(k + 1, j, i)]) / 2;
						dy = (fro[f][idx(k, j - 1, i)] - fro[f][idx(k, j + 1, i)]) / 2;
						dz = (fro[f][idx(k, j, i - 1)] - fro[f][idx(k, j, i + 1)]) / 2;
						gradFrame[f][idx(k, j, i)] = sqrt(dx*dx) + sqrt(dy*dy) + sqrt(dz*dz);
						froFeature[f][idx(k, j, i)] = fro[f][idx(k, j, i)];
						/*int trans = 0;
						for (int x = 0; x < 3; x++) {
							for (int y = 0; y < 3; y++) {
								for (int z = 0; z < 3; z++) {
									cout << mask[x][y][z] << " * " << (int)fro[f][idx(k + x - 1, j + x - 1, i + x - 1)] << endl;
									trans += mask[x][y][z] * fro[f][idx(k+x-1, j+x-1, i+x-1)];
								}
							}
						}
						cout << trans;
						gradFrame[f][idx(k, j, i)] = trans;*/
					}
				}
			}
		}
	}
}

void VolumeData::gaussianBlur() {
	int dx = 0;
	int dy = 0;
	int dz = 0;
	for (int f = 0; f < 2; f++)
	{
		for (int i = 0; i < depth; i++)
		{
			for (int j = 0; j < width; j++)
			{
				for (int k = 0; k < height; k++)
				{
					if (i == 0 || i == depth - 1 || j == 0 || j == width - 1 || k == 0 || k == height - 1) {
						fro[f][idx(k, j, i)] = 0;
					}
					else {
						for (int z = 0; z < 3; z++)
						{
							for (int y = 0; y < 3; y++)
							{
								for (int x = 0; x < 3; x++)
								{
									fro[f][idx(k, j, i)] = 0;
									fro[f][idx(k, j, i)] += gradFrame[f][idx(k + x - 1, j + y - 1, i + z - 1)] * GKernel2[x][y][z];
								}
							}
						}
					}
				}
			}
		}
	}
}

void VolumeData::bit5map() {
	int dx = 0;
	int dy = 0;
	int dz = 0;
	for (int f = 0; f < numVolumes; f++)
	{
		for (int i = 0; i < depth; i++)
		{
			for (int j = 0; j < width; j++)
			{
				for (int k = 0; k < height; k++)
				{
					if (gradFrame[f][idx(k, j, i)] <= 50) {
						gradFrame[f][idx(k, j, i)] = 0;
					}
					else if (gradFrame[f][idx(k, j, i)] > 50 && gradFrame[f][idx(k, j, i)] <= 75){
						gradFrame[f][idx(k, j, i)] = 50;
					}
					else if (gradFrame[f][idx(k, j, i)] > 75 && gradFrame[f][idx(k, j, i)] <= 125) {
						gradFrame[f][idx(k, j, i)] = 100;
					}
					else if (gradFrame[f][idx(k, j, i)] > 125 && gradFrame[f][idx(k, j, i)] <= 175) {
						gradFrame[f][idx(k, j, i)] = 150;
					}
					else if (gradFrame[f][idx(k, j, i)] > 175 && gradFrame[f][idx(k, j, i)] <= 225) {
						gradFrame[f][idx(k, j, i)] = 200;
					}
					else{
						gradFrame[f][idx(k, j, i)] = 255;
					}
				}
			}
		}
	}
}

/******************************************************************************************/
/***********************************nearest Neighbors**************************************/
/******************************************************************************************/

/*
fills the neighborPositions array
*/
void VolumeData::nearestNeighbors(int f, bool b) {
	int x = idx_get_x(pos);
	int y = idx_get_y(pos);
	int z = idx_get_z(pos);
	//Vector of main position shift
	calculateNeighborVector(pos, 0, f, b);
	//Vector of Above position shift
	calculateNeighborVector(pos, 1, f, b);
	//Vector of Underneath position shift
	calculateNeighborVector(pos, 2, f, b);
	//Vector of Left position shift
	calculateNeighborVector(pos, 3, f, b);
	//Vector of Right position shift
	calculateNeighborVector(pos, 4, f, b);
	//Vector of Behind position shift
	calculateNeighborVector(pos, 5, f, b);
	//Vector of Front position shift
	calculateNeighborVector(pos, 6, f, b);
	dismissNeigbors();
}
/*
calculates the transition vector for all neighbors
*/
void VolumeData::calculateNeighborVector(int startPos, int shift, int f, bool b) {
	int x = idx_get_x(startPos);
	int y = idx_get_y(startPos);
	int z = idx_get_z(startPos);
	switch (shift) {
	case 0: //Start
		break;
	case 1: //Above
		x -= NEIGHBORDISTANCE;
		break;
	case 2: //Under
		x += NEIGHBORDISTANCE;
		break;
	case 3: // Left
		y -= NEIGHBORDISTANCE;
		break;
	case 4: //Right
		y += NEIGHBORDISTANCE;
		break;
	case 5: //Behind
		z -= NEIGHBORDISTANCE;
		break;
	case 6: //Front
		z += NEIGHBORDISTANCE;
		break;
	}
	int neighborPos = idx(x, y, z);
	fillFeat(f, neighborPos, b);
	int trans = minDifferences(neighborPos, f, b, SSDorNCC);
	neighborPositions[shift][0] = idx_get_x(trans) - x;
	neighborPositions[shift][1] = idx_get_y(trans) - y;
	neighborPositions[shift][2] = idx_get_z(trans) - z;
}

void VolumeData::dismissNeigbors() {
	int left = 7;
	for (int i = 1; i < 7; i++) {
		double max = 0;
		int dissmiss = REALNULL;
		for (int i = 1; i < 7; i++) {
			double currX = abs(neighborPositions[0][0] - neighborPositions[i][0]);
			double currY = abs(neighborPositions[0][1] - neighborPositions[i][1]);
			double currZ = abs(neighborPositions[0][2] - neighborPositions[i][2]);
			if (neighborPositions[i][0] != REALNULL && currX > max) {
				max = currX;
				dissmiss = i;
				left--;
			}
			if (neighborPositions[i][0] != REALNULL && currY > max) {
				max = currY;
				dissmiss = i;
				left--;
			}
			if (neighborPositions[i][0] != REALNULL && currZ > max) {
				max = currZ;
				dissmiss = i;
				left--;
			}
		}
		if (left > 4 && max > 1) {
			neighborPositions[dissmiss][0] = REALNULL;
			neighborPositions[dissmiss][1] = REALNULL;
			neighborPositions[dissmiss][2] = REALNULL;
		}
	}
}

/******************************************************************************************/
/***********************************Cross-Correlation**************************************/
/******************************************************************************************/

double VolumeData::NCC(int sx, int sy, int sz, int f, bool b) {
	double mult = 0;
	double scalFrame = 0;
	double scalGradFrame = 0;
	for (int i = 0; i < FEATURELENGTH; i++)
	{
		for (int j = 0; j < FEATURELENGTH; j++)
		{
			for (int k = 0; k < FEATURELENGTH; k++)
			{
				if (f < numVolumes - 1 && b == false) {
					mult += frame[f][idxf(i, j, k)] * gradFrame[f + 1][idx(i + sx, j + sy, k + sz)];
					scalFrame += frame[f][idxf(i, j, k)] * frame[f][idxf(i, j, k)];
					scalGradFrame += gradFrame[f + 1][idx(i + sx, j + sy, k + sz)] * gradFrame[f + 1][idx(i + sx, j + sy, k + sz)];
				}
				else if (f == numVolumes - 1 && b == false) {
					mult += frame[f][idxf(i, j, k)] * gradFrame[f][idx(i + sx, j + sy, k + sz)];
					scalFrame += frame[f][idxf(i, j, k)] * frame[f][idxf(i, j, k)];
					scalGradFrame += gradFrame[f][idx(i + sx, j + sy, k + sz)] * gradFrame[f][idx(i + sx, j + sy, k + sz)];
				}
				if (f > 0 && b == true) {
					mult += backframe[f][idxf(i, j, k)] * gradFrame[f - 1][idx(i + sx, j + sy, k + sz)];
					scalFrame += backframe[f][idxf(i, j, k)] * backframe[f][idxf(i, j, k)];
					scalGradFrame += gradFrame[f - 1][idx(i + sx, j + sy, k + sz)] * gradFrame[f - 1][idx(i + sx, j + sy, k + sz)];
				}
				if (f == 0 && b == true) {
					mult += backframe[f][idxf(i, j, k)] * gradFrame[f][idx(i + sx, j + sy, k + sz)];
					scalFrame += backframe[f][idxf(i, j, k)] * backframe[f][idxf(i, j, k)];
					scalGradFrame += gradFrame[f][idx(i + sx, j + sy, k + sz)] * gradFrame[f][idx(i + sx, j + sy, k + sz)];
				}
			}
		}
	}
	double a = scalFrame * scalGradFrame;
	double bb = sqrt(a);
	double c = mult / bb;
	return c;
}

/******************************************************************************************/
/***********************************Koordinaten********************************************/
/******************************************************************************************/

int VolumeData::idx(int x, int y, int z) {
    return x*width + y + z*width*height;
}

int VolumeData::idxf(int x, int y, int z) {
	return x * fWidth + y + z * fWidth*fHeight;
}

int VolumeData::idx_get_x(int idx) {
	return floor((idx - (idx_get_z(idx) * width * height))/width);
}

int VolumeData::idx_get_y(int idx) {
	return idx - idx_get_x(idx)*width - idx_get_z(idx)*width*height;
}

int VolumeData::idx_get_z(int idx) {
	return floor(idx / (width*height));
}

