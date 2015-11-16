/*
 * Module1Appl.cpp
 *
 *  Created on: Nov 10, 2015
 *      Author: Chandra
 */
#include <iostream>
#include <string>
#include <set>
#include <cassert>
#include <math.h>
#include <iomanip>
#include "../core/spline.h"

#include "Module1Appl.h"

using namespace std;

Module1Appl::Module1Appl() {
	initializeDataStructure();
}

Module1Appl::~Module1Appl() {
//	cout << "Module1Appl destructor" << endl;
}

int Module1Appl::moduleMainFunc(){
	assignMeanReversion();
	assignVol(0);
	double t = 0.0;
	double T = 0.0;
	double K = 0.04;
	cout << "a\t" << *(data.aMeanReversion.begin())<<endl;
	cout << "sigma\t" << *(data.sigma.begin())<<endl;
	cout << "Maturity\\Tenor\t" ;
	for(vector<double>::iterator it_ten = implVol.tenor.begin(); it_ten != implVol.tenor.end(); ++it_ten){
		cout << *it_ten << "\t";
	}
	cout << endl;
	for(vector<double>::iterator it_mat = implVol.maturity.begin(); it_mat!=implVol.maturity.end(); ++it_mat){
		cout << *it_mat << "\t";
		for(vector<double>::iterator it_ten = implVol.tenor.begin(); it_ten != implVol.tenor.end(); ++it_ten){
			calculateEt();

			t = *it_mat;
			T = *it_ten + t;
			double price = pSwaption(K, t, T);
			cout << price << "\t";
		}
		cout << endl;
	}
//
//	assignMeanReversion();
//	assignVol(0);
//	calculateEt();
////	calculateVr(1);
//	vector<double> th = theta();
//
//	cout << "a\t" << *(data.a.begin())<<"\t";
//	cout << "sigma\t" << *(data.sigma.begin())<<"\t";
//	for(vector<double>::iterator it = th.begin(); it != th.end(); ++it){
//		cout << *it << "\t" ;
//	}
//	cout << endl;

	return 0;
}

void Module1Appl::interpolatePricesAndForwardRates(vector<vector<double> > newNumbers){
	vector<vector<double> >::iterator vvi_iterator;
	vector<double>::iterator vi_iterator;
	vvi_iterator = newNumbers.begin();
	vi_iterator = (*vvi_iterator).begin();
	vector<double> X((*vvi_iterator).size());
	X = *vvi_iterator;

	// Initializing forwards data - 2nd column
	++vvi_iterator;
	vi_iterator = (*vvi_iterator).begin();
	vector<double> Y((*vvi_iterator).size());
	Y = *vvi_iterator;

	// Inserting year into set
	vi_iterator = X.begin();
	double left = *vi_iterator;
	double right = 0.0;
	++vi_iterator;
	std::set<double> yearSet;
	for(; vi_iterator!=X.end(); ++vi_iterator){
		right = *vi_iterator;
		for(double i=left; i<=right; i=i+0.25){
			yearSet.insert(i);
		}
		left = *vi_iterator;
	}

	// Initializing year and forwards data
	tk::spline s;
	s.set_points(X,Y);    // currently it is required that X is already sorted

	// Interpolating forwards data
	for (std::set<double>::iterator it=yearSet.begin(); it!=yearSet.end(); ++it){
		data.time.push_back(*it);
		data.forward.push_back(s(*it));
	}

	// Initializing discounts data - 3rd column
	++vvi_iterator;
	vi_iterator = (*vvi_iterator).begin();
	vector<double> Z((*vvi_iterator).size());
	Z = *vvi_iterator;

	// Interpolating discounts data
	s.set_points(X,Z);    // currently it is required that X is already sorted
	for (std::set<double>::iterator it=yearSet.begin(); it!=yearSet.end(); ++it){
		double t=s(*it);
		data.priceD.push_back(t);
	}
}

void Module1Appl::initializeDataStructure(){
	string directoryPath = fileUtils.getDirectoryPath();
	directoryPath += "\\src\\defs\\UCLA_Discounts.txt";
//	directoryPath = "E:\\work\\cpp_ws\\hullwhite\\src\\defs\\UCLA_Discounts.txt";
	cout << "Reading default input file: " << directoryPath << endl;
	vector<vector<double> > importData =
			fileUtils.importData(directoryPath);
	vector<vector<double> > forwardRateAndDiscountFactors =
			fileUtils.rowsToColumnTransposeVector(importData);
	interpolatePricesAndForwardRates(forwardRateAndDiscountFactors);

//	printStructure();

	for(int i=0; i<data.time.size(); i++){
		timePos[data.time[i]] = i;
	}

	directoryPath = fileUtils.getDirectoryPath();
	directoryPath += "\\src\\defs\\SwaptionVolatilityOnly.txt";
	vector<vector<double> > volData = fileUtils.importData(directoryPath);
	implVol.vol = volData;

	directoryPath = fileUtils.getDirectoryPath();
	directoryPath += "\\src\\defs\\SwaptionVolatilityMaturity.txt";
	vector<vector<double> > volMaturityData = fileUtils.importData(directoryPath);
	implVol.maturity = *(volMaturityData.begin());

	directoryPath = fileUtils.getDirectoryPath();
	directoryPath += "\\src\\defs\\SwaptionVolatilityTenor.txt";
	vector<vector<double> > volTenorData = fileUtils.importData(directoryPath);
	implVol.tenor = *(volTenorData.begin());
}

void Module1Appl::assignMeanReversion(){
	double a = 5.0/100;
	int x = data.time.size();
	data.aMeanReversion.assign(x, a);
}

void Module1Appl::assignVol(double s){
	double s1 = 70.0/100;
	int x = data.time.size();
	data.sigma.assign(x, s1);
}

void Module1Appl::printStructure(){
	vector<double>::iterator vi1;
	vector<double>::iterator vi2;
	vector<double>::iterator vi3;
	vector<double>::iterator vi4;
	vector<double>::iterator vi5;
	vector<double>::iterator vi6;
	for(vi1=data.time.begin(),
		vi2=data.forward.begin(),
		vi3=data.priceD.begin(),
		vi4=data.aMeanReversion.begin(),
		vi5=data.Et.begin(),
		vi6=data.sigma.begin()
		;
		vi1!=data.time.end();
		++vi1,++vi2,++vi3,++vi4,++vi5,++vi6) {
		cout << *vi1 << " " << *vi2 << " " << *vi3 << " "
			 << *vi4 << " "
			 << *vi5 << " " << *vi6 << " "
			 << endl;
	}
}

