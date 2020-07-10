// *****************************************************************************
// * Name        : main.cpp
// * Author      : Ruari Armstrong
// * Version     : 1.0
// * Copyright   : see LICENSE.md
// * Description : main file of CompareCommits, gives example of implementation.
// *****************************************************************************

#include <stddef.h>
#include <stdlib.h>	//to use system()
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

#include "boost/program_options.hpp"
namespace po = boost::program_options;

#include "CompareCommitsLibrary/CompareCommits.hpp"

/*******************************************************************************
 * Derrived Class from CompareCommits, provided as an example of implementation
 * of the CompareCommits class. CompareTwoCommits member function is overridden
 * to use the Linux diff system call for calculating the difference metric.
 ******************************************************************************/
class DiffCompareCommits: public CompareCommits{
public:
	DiffCompareCommits(){};
	/***************************************************************************
	 * Overrides CompareTwoCommits member function to use the Linux diff system
	 * call for calculating the difference metric.
	 *
	 * @param commit_path full path to the directory containing the files
	 * @param commit1_path full path to first file
	 * @param commit2_path full path to second file
	 * @param commit1_filename the file name of first file
	 * @param commit2_filename the file name of second file
	 * @param repo_name repositories name on GitHub
	 * @return difference metric between files or an error code
	 **************************************************************************/
	double CompareTwoCommits(
			std::string commit_path,
			std::string commit1_path,
			std::string commit2_path,
			std::string commit1_filename,
			std::string commit2_filename,
			std::string repo_name="") override
	{
		if (repo_name != "")
		{
			return DiffTwoCommitsGit(
					repo_name,
					commit_path,
					commit1_path,
					commit2_path,
					commit1_filename,
					commit2_filename);
		}
		else
		{
			return DiffTwoCommitsZip(
					commit_path,
					commit1_path,
					commit2_path,
					commit1_filename,
					commit2_filename);
		}
	}
	/***************************************************************************
	 * a destructor.
	 **************************************************************************/
	~DiffCompareCommits(){};

protected:
private:
	/*******************************************************************************
	* Calculates the difference between two git commits.
	*
	* @param repo_name repositories name on GitHub
	* @param commit_path full path to the directory containing the zipped commits
	* @param commit1_path full path to first commit folder
	* @param commit2_path full path to second commit folder
	* @param commit1_hash the long hash value for the first commit
	* @param commit2_hash the long hash value for the second commit
	* @return difference metric between first and second commit or an error code
	*******************************************************************************/
	double DiffTwoCommitsGit(
			std::string repo_name,
			std::string commit_path,
			std::string commit1_path,
			std::string commit2_path,
			std::string commit1_hash,
			std::string commit2_hash)
	{
		double similarity = -1;

		std::string command = "";
		std::string diff_commit_path = commit_path + "/diffs/" + commit1_hash + commit2_hash + ".txt";
		std::string commit1_folder_path = commit_path + "/" + repo_name + "-" + commit1_hash;
		std::string commit2_folder_path = commit_path + "/" + repo_name + "-" + commit2_hash;


		/* make diffs dir -verbose -parent */
		command = "mkdir -v -p '" + commit_path + "/diffs'";
		system(command.c_str());


		/* executing unzip command for both commit zips */
		/* q for quiet o for overwrite */
		command = "unzip -qo '" + commit1_path + "' -d '" + commit_path + "'";
		system(command.c_str());

		command = "unzip -qo '" + commit2_path + "' -d '" + commit_path + "'";
		system(command.c_str());


		/* executing diff command symmetrically */
		command = "diff --recursive --brief '" + commit1_folder_path + "' '" + commit2_folder_path + "' > '" + diff_commit_path + "'";
		system(command.c_str());


		/* read number of differences between folders(commits) */
		std::string line;
		std::ifstream file;
		try
		{
			file.open(diff_commit_path);
			double count = 0;
			if (file.is_open())
			{
				while (!file.eof())
				{
					getline(file, line);

					/* If line contains 'differ' */
					unsigned long pos = 0;
					while (true)
					{
						pos = line.find("differ", pos);
						if (pos != std::string::npos)
							count++;
						else
							break;
						++pos;
					}

					/* If line contains 'Only-in' */
					pos = 0;
					while (true)
					{
						pos = line.find("Only in", pos);
						if (pos != std::string::npos)
							count++;
						else
							break;
						++pos;
					}
				}
				file.close();
				similarity = count;
			}
			else
			{
				std::cerr << "Unable to open file \n";
				return -1;
			}
		}
		catch(std::ifstream::failure &e)
		{
			std::cerr << "Exception opening \n";
			std::cerr << e.code() << "\n";
			//return e.code();
			return -1; //FIXME tempfix
		}


		/* remove diff file */
		command = "rm '" + diff_commit_path + "'";
		system(command.c_str());


		/* remove unzipped commit folders (recursive no prompt) */
		command = "rm -rf '" + commit1_folder_path + "'";
		system(command.c_str());

		command = "rm -rf '" + commit2_folder_path + "'";
		system(command.c_str());


		return similarity;
	}

