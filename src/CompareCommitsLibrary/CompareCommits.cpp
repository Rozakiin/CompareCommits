// *****************************************************************************
// * Name        : CompareCommits.cpp
// * Author      : Ruari Armstrong
// * Version     : 1.0
// * Copyright   : see LICENSE.md
// * Description : Compare commits, add to SimilarityMatrix and create a NEXUS
// *			   file for SplitsTree.
// *****************************************************************************

//TODO fix reposize from counting duplicates

#include <cstdlib>

#include "CompareCommits.hpp"

SimilarityMatrix CompareCommits::IterateOverDirectory(
		SimilarityMatrix& similarity_matrix, 
		std::string commit_path, 
		std::string file_extension, 
		std::string repo_name)
{
	SimilarityMatrix m = similarity_matrix;
	/* If path exists */
	fs::path startpath(commit_path);
	if (exists(startpath))
	{
		std::string commit1_filename;
		std::string commit2_filename;
		fs::path file;
		fs::path previousfile;
		double similarity;
		int error = 0;

		/* Iterate over directory */
		for (auto &dirEntry : fs::directory_iterator(commit_path))
		{
			file = dirEntry.path();
			/* if file path not empty then accessed a file already */
			if (!file.empty())
			{
				/* store previous file */
				previousfile = file;
				/* Iterate over all files in directory for comparison */
				for (auto &dirEntry2 : fs::directory_iterator(commit_path))
				{
					file = dirEntry2.path();
					/* if both paths exist and file extensions are the given file extension */
					bool fileisFileExtension = file.extension().string().compare(file_extension) == 0;
					bool previousfileIsFileExtension = previousfile.extension().string().compare(file_extension) == 0;
					if (exists(file) && exists(previousfile) && fileisFileExtension && previousfileIsFileExtension)
					{
						commit1_filename = previousfile.stem().string();
						commit2_filename = file.stem().string();

						similarity = this->CompareTwoCommits(
								commit_path,
								previousfile.string(),
								file.string(),
								commit1_filename,
								commit2_filename,
								repo_name);

						/* return if error comparing commits, print error number */
						if (similarity < 0)
						{
							std::cerr << "CompareTwoCommits("
									<< commit_path << ", "
									<< previousfile.string() << ", "
									<< file.string() << ", "
									<< commit1_filename << ", "
									<< commit2_filename << ", "
									<< repo_name << ")\n" 
									<< "Error " << similarity << "\n";
							return static_cast<int>(similarity);
						}

						error = m.add(commit1_filename, commit2_filename, similarity);

						/* return if error adding to SimilarityMatrix, print error number */
						if (error < 0)
						{
							std::cerr << "m.add("
									<< commit1_filename << ", "
									<< commit2_filename << ", "
									<< similarity << ")\n"
									<< "returned: " << error << "\n";
							return error;
						}
					}
				}
			}
		}
	}
	else
	{
		std::cerr << "PATH INVALID: " << commit_path << "\n";
		return -1;
	}
	return m;
}


int CompareCommits::CommitCompareAllZip(
		std::string filename,
		std::string commit_path)
{
	unsigned long long reposize = 0;
	int error = 0;

	/* If path exists */
	fs::path startpath(commit_path);
	if (exists(startpath))
	{
		/* Iterate over directory, count number of files with extension zip */
		for (auto &dirEntry : fs::directory_iterator(commit_path))
		{
			/* If file has extension zip */
			bool fileisZip = dirEntry.path().extension().string().compare(".zip") == 0;
			if (fileisZip)
			{
				reposize++;
			}
		}
	}
	else
	{
		std::cerr << "PATH INVALID: " << commit_path << "\n";
		return -1;
	}

	if (reposize == 0)
	{
		std::cerr << "Number of commits to compare is 0 \n";
		return -1;
	}

	SimilarityMatrix m = SimilarityMatrix(reposize);

	m = IterateOverDirectory(m, commit_path, ".zip");

	/* return if error iterating over directory, print error number */
	if(error < 0)
	{
		std::cerr << "IterateOverDirectory(m, " << commit_path << ".zip)\n" << "returned: " << error << "\n";
		return error;
	}

	std::cout << "Writing Nexus File to: \n" << commit_path << filename << "\n";
	error = m.NexusOut(commit_path, filename);

	/* return if error writing NEXUS file, print error number */
	if (error < 0)
	{
		std::cerr << "m.NexusOut(" << commit_path << ", " << filename << ")\n" << "returned: " << error << "\n";
		return error;
	}

	return 0;
}


