// *****************************************************************************
// * Name        : SimilarityMatrix.cpp
// * Author      : Ruari Armstrong
// * Version     : 1.0
// * Copyright   : Your copyright notice
// * Description : Similarity or difference matrix using vector and map.
// *****************************************************************************

#include "SimilarityMatrix.hpp"

#include <fstream>
#include <iomanip>
#include <regex>
#include <exception>

SimilarityMatrix::SimilarityMatrix(unsigned long size)
{
	/* Construct with size 1 then throw exception */
	if (size == 0) {
		mSize = 1;
		std::vector<double> d(1);
		mData = d;
		HASHINDEX_MAP m{};
		commithashindexmap = m;
		throw std::invalid_argument("Constructor called with zero size");
	}
	mSize = size;
	std::vector<double> d(size * size);
	mData = d;
	HASHINDEX_MAP m{};
	commithashindexmap = m;
}


SimilarityMatrix::~SimilarityMatrix() {}


SimilarityMatrix::SimilarityMatrix(const SimilarityMatrix &other) :
		mSize(other.mSize),
		mData(other.mData),
		commithashindexmap(other.commithashindexmap) {}


SimilarityMatrix& SimilarityMatrix::operator=(const SimilarityMatrix &other)
{
	this->mSize = other.mSize;
	this->mData = other.mData;
	this->commithashindexmap = other.commithashindexmap;

	return *this;
}


double& SimilarityMatrix::operator()(unsigned long i, unsigned long j)
{
	return mData.at(i * mSize + j);
}


double SimilarityMatrix::operator()(unsigned long i, unsigned long j) const
{
	return mData.at(i * mSize + j);
}


int SimilarityMatrix::add(
		std::string commit1_hash,
		std::string commit2_hash,
		double similarity)
{
	/* Check if key already in map, add to end of map if doesn't exist */
	if (commithashindexmap.count(commit1_hash) == 0)
	{
		commithashindexmap.insert(
				std::make_pair(commit1_hash, commithashindexmap.size()));
	}

	if (commithashindexmap.count(commit2_hash) == 0)
	{
		commithashindexmap.insert(
				std::make_pair(commit2_hash, commithashindexmap.size()));
	}

	/* Throws out of bound exception if !(pos < size()) */
	mData.at(getIndex(commit1_hash) * mSize + getIndex(commit2_hash)) = similarity;
	mData.at(getIndex(commit2_hash) * mSize + getIndex(commit1_hash)) = similarity;

	return 0;
}


unsigned long long SimilarityMatrix::getIndex(std::string commit_hash)
{
	return commithashindexmap.find(commit_hash)->second;
}


std::string SimilarityMatrix::getHash(unsigned long long index)
{
	/* Iterate through the map */
	for (auto element : commithashindexmap)
	{
		/* Check if value of this entry matches with given index */
		if(element.second == index)
		{
			/* Push the key in given map */
			return element.first;
		}
	}
	//TODO change to throw runtime error?
	return "";
}


int SimilarityMatrix::NexusOut(std::string output_path, std::string filename)
{
	// online converter http://phylogeny.lirmm.fr/phylo_cgi/data_converter.cgi
	unsigned long rownum = 0;
	std::ofstream myfile;
	myfile.open(output_path + filename + ".nxs");

	myfile <<
			"#NEXUS\n"
			"\n"
			"BEGIN TAXA;\n"
			"\tDIMENSIONS ntax="
			<< size()
			<< ";\n"
			"\tTAXLABELS ";
	for (unsigned long i = 0; i != size(); i++)
	{
		myfile << " " << getHash(i);
	}
	myfile <<
			";\n"
			"END;\n"
			"\n"
			"BEGIN DISTANCES;\n"
			"\tDIMENSIONS ntax="
			<< size() <<
			";\n"
			"\tFORMAT TRIANGLE=BOTH DIAGONAL LABELS MISSING=?;\n"
			"\tMATRIX\n";
	for (unsigned long long i = 0; i != getData().size(); i++)
	{
		/* if mod of numcols is 0, and not end of Data (at end of row) */
		if (i % size() == 0 && (i + 1) != (getData().size()))
		{
			myfile << "\n\t\t" << getHash(rownum++); //
		}
		myfile << "\t" << std::setw(7) << getData().at(i);
	}
	myfile <<
			"\n"
			"\t;\n"
			"END;\n";

	myfile.close();
	return 0;
}


std::istream& operator>>(std::istream &is, SimilarityMatrix &m)
{
	// TODO code istream operator
	char a;
	char b;
	std::string hash1;
	std::string hash2;
	double similarity;
	std::string line;

	// if( std::getline(is, line) )
	// {
	// 	std::stringstream iss(line);
	// 	if (iss >> hash1 >> a >> hash2 >> b  >> similarity)
	// 	{
	// 		if (a == ',' && b == ',')
	// 		{
	// 			/* remove leading and trailing whitespace */
	// 			hash1 = std::regex_replace(hash1, std::regex("^ +| +$|( ) +"), "$1");
	// 			hash2 = std::regex_replace(hash2, std::regex("^ +| +$|( ) +"), "$1");
	// 			m.add(hash1, hash2, similarity);
	// 		}
	// 		else
	// 		{
	// 			is.setstate(std::ios::failbit);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		is.setstate(std::ios::failbit);
	// 	}
	// }
	// else
	// {
	// 	is.setstate(std::ios::failbit);
	// }
	return is;
}
