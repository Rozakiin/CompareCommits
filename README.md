CompareCommits
========

CompareCommits produces distance matricies in NEXUS file format from commits given by the user.

Look how easy it is to use:

```shell
$ ./comparecommits -r './mineserver' mineserver
```

Features
--------
- Compare all commits of given repository
- Compare all zips in a given directory
- Compare commits given in a text file of given repository
- Compare commits between the start and end commit of a given repository
- Compare commits of a given vector of commit hashes

Installation
------------

## Dependencies
- [git](https://git-scm.com/) (tested with version 2.17.1)
- [libgit2](https://github.com/libgit2/libgit2) (tested with version 1.0.0)

## Installation

### Windows 10
1. Clone this repository
2. Install libgit2 ([good tutorial](https://bvisness.me/libgit2/))
3. Link libgit2 to project
4. Compile CompareCommits using c++11 or greater
5. Verify CompareCommits was correctly installed by checking version
```shell
$ ./Code/CompareCommits/Debug/comparecommits.exe --version

```

1. Clone this repository
2. cd to lib/libgit2 install
3. cd to lib/boost install
31. cd lib/boost/tools/build
32. ./bootstrap.bat 
33. ./b2 --prefix=../../ install
34. cd to lib/boost 
35. ./b2
4. build using CMake TODO
5. Verify CompareCommits was correctly installed by checking version
```shell
$ ./src/CompareCommits/Debug/comparecommits.exe --version

```



### Linux/Ubuntu
1. Clone this repository
2. Install libgit2 ([good tutorial](https://bvisness.me/libgit2/))
3. Link libgit2 to project
4. Compile CompareCommits using c++11 or greater
5. Verify CompareCommits was correctly installed by checking version
```shell
$ ./Code/CompareCommits/Debug/comparecommits.exe --version
```

### Linux Install Notes
- boost/program_options needs to be built
- build and install to dependents/install removing .so extension libs helped 
- linking needed ssl, pthreads and crypto from path /usr/lib/x86_64-linux-gnu
- adding Xlinker --verbose is useful for debuging where the library is found/not

Contribute
----------

- Issue Tracker: github.com/rozakiin/CompareCommits/issues
- Source Code: github.com/rozakiin/CompareCommits

Usage
-----

### Arguments
- -r --repo repository local path
- -n --name repository name
- -s --start start commit hash
- -e --end end commit hash
- -c --commits path to directory containing the zipped commits
- -f --file path to file conatining list of commits to compare
- -o --output output file name
- --help prints usage information
- --version prints version of program
- The output file name can be without argument

### Methods(advanced)
#### CommitCompareAllZip
Compares all zips in given folder, gets two commits and passes to CompareTwoCommits for calculation, adds result to matrix.
##### Parameters
- filename: full path to the directory containing the zipped commits
- commit_path: full path to commit directory

##### Return
Returns 0 if success or an error code.


#### CommitCompareAllGit
Compares all commits, gets two commits and passes to CompareTwoCommits for calculation, adds result to matrix.
##### Parameters
- filename: full path to the directory containing the zipped commits
- repo_path: full path to repository directory
- commit_path: full path to commit directory

##### Return
Returns 0 if success or an error code.


#### CommitCompareVector
Compares all commits in vector, gets two commits and passes to CompareTwoCommits for calculation, adds result to matrix.
##### Parameters
- filename: full path to the directory containing the zipped commits
- repo_name: name given to repository on GitHub
- commit_path: full path to commit directory
- commits: vector of strings of commits to compare

##### Return
Returns 0 if success or an error code.


#### CommitCompareStartEnd
Gets all commits between start and end commits and adds to vector, passes to CommitCompareVector.
##### Parameters
- filename: full path to the directory containing the zipped commits
- repo_path: full path to repository directory
- commit_path: full path to commit directory
- start_commit_hash: long hash value for the start commit
- end_commit_hash: long hash value for the end commit

##### Return
Returns 0 if success or an error code.


#### CompareTwoCommits
Virtual Function to be used to calculate the difference metric between two zip files.
Override this method to change how two commits compute the difference metric.
##### Parameters
- commit_path: full path to the directory containing the zipped commits
- commit1_path: full path to first commit zip
- commit2_path: full path to second commit zip
- commit1_filename: long hash value for the first commit or file name
- commit2_filename: long hash value for the second commit or file name
- repo_name: repository's name on GitHub or empty

##### Return
Returns difference metric between first and second commit or an error code



#### DiffTwoCommitsZip
Calculates the similarity between two zip files using linux diff
##### Parameters
- commit_path: full path to the directory containing the zipped commits
- commit1_path: full path to first commit zip
- commit2_path: full path to second commit zip
- commit1_filename: the long hash value for the first commit
- commit2_filename: the long hash value for the second commit

##### Return
Returns difference metric between first and second commit or an error code


#### DiffTwoCommitsGit
Calculates the difference between two git commits using linux diff
##### Parameters
- repo_name: repository's name on GitHub
- commit_path: full path to the directory containing the zipped commits
- commit1_path: full path to first commit zip
- commit2_path: full path to second commit zip
- commit1_hash: long hash value for the first commit
- commit2_hash: long hash value for the second commit

##### Return
Returns difference metric between first and second commit or an error code




Examples
--------
Create distance matrix named 'mineserver.nxs' comparing all commits in the [mineserver](https://github.com/fador/mineserver) repository

```shell
$ ./comparecommits -r './mineserver' -c './mineserver-commits' -n mineserver minserver
```
Create distance matrix named 'mineserver20c73671.nxs' comparing commits between 20c7 and 3671 in the [mineserver](https://github.com/fador/mineserver) repository

```shell
$ ./comparecommits -r './mineserver' -c './mineserver-commits' -n mineserver -s 20c7 -e 3671 mineserver20c73671
```
Create distance matrix named 'mineservertextfile.nxs' comparing commits listed in file 'commits.txt' from the [mineserver](https://github.com/fador/mineserver) repository

```shell
$ ./comparecommits -r './mineserver' -c './mineserver-commits' -n mineserver -t 'commits.txt' mineservertextfile
```
Create distance matrix named 'test.nxs' comparing all zips in the 'Test' directory

```shell
$ ./comparecommits -c './Test' test
```

Support
-------

If you are having issues, please let us know.
Please email: ruari.armstrong@uea.ac.uk

License
-------

The project is licensed under the GNU General Public License v3.0.
