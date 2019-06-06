//
//  VentTrackS.hpp
//  VentTrackS
//
//  Created by Tobias Lang on 07.04.17.
//
//

#ifndef VentTrackS_hpp
#define VentTrackS_hpp

#include <fstream>
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <string>

class VolumeData
{
public:
    unsigned int height, fHeight, width, fWidth, fDepth;
    unsigned int numVolumes, depth;
    unsigned char * data;
    std::string fileType;

    std::ifstream inFile;
    
    int volumeSize;
	int featureSize;

	unsigned int* feat;
    unsigned char** frame;
    unsigned char** fro;
    unsigned char** dx; // Displacement
    unsigned char** dy;
    unsigned char** dz;
    unsigned char** dm; // Displacement magnitude square
    
    void readPhilipsDicomFile();
	void doSomething();
    void resetFrames();
    void drawGrid();
	void fillFeat();
	void showFeature(int x, int y, int z, int f);
	void fillSeed(int x, int y, int z, int f);
	void FilterCreation(int size);
	void Print3D();
	void SSDbackward(int pos);
	void SSDforward(int pos);

	int sumOfSqares(int f);
	double sumOfSqareDifference(int x, int y, int z, int f);
    int gradientMagnitude(int f, int x, int y, int z);
    int specklitude(int f, int x, int y, int z);
    int idx(int x, int y, int z);
	int idx_get_x(int idx);
	int idx_get_y(int idx);
	int idx_get_z(int idx);
	int idxf(int x, int y, int z);
	int idxf_get_x(int idx);
	int idxf_get_y(int idx);
	int idxf_get_z(int idx);
    
    void bma();
};


int calcBrightness(int* x);
int* findSpeckle(int* x);

unsigned int sq(int x);
unsigned int mgn(int x, int y, int z);

#endif /* VentTrackS_hpp */
