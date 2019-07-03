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
int NEIGHBORDISTANCE = 1;
bool SSDorNCC = false;

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
    frame = new unsigned char*[numVolumes];
	backfeat = new unsigned int[numVolumes];
	backframe = new unsigned char*[numVolumes];
    fro   = new unsigned char*[numVolumes];
    for (unsigned int i=0; i<numVolumes; i++) {
		gradFrame[i] = new unsigned char[volumeSize];
        frame[i] = new unsigned char[featureSize];
		backframe[i] = new unsigned char[featureSize];
        fro[i] = new unsigned char[volumeSize];
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
	cout << "from   ";
	cout << idx_get_x(startPos) << " ";
	cout << idx_get_y(startPos) << " ";
	cout << idx_get_z(startPos) << endl;
	cout << " mit Count " << min << endl;
	cout << "to   ";
	cout << idx_get_x(nextPos) << " ";
	cout << idx_get_y(nextPos) << " ";
	cout << idx_get_z(nextPos) << endl;
	return nextPos;
}

void VolumeData::forward() {
	for (unsigned int f = 0; f < FRAMEDISTANCE; f++) {
		fillFeat(f, pos, false);
		feat[f] = pos;
		if (f == FRAMEDISTANCE-1) {
			showFeature(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), f);
			break; 
		}
		//pos = minDifferences(pos, f, false, SSDorNCC);
		nearestNeighbors(f, true);
		showFeature(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), f);
		for (int i = 0; i < 7; i++) {
			if (neighborPositions[i][0] != NULL) {
				pos = idx(neighborPositions[i][0] + idx_get_x(pos), neighborPositions[i][1] + idx_get_y(pos), neighborPositions[i][2] + idx_get_z(pos));
				break;
			}
		}
	}
}

void VolumeData::backward() {
	for (unsigned int f = FRAMEDISTANCE-1; f >= 0; f--) {
		fillFeat(f, pos, true);
		backfeat[f] = pos;
		if (f == 0) {
			showFeature(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), f); 
			break; 
		}
		//pos = minDifferences(pos, f, false, SSDorNCC);
		nearestNeighbors(f, false);
		showFeature(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), f);
		for (int i = 0; i < 7; i++) {
			if (neighborPositions[i][0] != NULL) {
				pos = idx(neighborPositions[i][0] + idx_get_x(pos), neighborPositions[i][1] + idx_get_y(pos), neighborPositions[i][2] + idx_get_z(pos));
				break;
			}
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
				}
				else {
					backframe[f][idxf(i, j, k)] = gradFrame[f][idx(x + i, y + j, z + k)];
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
						dx = (fro[f][idx(k - 1, j, i)] + fro[f][idx(k + 1, j, i)]) / 2;
						dx = (fro[f][idx(k, j - 1, i)] + fro[f][idx(k, j + 1, i)]) / 2;
						dx = (fro[f][idx(k, j, i - 1)] + fro[f][idx(k, j, i + 1)]) / 2;
						gradFrame[f][idx(k, j, i)] = sqrt(dx*dx + dy * dy + dz * dz);
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
	fillFeat(f, neighborPos, false);
	int trans = minDifferences(neighborPos, f, b, SSDorNCC);
	neighborPositions[shift][0] = idx_get_x(trans) - x;
	neighborPositions[shift][1] = idx_get_y(trans) - y;
	neighborPositions[shift][2] = idx_get_z(trans) - z;
}

void VolumeData::dismissNeigbors() {
	double generalX = 0;
	double generalY = 0;
	double generalZ = 0;
	int posLeft = 0;
	for (int i = 0; i < 7; i++) {
		posLeft = 0;
		generalX = 0;
		generalY = 0;
		generalZ = 0;
		for (int i = 0; i < 7; i++) {
			if (neighborPositions[i][0] != NULL) {
				generalX += neighborPositions[i][0];
				generalY += neighborPositions[i][1];
				generalZ += neighborPositions[i][2];
				posLeft++;
			}
		}
		generalX = round(generalX / posLeft);
		generalY = round(generalY / posLeft);
		generalZ = round(generalZ / posLeft);
		double max = 0;
		int dissmiss = NULL;
		for (int i = 0; i < 7; i++) {
			double currX = sqrt((generalX - neighborPositions[i][0]) * (generalX - neighborPositions[i][0]));
			double currY = sqrt((generalY - neighborPositions[i][1]) * (generalY - neighborPositions[i][1]));
			double currZ = sqrt((generalZ - neighborPositions[i][2]) * (generalZ - neighborPositions[i][2]));
			if (neighborPositions[i][0] != NULL && currX > max) {
				max = currX;
				dissmiss = i;
			}
			if (neighborPositions[i][0] != NULL && currY > max) {
				max = currY;
				dissmiss = i;
			}
			if (neighborPositions[i][0] != NULL && currZ > max) {
				max = currZ;
				dissmiss = i;
			}
		}
		if (posLeft > 1) {
			cout << "position: " <<dissmiss << "vector: " << neighborPositions[i][0] << neighborPositions[i][1] << neighborPositions[i][2] << " Dissmissed with maximum difference: " << max << endl;
			neighborPositions[dissmiss][0] = NULL;
			neighborPositions[dissmiss][1] = NULL;
			neighborPositions[dissmiss][2] = NULL;
		}
	}
	for (int i = 0; i < 7; i++) {
		if (neighborPositions[i][0] != NULL) {
			cout << neighborPositions[i][0] << neighborPositions[i][1] << neighborPositions[i][2] << endl;
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

