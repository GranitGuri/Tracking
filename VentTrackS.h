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

	int** neighborPositions;
	unsigned char** gradFrame;
	unsigned int* feat;
    unsigned char** frame;
	unsigned int* backfeat;
	unsigned char** backframe;
    unsigned char** fro;
    
    void readPhilipsDicomFile();
	void doSomething();
	void fillFeat(int f, int pos, bool b);
	void showFeature(int x, int y, int z, int f);
	void FilterCreation(int size);
	void FilterCreation2(int size);
	void Print3D();
	void SSDbackward();
	void SSDforward();
	void nearestNeighbors(int f, bool b);
	void calculateNeighborVector(int startPos, int shift, int f, int b);

	int sumOfSqares(int startPos,  int f, bool b);
	double sumOfSqareDifference(int x, int y, int z, int f, bool b);
    void gradientMagnitude();
	void gaussianBlur();
	void bit5map();

    int idx(int x, int y, int z);
	int idx_get_x(int idx);
	int idx_get_y(int idx);
	int idx_get_z(int idx);
	int idxf(int x, int y, int z);
	int idxf_get_x(int idx);
	int idxf_get_y(int idx);
	int idxf_get_z(int idx);
    
};

#endif /* VentTrackS_hpp */
