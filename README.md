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
F2S2, uses pre-calculated ssdeep's digest, so before using it, generate the digests from both datasets, the known and the target. Storing the known's at files which MUST be listed at a file named "cb_known_set.txt", and the target's must be stored at a file name "cb_target_set.txt".

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

2.After inserting the digests into the index table, search for the target database into de index table.

## Ouput

The output is a list of similar files. The tool only indicates possible matches, so to be sure, run a comparison using ssdeep algorithm. Future work will implement this functionality.

## Notes

The search results are listed at "resultados.txt"

To save RAM, the known output are just the known-file's IDs, so, the relation filename vs iD are listed on the "iDvsFile.txt" file.

The parameters like the n-gram size, e-key size, output file name etc, can be changed at "config.h" file.
