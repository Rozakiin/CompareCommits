// *****************************************************************************
// * Name        : SimilarityMatrix.h
// * Author      : Ruari Armstrong
// * Version     : 1.0
// * Copyright   : Your copyright notice
// * Description : Similarity or difference matrix using vector and map.
// *****************************************************************************

//TODO only printout valid entries not ones without hash value


#ifndef SIMILARITYMATRIX_H_
#define SIMILARITYMATRIX_H_

#include <stddef.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

typedef std::map<std::string, size_t> HASHINDEX_MAP;
/*******************************************************************************
 * a class representing a similarity or difference matrix using a vector and map.
 ******************************************************************************/
class SimilarityMatrix {
public:
	/*******************************************************************************
	 * Constructor for SimilarityMatrix.
	 *
	 * @param size
	 ******************************************************************************/
	SimilarityMatrix(size_t size);

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
	 * Gets the data at row i, column j.
	 *
	 * @param i row
	 * @param j column
	 * @return reference to data at position i,j or an error code
	 ******************************************************************************/
	double& operator()(size_t i, size_t j);

	/*******************************************************************************
	 * Gets value at row i, column j.
	 *
	 * @param i row
	 * @param j column
	 * @return value of data at position i,j or an error code
	 ******************************************************************************/
	double operator()(size_t i, size_t j) const;

	/* Getters */
	/*******************************************************************************
	 * Gets number of rows.
	 *
	 * @return number of rows
	 ******************************************************************************/
	size_t getRows() const;

	/*******************************************************************************
	 * Gets number of columns.
	 *
	 * @return number of columns
	 ******************************************************************************/
	size_t getCols() const;

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
	 * @return 0 success or an error code
	 ******************************************************************************/
	int add(std::string commit1_hash, std::string commit2_hash, double similarity);

	/*******************************************************************************
	 * Gets index for commit_hash.
	 *
	 * @param commit_hash key for a pair in commithashindexmap
	 * @return Index pointing to start of data for given key, or end() if not found.
	 ******************************************************************************/
	size_t getIndex(std::string commit_hash);

	/*******************************************************************************
	 * Gets hash string for value.
	 *
	 * @param value index value for a specific pair in commithashindexmap
	 * @return hash string for given value
	 ******************************************************************************/
	std::string getHash(size_t index);

	/*******************************************************************************
	 * Creates NEXUS file at output_path with name 'filename.nxs'.
	 *
	 * @param output_path
	 * @param filename
	 * @return 0 success or an error code
	 ******************************************************************************/
	int NexusOut(std::string output_path, std::string filename);
protected:
	/** a protected variable to store number of rows in the matrix. */
	size_t mRows;
	/** a protected variable to store number of columns in the matrix. */
	size_t mCols;
	/** a protected variable to store the data of the matrix. */
	std::vector<double> mData;
	/** a protected variable to store the map of vector index to label. */
	HASHINDEX_MAP commithashindexmap;
private:
};

/* Getters */

inline size_t SimilarityMatrix::getRows() const
{
	return mRows;
}


inline size_t SimilarityMatrix::getCols() const
{
	return mCols;
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
inline std::ostream& operator<<(std::ostream &os, const SimilarityMatrix &m)
{
	os << m.getRows() << " x " << m.getCols() << std::endl;

	for (auto elem : m.getMap())
	{
		os << elem.first << " " << elem.second << std::endl;
	}

	for (size_t i = 0; i != m.getData().size(); i++)
	{
		if (i % m.getCols() == 0)
			os << "\n"; //new line when at end of row
		os << m.getData().at(i) << " ";
	}

	return os;
}

/*******************************************************************************
 * Take in NEXUS formated data, save data to given SimilarityMatrix.
 *
 * @param is reference to istream
 * @param m reference to SimilarityMatrix to save data to
 * @return reference to istream
 ******************************************************************************/
std::istream& operator>>(std::istream &is, SimilarityMatrix &m);


#endif /* SIMILARITYMATRIX_H_ */
