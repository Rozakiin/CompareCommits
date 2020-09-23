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
#include <sstream>
#include <climits>

SimilarityMatrix::SimilarityMatrix() :
	mSize(1),
	mData(std::vector<double> (1)),
	commithashindexmap(HASHINDEX_MAP{}) {}

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


void SimilarityMatrix::add(
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

	/* at() Throws out of bound exception if pos > size() */
	mData.at(getIndex(commit1_hash) * mSize + getIndex(commit2_hash)) = similarity;
	mData.at(getIndex(commit2_hash) * mSize + getIndex(commit1_hash)) = similarity;
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


void SimilarityMatrix::NexusOut(std::string output_path, std::string filename)
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
}


std::ostream& operator<<(std::ostream &os, const SimilarityMatrix &m)
{
	os << m.size() << " x " << m.size() << std::endl;

	for (auto elem : m.getMap())
	{
		os << elem.first << " " << elem.second << std::endl;
	}

	for (std::size_t i = 0; i != m.getData().size(); i++)
	{
		if (i % m.size() == 0)
			os << "\n"; //new line when at end of row
		os << m.getData().at(i) << " ";
	}

	return os;
}


std::istream& operator>>(std::istream &is, SimilarityMatrix &m)
{
	//TODO simple read in, could be improved
	std::vector<std::string> labels;
	std::vector<double> matrix;
	std::string labelstring;
	std::string matrixstring;

	/* read in using [ ] as start and end of lines */
	is.ignore(INT_MAX, '[');
	std::getline(is, labelstring, ']');
	is.ignore(INT_MAX, '[');
	std::getline(is, matrixstring, ']');

	/* remove \n */
	labelstring.erase(std::remove(labelstring.begin(), labelstring.end(), '\n'), labelstring.end());
	matrixstring.erase(std::remove(matrixstring.begin(), matrixstring.end(), '\n'), matrixstring.end());

	/* add to vectors */
	std::stringstream sslabels(labelstring);
	std::stringstream ssmatrix(matrixstring);
	while (sslabels.good())
	{
		std::string label;
		getline(sslabels, label, ',');
		labels.push_back(label);
	}
	while (ssmatrix.good())
	{
		std::string elements;
		getline(ssmatrix, elements, ',');
		matrix.push_back(std::stod(elements));
	}

	/* add to SimilarityMatrix */
	m = SimilarityMatrix(static_cast<unsigned long>(labels.size()));
	for (std::size_t i=0; i != labels.size(); i++)
	{
		for (std::size_t j=0; j != labels.size(); j++)
		{
			m.add(labels.at(i), labels.at(j), matrix.at(i*labels.size()+j));
		}
	}


	return is;
}
