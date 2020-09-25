# F2S2 - Fast Forensic Similarity Search Strategy
An implementation of F2S2 similarity search strategy.

This is a proof-of-concept based on the article:
Christian Winter, Markus Schneider, York Yannikos,
F2S2: Fast forensic similarity search through indexing piecewise hash signatures,
Digital Investigation,
Volume 10, Issue 4,
2013,
Pages 361-371,
ISSN 1742-2876,
https://doi.org/10.1016/j.diin.2013.08.003.

It may differ at some points from the original ideia.

# Instructions
F2S2 uses pre-calculated ssdeep's digests that it generates from both datasets: The known and the target datasets. The known dataset digests must be in a file named "cb_known_set.txt", and the target digests must be in a file called "cb_target_set.txt".

## Compiling

To compile run:
```
make
```
## Execute

To execute run:
```
./F2S2
```
And choose among the options:

1.Insert the digests into the index table.

2.Search for the target database into de index table.

## Ouput

The output is a list of similar files. The tool only indicates possible matches, so to be sure, run a comparison using ssdeep algorithm. Future work will implement this functionality.

## Notes

The search results are stored at file "results.txt"

To save RAM, the known output are just the known-file's IDs. The filename can be obtained from the file "iDvsFile.txt".

The parameters like n-gram size, e-key size, output file name etc. can be set in file "config.h".