int CompareCommits::CommitCompareAllGit(
		std::string filename,
		std::string repo_name,
		std::string repo_path,
		std::string commit_path)
{
	unsigned long long reposize = 0;
	int error = 0;

	/* init libgit */
	error = git_libgit2_init();

	/* return if error initialising repo, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* -- open repo -- */
	const char *REPO_PATH = repo_path.c_str();
	git_repository *repo = nullptr;
	error = git_repository_open(&repo, REPO_PATH);

	/* return if error opening repo, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* -- create revision walker -- */
	git_revwalk *walker = nullptr;
	error = git_revwalk_new(&walker, repo);

	/* return if error allocating new revision walker, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* sort by chronological order */
	error = git_revwalk_sorting(walker, GIT_SORT_NONE);

	/* return if error changing sorting mode, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* start from HEAD */
	error = git_revwalk_push_head(walker);

	/* return if error pushing head to walker, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* -- walk the walk -- */
	git_oid oid;

	/* itterate over each commit */
	while (!git_revwalk_next(&oid, walker))
	{
		reposize++;
	}

	/* -- clean up -- */
	git_revwalk_free(walker);
	git_repository_free(repo);

	error = git_libgit2_shutdown();

	/* return if error shutting down, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	// TODO check if this is still needed
	if (reposize == 0)
	{
		std::cerr << "Number of commits to compare is 0 \n";
		return -1;
	}

	SimilarityMatrix m = SimilarityMatrix(reposize);

	m = IterateOverDirectory(m, commit_path, ".zip", repo_name);

	std::cout << "Writing NEXUS File to: \n" << commit_path << filename << "\n";
	error = m.NexusOut(commit_path, filename);

	/* return if error writing NEXUS file, print error number */
	if (error < 0)
	{
		std::cerr << "m.NexusOut(" << commit_path << ", " << filename << ")\n" << "returned: " << error << "\n";
		return error;
	}

	return 0;
}


int CompareCommits::CommitCompareVector(
		std::string filename,
		std::string repo_name,
		std::string commit_path,
		std::vector<std::string>& commits)
{
	if (commits.size() == 0)
	{
		std::cerr << "Number of commits to compare is 0 \n";
		return -1;
	}

	SimilarityMatrix m = SimilarityMatrix(commits.size());
	int error = 0;

	/* if path exists */
	fs::path startpath(commit_path);
	if (exists(startpath))
	{
		std::string commit1_path;
		std::string commit2_path;
		std::string commit1_hash;
		std::string commit2_hash;
		fs::path file;
		fs::path previousfile;
		double similarity = -1;

		/* for each item in vector compare with all other items in vector */
		for (auto &commit1 : commits)
		{
			for (auto &commit2 : commits)
			{
				/* build filepaths */
				commit1_path = commit_path + "/" + commit1 + ".zip";
				commit2_path = commit_path + "/" + commit2 + ".zip";
				previousfile.assign(commit1_path);
				file.assign(commit2_path);

				/* if both paths exist and file extensions are zip */
				bool fileIsZip = file.extension().string().compare(".zip") == 0;
				bool previousfileIsZip = previousfile.extension().string().compare(".zip") == 0;
				if (exists(file) && exists(previousfile) && fileIsZip && previousfileIsZip)
				{
					commit1_hash = previousfile.stem().string();
					commit2_hash = file.stem().string();

					similarity = this->CompareTwoCommits(
							commit_path,
							previousfile.string(),
							file.string(),
							commit1_hash,
							commit2_hash,
							repo_name);

					/* return if error comparing commits, print error number */
					if (similarity<0)
					{
						std::cerr << "CompareTwoCommits("
								<< commit_path << ", "
								<< previousfile.string() << ", "
								<< file.string() << ", "
								<< commit1_hash << ", "
								<< commit2_hash << ", "
								<< repo_name << ")\n" 
								<< "Error: " << similarity << "\n";
						return static_cast<int>(similarity);
					}

					error = m.add(commit1_hash, commit2_hash, similarity);

					/* return if error adding to SimilarityMatrix, print error number */
					if (error < 0)
					{
						std::cerr << "m.add("
								<< commit1_hash << ", "
								<< commit2_hash << ", "
								<< similarity << ")\n" 
								<< "returned: " << error << "\n";
						return error;
					}
				}
			}
		}
		std::cout << "Writing NEXUS File to: \n" << commit_path << filename << "\n";
		error = m.NexusOut(commit_path, filename);

		/* return if error writing NEXUS file, print error number */
		if (error < 0)
		{
			std::cerr << "m.NexusOut(" << commit_path << ", " << filename << ")\n" << "returned: " << error << "\n";
			return error;
		}
	}
	else
	{
		std::cerr << "PATH INVALID: " << commit_path << "\n";
		return -1;
	}

	return 0;
}