	/*******************************************************************************
	* Calculates the difference between two zip files.
	*
	* @param commit_path full path to the directory containing the zipped commits
	* @param commit1_path full path to first commit folder
	* @param commit2_path full path to second commit folder
	* @param commit1_filename file name of commit1 zip file
	* @param commit2_filename file name of commit2 zip file
	* @return similarity difference metric between first and second commit
	* 		   or an error code
	*******************************************************************************/
	double DiffTwoCommitsZip(
			std::string commit_path,
			std::string commit1_path,
			std::string commit2_path,
			std::string commit1_filename,
			std::string commit2_filename)
	{
		double similarity = -1;

		std::string command = "";
		std::string diffcommitpath = commit_path + "/diffs/" + commit1_filename	+ commit2_filename + ".txt";
		std::string commitfolderpath1 = commit_path + "/" + commit1_filename;
		std::string commitfolderpath2 = commit_path + "/" + commit2_filename;


		/* make diffs dir -verbose -parent */
		command = "mkdir -v -p '" + commit_path + "/diffs'";
		system(command.c_str());


		/* executing unzip command for both zips */
		/* q for quiet o for overwrite */
		command = "unzip -qo '" + commit1_path + "' -d " + commit_path;
		system(command.c_str());

		command = "unzip -qo '" + commit2_path + "' -d " + commit_path;
		system(command.c_str());


		/* executing diff command symmetrically */
		command = "diff --recursive --brief '" + commitfolderpath1 + "' '" + commitfolderpath2 + "' > '" + diffcommitpath + "'";
		system(command.c_str());


		/* read number of differences between folders */
		std::string line;
		std::ifstream file;
		try
		{
			file.open(diffcommitpath);
			double count = 0;
			if (file.is_open())
			{
				while (!file.eof())
				{
					getline(file, line);

					/* If line contains 'differ' */
					unsigned long pos = 0;
					while (true)
					{
						pos = line.find("differ", pos);
						if (pos != std::string::npos)
							count++;
						else
							break;
						++pos;
					}

					/* If line contains 'Only-in' */
					pos = 0;
					while (true)
					{
						pos = line.find("Only in", pos);
						if (pos != std::string::npos)
							count++;
						else
							break;
						++pos;
					}
				}
				file.close();
				similarity = count;
			}
			else
			{
				std::cerr << "Unable to open file \n";
				return -1;
			}
		}
		catch(std::ifstream::failure &e)
		{
			std::cout << "Exception opening/writing file \n";
			std::cout << e.code() << "\n";
			//return e.code();
			return -1; //FIXME tempfix

		}


		/* remove diff file */
		command = "rm '" + diffcommitpath + "'";
		system(command.c_str());


		/* remove unzipped commit folders (recursive no prompt) */
		command = "rm -rf '" + commitfolderpath1 + "'";
		system(command.c_str());

		command = "rm -rf '" + commitfolderpath2 + "'";
		system(command.c_str());


		return similarity;
	}
};


