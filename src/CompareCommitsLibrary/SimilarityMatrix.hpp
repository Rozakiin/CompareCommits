// *****************************************************************************
// * Name        : SimilarityMatrix.h
// * Author      : Ruari Armstrong
// * Version     : 1.0
// * Copyright   : Your copyright notice
// * Description : Similarity or difference matrix using vector and map.
// *****************************************************************************

//TODO only printout valid entries not ones without hash value


#ifndef SIMILARITYMATRIX_HPP_
#define SIMILARITYMATRIX_HPP_

#include <stddef.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

typedef std::map<std::string, unsigned long long> HASHINDEX_MAP;
/*******************************************************************************
 * a class representing a similarity or difference matrix using a vector and map.
 ******************************************************************************/
class SimilarityMatrix {
public:
	/*******************************************************************************
	 * Default constructor for SimilarityMatrix.
	 ******************************************************************************/
	SimilarityMatrix();
	
	/*******************************************************************************
	 * Constructor for SimilarityMatrix.
	 *
	 * @param size
	 * @throw invalid_argument
	 ******************************************************************************/
	SimilarityMatrix(unsigned long size) noexcept(false);

	/*******************************************************************************
	 * Destructor for SimilarityMatrix.
	 ******************************************************************************/
	virtual ~SimilarityMatrix();

	/*******************************************************************************
	 * Creates from copied values of other.
	 *
	 * @param other constant reference to a SimilarityMatrix object
	 ******************************************************************************/
	SimilarityMatrix(const SimilarityMatrix &other);

	/*******************************************************************************
	 * Sets values to those of other.
	 *
	 * @param other constant reference to a SimilarityMatrix object
	 * @return reference to SimilarityMatrix
	 ******************************************************************************/
	SimilarityMatrix& operator=(const SimilarityMatrix &other);

	/*******************************************************************************
	 * Gets reference to data at row i, column j.
	 *
	 * @param i row
	 * @param j column
	 * @return reference to data at position i,j or an error code
	 ******************************************************************************/
	double& operator()(unsigned long i, unsigned long j);

	/*******************************************************************************
	 * Gets value at row i, column j.
	 *
	 * @param i row
	 * @param j column
	 * @return value of data at position i,j or an error code
	 ******************************************************************************/
	double operator()(unsigned long i, unsigned long j) const;

	/* Getters */
	/*******************************************************************************
	 * Gets number of columns (size).
	 *
	 * @return number of columns
	 ******************************************************************************/
	unsigned long size() const;

	/*******************************************************************************
	 * Gets reference to vector of similarity data.
	 *
	 * @return constant reference to vector of similarity data
	 ******************************************************************************/
	std::vector<double> const& getData() const;

	/*******************************************************************************
	 * Gets reference to Hash-Index map.
	 *
	 * @return constant reference to the Hash-Index map
	 ******************************************************************************/
	HASHINDEX_MAP const& getMap() const;

	/* Methods */
	/*******************************************************************************
	 * Adds similarity between commit1_hash and commit2_hash.
	 *
	 * @param commit1_hash
	 * @param commit2_hash
	 * @param similarity
	 * @throw out_of_bounds
	 ******************************************************************************/
	void add(std::string commit1_hash, std::string commit2_hash, double similarity) noexcept(false);

	/*******************************************************************************
	 * Gets index for commit_hash.
	 *
	 * @param commit_hash key for a pair in commithashindexmap
	 * @return Index pointing to start of data for given key, or end() if not found.
	 ******************************************************************************/
	unsigned long long getIndex(std::string commit_hash);

	/*******************************************************************************
	 * Gets hash string for value.
	 *
	 * @param index index value for a specific pair in commithashindexmap
	 * @return hash string for given value
	 ******************************************************************************/
	std::string getHash(unsigned long long index);

	/*******************************************************************************
	 * Creates NEXUS file at output_path with name 'filename.nxs'.
	 *
	 * @param output_path
	 * @param filename
	 * @throw 
	 ******************************************************************************/
	void NexusOut(std::string output_path, std::string filename) noexcept(false);
protected:
	/** a protected variable to store number of columns in the matrix. */
	unsigned long mSize;
	/** a protected variable to store the data of the matrix. */
	std::vector<double> mData;
	/** a protected variable to store the map of vector index to label. */
	HASHINDEX_MAP commithashindexmap;
private:
};

/* Getters */

inline unsigned long SimilarityMatrix::size() const
{
	return mSize;
}


inline std::vector<double> const& SimilarityMatrix::getData() const
{
	return mData;
}


inline HASHINDEX_MAP const& SimilarityMatrix::getMap() const
{
	return commithashindexmap;
}

/* I/O stream operators */
/*******************************************************************************
 * output stream.
 *
 * @param os reference to ostream
 * @param m reference to SimilarityMatix to read from
 * @return reference to ostream
 ******************************************************************************/
std::ostream& operator<<(std::ostream &os, const SimilarityMatrix &m);

/*******************************************************************************
 * Take in NEXUS formated data, save data to given SimilarityMatrix.
 *
 * @param is reference to istream
 * @param m reference to SimilarityMatrix to save data to
 * @return reference to istream
 ******************************************************************************/
std::istream& operator>>(std::istream &is, SimilarityMatrix &m);


#endif /* SIMILARITYMATRIX_HPP_ */