int CompareCommits::CommitCompareStartEnd(
		std::string filename,
		std::string repo_name,
		std::string repo_path,
		std::string commit_path,
		std::string start_commit_hash,
		std::string end_commit_hash)
{
	std::string commit_hash;
	std::vector<std::string> commits;
	int error = 0;

	/* init libgit */
	error = git_libgit2_init();

	/* return if error initialising repo, print error */
	//https://libgit2.org/docs/guides/101-samples/
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* -- open repo -- */
	const char *REPO_PATH = repo_path.c_str();
	git_repository *repo = nullptr;
	error = git_repository_open(&repo, REPO_PATH);

	/* return if error opening repo, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* -- create revision walker -- */
	git_revwalk *walker = nullptr;
	error = git_revwalk_new(&walker, repo);

	/* return if error allocating new revision walker, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* sort by chronological order */
	error = git_revwalk_sorting(walker, GIT_SORT_NONE);

	/* return if error changing sorting mode, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* set range for walker */
	if (start_commit_hash == "" && end_commit_hash == "")
	{
		/* start from HEAD */
		std::cout << "range: All \n";
		error = git_revwalk_push_head(walker);
	}
	else
	{
		std::string range = start_commit_hash + ".." + end_commit_hash;
		std::cout << "range: " << range << "\n";
		error = git_revwalk_push_range(walker, range.c_str());
	}

	/* return if error pushing range to walker, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* -- walk the walk -- */
	git_oid oid;

	while (!git_revwalk_next(&oid, walker))
	{
		/* -- get the current commit -- */
		git_commit *commit = nullptr;
		error = git_commit_lookup(&commit, repo, &oid);

		/* return if error looking up commit, print error */
		if (error < 0)
		{
			PrintGitErrorDetails(git_error_last(), error);
			return error;
		}

		/* add commit */
		commit_hash = git_oid_tostr_s(&oid);
		commits.push_back(commit_hash);

		/* free the commit */
		git_commit_free(commit);
	}
	/* -- clean up -- */
	git_revwalk_free(walker);
	git_repository_free(repo);

	error = git_libgit2_shutdown();

	/* return if error shutting down, print error */
	if (error < 0)
	{
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* call CommitCompareVector now commits vector has been created */
	error = CommitCompareVector(filename, repo_name, commit_path, commits);

	/* return if error in CommitCompareVector, print error number */
	if (error < 0)
	{
		std::cerr << "CommitCompareVector("
				<< filename << ", "
				<< repo_name << ", "
				<< commit_path << ", "
				<< "commits" << ")\n"
				<< "Error " << error << "\n";
		return error;
	}


	return 0;
}


void CompareCommits::PrintGitErrorDetails(const git_error* e, int error_code) const
{
	std::cerr << "Error " << error_code << "/" << e->klass << ": " << e->message << "\n";
}