/*******************************************************************************
 * main.
 *
 * @param argc number of arguments passed
 * @param argv[] array of arguments
 * @return 0 success or an error code
 ******************************************************************************/
int main(int argc, char* argv[])
{
	/* Using boost/program_options to parse command line options */

	/* Declare variable names for args to be saved to */
	std::string filename;
	std::string repo_path;
	std::string repo_name;
	std::string commit_path;
	std::string start_commit_hash;
	std::string end_commit_hash;
	std::string input_file_path;

	try
	{
		/* Declare the supported options */
		po::options_description desc{"Allowed Options"};
		desc.add_options()
				("help", "prints usage information")
				("version","prints version of program")
				("repo,r", po::value<std::string>(&repo_path)->default_value(""),"repository local path")
				("name,n", po::value<std::string>(&repo_name)->default_value(""),"repository name")
				("commits,c", po::value<std::string>(&commit_path)->required(),"path to directory containing the zipped commits")
				("start,s", po::value<std::string>(&start_commit_hash)->default_value(""),"start commit hash")
				("end,e", po::value<std::string>(&end_commit_hash)->default_value(""),"end commit hash")
				("file,f", po::value<std::string>(&input_file_path)->default_value(""),"path to file containing list of commits to compare")
				("output-file,o", po::value<std::string>(&filename)->default_value("CompareCommits"),"output file name")
		;

		po::positional_options_description p;
		/* 1st arg with no option name treat as output-file option */
		p.add("output-file", 1);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
				options(desc).positional(p).run(), vm);

		if (vm.count("help"))
		{
			std::cout << desc << "\n";
			return 1;
		}

		if (vm.count("version"))
		{
			std::cout << "CompareCommits Version: " << CompareCommits::VersionNo << "\n";
			return 1;
		}

		/* called after help and version to stop required option exception being thrown */
		po::notify(vm);

		DiffCompareCommits dcc;
		int error;

		/* Print out options being ran */
		std::cout << "Attempting to run with options:\n" <<
				"Repository Path: " << repo_path << "\n" <<
				"Repository Name: " << repo_name << "\n" <<
				"Commit Path: " << commit_path << "\n" <<
				"Start Hash: " << start_commit_hash << "\n" <<
				"End Hash: " << end_commit_hash << "\n" <<
				"List of Commits Path: " << input_file_path << "\n" <<
				"Output File: " << filename << "\n" ;

		if (start_commit_hash != "" && end_commit_hash != "" && repo_path != "" && repo_name != "")
		{
			error = dcc.CommitCompareStartEnd(
					filename,
					repo_name,
					repo_path,
					commit_path,
					start_commit_hash,
					end_commit_hash);
		}
		else if (input_file_path != "" && repo_path != "" && repo_name != "")
		{
			std::vector<std::string> commits;
			std::string line;
			std::ifstream myfile(input_file_path);
			try
			{
				myfile.exceptions(std::ios::badbit);
				if (myfile.is_open())
				{
					while (getline(myfile, line))
					{
						commits.push_back(line);
					}
					myfile.close();
					error = dcc.CommitCompareVector(
							filename,
							repo_name,
							commit_path,
							commits);
				}
				else
				{
					std::cerr << "Unable to open file \n";
					return -1;
				}
			}
			catch (std::ifstream::failure &e)
			{
				std::cerr << "Exception opening/writing file \n";
				std::cerr << e.code() << "\n";
				//return e.code();
				return -1; //FIXME tempfix
			}
		}
		else if (repo_path != "" && repo_name != "")
		{
			error = dcc.CommitCompareAllGit(
					filename,
					repo_name,
					repo_path,
					commit_path);
		}
		else
		{
			error = dcc.CommitCompareAllZip(
					filename,
					commit_path);
		}

		if (error != 0 )
		{
			std::cerr << "Error occurred during execution \n";
			std::cerr << "Error: " << error << "\n";
			return -1;
		}
	}
	catch (const po::error &ex)
	{
		std::cerr << ex.what() << "\n";
		return -1;
	}

	return 0;
}
