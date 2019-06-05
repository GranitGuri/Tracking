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
int SEARCHDISTANCE = 1;
int FRAMEDISTANCE = 1;

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
    frame = new unsigned char*[numVolumes];
    fro   = new unsigned char*[numVolumes];
    dx   = new unsigned char*[numVolumes];
    dy   = new unsigned char*[numVolumes];
    dz   = new unsigned char*[numVolumes];
    dm   = new unsigned char*[numVolumes];
    for (unsigned int i=0; i<numVolumes; i++) {
        frame[i] = new unsigned char[featureSize];
        fro[i] = new unsigned char[volumeSize];
        dx[i] = new unsigned char[volumeSize];
        dy[i] = new unsigned char[volumeSize];
        dz[i] = new unsigned char[volumeSize];
        dm[i] = new unsigned char[volumeSize];
        inFile.read((char*) fro[i], volumeSize);
    }
	int pos = idx(100, 100, 100);
	fillFeat(pos);
//	FilterCreation(FEATURELENGTH);
//	fillSeed(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), 0);
//	Print3D();
//	SSDforward(pos);
//	SSDbackward(pos);
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

vector<vector<vector<double>>> feature = make_3d_vector(FEATURELENGTH, FEATURELENGTH, FEATURELENGTH, 0.0);
vector<vector<vector<double>>> GKernel = make_3d_vector(FEATURELENGTH, FEATURELENGTH, FEATURELENGTH, 0.0);

void VolumeData::fillSeed(int x, int y, int z, int f) {
		for (int i = z; i < z + FEATURELENGTH; i++)
			for (int j = y; j < y + FEATURELENGTH; j++)
				for (int k = x; k < x + FEATURELENGTH; k++)
					feature[k-x][j-y][i-z] = frame[f][idx(k-x, j-y, i-z)];
}

void VolumeData::showFeature(int x, int y, int z, int f) {
	for (int i = z; i < z + FEATURELENGTH; i++)
		for (int j = y; j < y + FEATURELENGTH; j++)
			for (int k = x; k < x + FEATURELENGTH; k++)
					frame[f][idx(k, j, i)] = 255;
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

void VolumeData::Print3D() {

	for (int y = 0; y < GKernel[0].size(); y++)
	{
		for (int z = 0; z < GKernel[0][0].size(); z++)
		{
			for (int x = 0; x < GKernel.size(); x++)
			{
				cout << GKernel[x][y][z];
			}
			cout << "    ";
		}
		cout << endl;
	}
	for (int y = 0; y < feature[0].size(); y++)
	{
		for (int z = 0; z < feature[0][0].size(); z++)
		{
			for (int x = 0; x < feature.size(); x++)
			{
				cout << feature[x][y][z];
			}
			cout << "    ";
		}
		cout << endl;
	}
	for (int y = 0; y < GKernel[0].size(); y++)
	{
		for (int z = 0; z < GKernel[0][0].size(); z++)
		{
			for (int x = 0; x < GKernel.size(); x++)
			{
				cout << +frame[0][idx(x, y, z)];
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
double VolumeData::sumOfSqareDifference(int x, int y, int z, int f, int length)
{
	double sum = 0;
	double diff = 0;
	for (int i = z; i < z + length; i++)
	{
		for (int j = y; j < y + length; j++)
		{
			for (int k = x; k < x + length; k++)
			{
				//int a = +feature[k - x][j - y][i - z];
				//int b = +frame[f][idx(k, j, i)];
				diff = GKernel[k - x][j - y][i - z] * (+feature[k - x][j - y][i - z] - +frame[f][idx(k - x, j - y, i - z)]);
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
int VolumeData::sumOfSqares(int x, int y, int z, int f, int length)
{
	int pos = idx(x, y, z);
	double min = 1000000;
	for (int i = z - SEARCHDISTANCE; i <= z + SEARCHDISTANCE; i++)
	{
		for (int j = y - SEARCHDISTANCE; j <= y + SEARCHDISTANCE; j++)
		{
			for (int k = x - SEARCHDISTANCE; k <= x + SEARCHDISTANCE; k++)
			{
				double s = sumOfSqareDifference(k, j, i, f, length);
				if (min > s)
				{
					min = s;
					pos = idx(k, j, i);
				}
			}
		}
	}

	cout << "Count " << min << endl;
	cout << idx_get_x(pos) << endl;
	cout << idx_get_y(pos) << endl;
	cout << idx_get_z(pos) << endl;
	return pos;
}

void VolumeData::SSDforward(int pos)
{
	for (unsigned int i = 0; i < FRAMEDISTANCE; i++) {
		fillSeed(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), i);
		pos = sumOfSqares(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), i + 1, FEATURELENGTH);
	}
}

void VolumeData::SSDbackward(int pos)
{
	for (unsigned int i = FRAMEDISTANCE; i > 0; i--) {
		fillSeed(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), i);
		pos = sumOfSqares(idx_get_x(pos), idx_get_y(pos), idx_get_z(pos), i - 1, FEATURELENGTH);
	}
}


/******************************************************************************************/
/***********************************BeispielFunktionen*************************************/
/******************************************************************************************/

void VolumeData::resetFrames() {
    for (unsigned int i=0; i<numVolumes; i++)
        for (int j=0; j<volumeSize; j++)
            frame[i][j] = fro[i][j];
    
}

void VolumeData::drawGrid() {
    for (unsigned int f=0; f<numVolumes; f++) {
        int itv = 50;
        for (unsigned int z = 0; z < depth; z += itv)
            for (int x = 0; x < height   ; x++)
                for (int y = 0; y < width; y++)
                    frame[f][idx(x,y,z)] = 255;
        for (int x = 0; x < height; x += itv)
            for (unsigned int z = 0; z < depth   ; z++)
                for (int y = 0; y < width; y++)
                    frame[f][idx(x,y,z)] = 255;
        for (int y = 0; y < width; y += itv)
            for (unsigned int z = 0; z < depth   ; z++)
                for (int x = 0; x < height; x++)
                    frame[f][idx(x,y,z)] = 255;
    }
}

void VolumeData::fillFeat(int pos) {
	int x = idx_get_x(pos);
	int y = idx_get_y(pos);
	int z = idx_get_z(pos);
    for (unsigned int f = 0; f < numVolumes; f++) {
        for (int i = 0; i < fHeight; i++) {
            for (int j = 0; j < fWidth; j++) {
                for (int  k = 0; k < fDepth; k++) {
					char bla = fro[f][idx(x + i, y + j, z + k)];
                    frame[f][idxf(i,j,k)] = fro[f][idx(x+i, y+j ,z+k)];                    
                }
            }
        }
    }
}

int VolumeData::gradientMagnitude(int f, int x, int y, int z) {
    int dx, dy, dz;
    
    if (x==0) dx = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x+1,y,z)]);
    else
        if (x==height-1) dx = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x-1,y,z)]);
        else dx = fro[f][idx(x+1,y,z)] - fro[f][idx(x-1,y,z)];
    
    if (y==0) dy = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x,y+1,z)]);
    else
        if (y==width-1) dy = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x,y-1,z)]);
        else dy = fro[f][idx(x,y+1,z)] - fro[f][idx(x,y-1,z)];
    
    if (z==0) dz = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x,y,z+1)]);
    else
        if (z==depth-1) dz = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x,y,z-1)]);
        else dz = fro[f][idx(x,y,z+1)] - fro[f][idx(x,y,z-1)];
    
    return dx*dx+dy*dy+dz*dz;
}

