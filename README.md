# ATLAS
ROOT and Bash code I wrote for my undergraduate research

dmc   = Data/Monte Carlo

hist  = histograms

ttbar = top/anti-top quarks created in one particle collision event


ttbar.txt is an example of one of the text files containing the paths to input files. The text files were read by the plotting program to easily access the input files since there was a very large number of them and multiple types.

All input files had the same branches that needed to be accessed in order to add the data to histograms, so TreeConnector.h was created to easily initialize all these connections for every input file. If I remember correctly, files like TreeConnector.h can be created easily by using TTree::MakeClass in ROOT and copying the important information you need from the class that gets generated.
