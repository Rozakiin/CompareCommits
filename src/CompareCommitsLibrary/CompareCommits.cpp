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

int CompareCommits::IterateOverDirectory(SimilarityMatrix& m, std::string commit_path, std::string file_extension, std::string repo_name="")
{
	std::string commit1_filename;
	std::string commit2_filename;
	fs::path file;
	fs::path previousfile;
	double similarity;
	std::string extension = ".zip";
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
				/* if both paths exist and file extensions are zip */
				if (exists(file)
						&& exists(previousfile)
						&& extension.compare(file.extension().string()) == 0
						&& extension.compare(previousfile.extension().string()) == 0)
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
					if (similarity<0)
					{
						std::cerr << "CompareTwoCommits("
								<< commit_path << ", "
								<< previousfile.string() << ", "
								<< file.string() << ", "
								<< commit1_filename << ", "
								<< commit2_filename << ", "
								<< repo_name << ")\n";
						std::cerr << "Error " << similarity << "\n";
						return static_cast<int>(similarity);
					}

					error = m.add(commit1_hash, commit2_hash, similarity);

					/* return if error adding to SimilarityMatrix, print error number */
					if (error < 0)
					{
						std::cerr << "m.add("
								<< commit1_hash << ", "
								<< commit2_hash << ", "
								<< similarity << ")\n";
						std::cerr << "returned: " << error << "\n";
						return error;
					}
				}
			}
		}
	}
}
int CompareCommits::CommitCompareAllZip(
		std::string filename,
		std::string commit_path)
{
	unsigned long long reposize = 0;
	int error = 0;

	fs::path startpath(commit_path);
	/* If path exists */
	if (exists(startpath))
	{
		std::string extension = ".zip";

		/* Iterate over directory, count number of files with extension zip */
		for (auto &dirEntry : fs::directory_iterator(commit_path))
		{
			/* If file extension has extension zip */
			if (extension.compare(dirEntry.path().extension().string()) == 0)
			{
				reposize++;
			}
		}

		if (reposize <= 0)
		{
			std::cerr << "Number of commits to compare is 0 or less \n";
			return -1;
		}

		SimilarityMatrix m = SimilarityMatrix(reposize);
		std::string commit1_filename;
		std::string commit2_filename;
		fs::path file;
		fs::path previousfile;
		double similarity = -1;

		/* Iterate over directory */
		for (auto &dirEntry : fs::directory_iterator(commit_path))
		{
			file = dirEntry.path();
			/* If file path not empty then accessed a file already */
			if (!file.empty())
			{
				/* store previous file */
				previousfile = file;
				/* Iterate over all files in directory for comparison */
				for (auto &dirEntry2 : fs::directory_iterator(commit_path))
				{
					file = dirEntry2.path();
					/* If both paths exist and file extensions are zip */
					if (exists(file)
							&& exists(previousfile)
							&& extension.compare(file.extension().string()) == 0
							&& extension.compare(previousfile.extension().string()) == 0)
					{
						commit1_filename = previousfile.stem().string();
						commit2_filename = file.stem().string();

						similarity = this->CompareTwoCommits(
								commit_path,
								previousfile.string(),
								file.string(),
								commit1_filename,
								commit2_filename);

						/* return if error comparing commits, print error number */
						if (similarity<0)
						{
							std::cerr << "CompareTwoCommits(" 
									<< commit_path << ", "
									<< previousfile.string() << ", "
									<< file.string() << ", "
									<< commit1_filename << ", "
									<< commit2_filename << ")\n";
							std::cerr << "Error " << similarity << "\n";
							return static_cast<int>(similarity);
						}

						error = m.add(commit1_filename,	commit2_filename, similarity);
						/* return if error adding to SimilarityMatrix, print error number */
						if (error < 0)
						{
							std::cerr << "m.add(" << commit1_filename << ", "
									<< commit2_filename << ", "
									<< similarity << ")\n";
							std::cerr << "returned: " << error << "\n";
							return error;
						}
					}
				}
			}
		}
		std::cout << "Writing Nexus File to: \n";
		std::cout << commit_path << filename << "\n";
		error = m.NexusOut(commit_path, filename);

		/* return if error writing NEXUS file, print error number */
		if (error < 0)
		{
			std::cerr << "m.NexusOut(" << commit_path << ", " << filename << ")\n";
			std::cerr << "returned: " << error << "\n";
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
		// const git_error *e = git_error_last();
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
		// const git_error *e = git_error_last();
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* -- create revision walker -- */
	git_revwalk *walker = nullptr;
	error = git_revwalk_new(&walker, repo);

	/* return if error allocating new revision walker, print error */
	if (error < 0)
	{
		// const git_error *e = git_error_last();
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* sort by chronological order */
	error = git_revwalk_sorting(walker, GIT_SORT_NONE);

	/* return if error changing sorting mode, print error */
	if (error < 0)
	{
		// const git_error *e = git_error_last();
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* start from HEAD */
	error = git_revwalk_push_head(walker);

	/* return if error pushing head to walker, print error */
	if (error < 0)
	{
		// const git_error *e = git_error_last();
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
		// const git_error *e = git_error_last();
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	if (reposize <= 0)
	{
		std::cerr << "Number of commits to compare is 0 or less \n";
		return -1;
	}



	fs::path startpath(commit_path);
	/* if path exists */
	if (exists(startpath))
	{
		SimilarityMatrix m = SimilarityMatrix(reposize);
		std::string commit1_hash;
		std::string commit2_hash;
		fs::path file;
		fs::path previousfile;
		double similarity = -1;
		std::string extension = ".zip";

		IterateOverDirectory(m, commit_path, extension, repo_name);
		/* Iterate over directory */
		// for (auto &dirEntry : fs::directory_iterator(commit_path))
		// {
		// 	file = dirEntry.path();
		// 	/* if file path not empty then accessed a file already */
		// 	if (!file.empty())
		// 	{
		// 		/* store previous file */
		// 		previousfile = file;
		// 		/* Iterate over all files in directory for comparison */
		// 		for (auto &dirEntry2 : fs::directory_iterator(commit_path))
		// 		{
		// 			file = dirEntry2.path();
		// 			/* if both paths exist and file extensions are zip */
		// 			if (exists(file)
		// 					&& exists(previousfile)
		// 					&& extension.compare(file.extension().string()) == 0
		// 					&& extension.compare(previousfile.extension().string()) == 0)
		// 			{
		// 				commit1_hash = previousfile.stem().string();
		// 				commit2_hash = file.stem().string();

		// 				similarity = this->CompareTwoCommits(
		// 						commit_path,
		// 						previousfile.string(),
		// 						file.string(),
		// 						commit1_hash,
		// 						commit2_hash,
		// 						repo_name);

		// 				/* return if error comparing commits, print error number */
		// 				if (similarity<0)
		// 				{
		// 					std::cerr << "CompareTwoCommits("
		// 							<< repo_name << ", "
		// 							<< commit_path << ", "
		// 							<< previousfile.string() << ", "
		// 							<< file.string() << ", "
		// 							<< commit1_hash << ", "
		// 							<< commit2_hash << ")\n";
		// 					std::cerr << "Error " << similarity << "\n";
		// 					return static_cast<int>(similarity);
		// 				}

		// 				error = m.add(commit1_hash, commit2_hash, similarity);

		// 				/* return if error adding to SimilarityMatrix, print error number */
		// 				if (error < 0)
		// 				{
		// 					std::cerr << "m.add("
		// 							<< commit1_hash << ", "
		// 							<< commit2_hash << ", "
		// 							<< similarity << ")\n";
		// 					std::cerr << "returned: " << error << "\n";
		// 					return error;
		// 				}
		// 			}
		// 		}
		// 	}
		// }
		std::cout << "Writing NEXUS File to: \n";
		std::cout << commit_path << filename << "\n";
		error = m.NexusOut(commit_path, filename);

		/* return if error writing NEXUS file, print error number */
		if (error < 0)
		{
			std::cerr << "m.NexusOut(" << commit_path << ", " << filename << ")\n";
			std::cerr << "returned: " << error << "\n";
			return error;
		}
	}
	else
	{
		std::cerr << "PATH INVALID: " + commit_path << "\n";
		return -1;
	}

	return 0;
}


int CompareCommits::CommitCompareVector(
		std::string filename,
		std::string repo_name,
		std::string commit_path,
		std::vector<std::string>& commits)
{
	std::string commit1_path;
	std::string commit2_path;
	std::string commit1_hash;
	std::string commit2_hash;
	int error = 0;

	if (commits.size() <= 0)
	{
		std::cerr << "Number of commits to compare is 0 or less \n";
		return -1;
	}

	SimilarityMatrix m = SimilarityMatrix(commits.size());

	fs::path startpath(commit_path);
	/* if path exists */
	if (exists(startpath))
	{
		fs::path file;
		fs::path previousfile;
		double similarity = -1;
		std::string extension = ".zip";
		/* for each item in vector compare with all other items in vector */
		for (auto &commit1 : commits)
		{
			for (auto &commit2 : commits)
			{
				commit1_path = commit_path + "/" + commit1 + extension;
				commit2_path = commit_path + "/" + commit2 + extension;
				previousfile.assign(commit1_path);
				file.assign(commit2_path);

				/* if both paths exist and file extensions are zip */
				if (exists(file)
						&& exists(previousfile)
						&& extension.compare(file.extension().string()) == 0
						&& extension.compare(previousfile.extension().string()) == 0)
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
								<< repo_name << ", "
								<< commit_path << ", "
								<< previousfile.string() << ", "
								<< file.string() << ", "
								<< commit1_hash << ", "
								<< commit2_hash << ")\n";
						std::cerr << "Error: " << similarity << "\n";
						return static_cast<int>(similarity);
					}

					error = m.add(commit1_hash, commit2_hash, similarity);

					/* return if error adding to SimilarityMatrix, print error number */
					if (error < 0)
					{
						std::cerr << "m.add("
								<< commit1_hash << ", "
								<< commit2_hash << ", "
								<< similarity << ")\n";
						std::cerr << "returned: " << error << "\n";
						return error;
					}
				}
			}
		}
		std::cout << "Writing NEXUS File to: \n";
		std::cout << commit_path << filename << "\n";
		error = m.NexusOut(commit_path, filename);

		/* return if error writing NEXUS file, print error number */
		if (error < 0)
		{
			std::cerr << "m.NexusOut(" << commit_path << ", " << filename << ")\n";
			std::cerr << "returned: " << error << "\n";
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
	std::string commit1_path;
	std::string commit2_path;
	std::string commit1_hash;
	std::string commit2_hash;
	std::string commit_hash;
	std::vector<std::string> commits;
	int error = 0;

	/* init libgit */
	error = git_libgit2_init();

	/* return if error initialising repo, print error */
	//https://libgit2.org/docs/guides/101-samples/
	if (error < 0)
	{
		// const git_error *e = git_error_last();
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
		// const git_error *e = git_error_last();
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* -- create revision walker -- */
	git_revwalk *walker = nullptr;
	error = git_revwalk_new(&walker, repo);

	/* return if error allocating new revision walker, print error */
	if (error < 0)
	{
		// const git_error *e = git_error_last();
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* sort by chronological order */
	error = git_revwalk_sorting(walker, GIT_SORT_NONE);

	/* return if error changing sorting mode, print error */
	if (error < 0)
	{
		// const git_error *e = git_error_last();
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
		// const git_error *e = git_error_last();
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
			// const git_error *e = git_error_last();
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
		// const git_error *e = git_error_last();
		PrintGitErrorDetails(git_error_last(), error);
		return error;
	}

	/* call CommitCompareVector now commits vector has been created */
	error = CommitCompareVector(filename, repo_name, commit_path, commits);

	/* return if error in CommitCompareVector, print error number */
	if (error < 0)
	{
		std::cerr << "Error " << error << "\n";
		return error;
	}


	return 0;
}

void CompareCommits::PrintGitErrorDetails(const git_error* e, int error_code) const
{
	std::cerr << "Error " << error_code << "/" << e->klass << ": " << e->message << "\n";
}