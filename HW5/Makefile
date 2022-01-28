all:
	python3 main.py --fasta seq.fasta --aln aligned_sequences.aln --out seq.aln --gap -5 --match 3 --mismatch -2

test:
	python3 main.py --fasta test_sequence.fasta --aln test_sequences.aln --out seq.aln --gap -1 --match 2 --mismatch -1

clear:
	rm seq.aln dp_matrix.txt