int VolumeData::specklitude(int f, int x, int y, int z) {
    int dx, dy, dz;
    
    if (x==0) dx = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x+1,y,z)]);
    else
        if (x==height-1) dx = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x-1,y,z)]);
        else dx = (fro[f][idx(x+1,y,z)] - fro[f][idx(x,y,z)])
            * (fro[f][idx(x-1,y,z)] - fro[f][idx(x,y,z)]);
    
    if (y==0) dy = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x,y+1,z)]);
    else
        if (y==width-1) dy = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x,y-1,z)]);
        else dy = (fro[f][idx(x,y+1,z)] - fro[f][idx(x,y,z)])
            * (fro[f][idx(x,y-1,z)] - fro[f][idx(x,y,z)]);
    
    if (z==0) dz = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x,y,z+1)]);
    else
        if (z==depth-1) dz = 2 * (fro[f][idx(x,y,z)] - fro[f][idx(x,y,z-1)]);
        else dz = (fro[f][idx(x,y,z+1)] - fro[f][idx(x,y,z)])
            * (fro[f][idx(x,y,z-1)] - fro[f][idx(x,y,z)]);
    
    //return dx*dx+dy*dy+dz*dz;
    return dx*+dy+dz;
}

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

int calcBrightness(unsigned int* x) {
    int windowSize = 3;
        // Calculate brightness
        int brightness = 0;
        for (int i = -windowSize; i <= windowSize; i++) {
            for (int j = -windowSize; j <= windowSize; j++) {
                for (int k = -windowSize; k <= windowSize; k++) {
                    if ((x[0]+i >= 0) && (x[0]+i < vd.width)
                        && (x[1]+j >= 0) && (x[1]+j < vd.height)
                        && (x[2]+k >= 0) && (x[2]+k < vd.depth))
                        brightness += 1;
                }
            }
        }
    return brightness;
}

unsigned int sq(int x) {
    return x*x;
}

unsigned int mgn(int x, int y, int z) {
    int m = x*x + y*y + z*z;
    return std::min(255, m);
}
