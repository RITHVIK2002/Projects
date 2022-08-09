input_params.txt
threads vertices
adjacecy matrix
ex:
2 5
  1 2 3 4 5
1 0 1 0 1 0
2 1 0 0 1 0
3 0 0 0 0 1
4 0 1 0 1 0
5 1 1 1 0 1


execution each file:
	g++ SrcAssgn5_fined_cs19btech11038.cpp -pthread
	./a.out
	g++ SrcAssgn5_Coarse_cs19btech11038.cpp -pthread
	./a.out
Output:
Stats: output_fined|coasre_stats.txt
Results:Output_fined|coasre_results.txt

