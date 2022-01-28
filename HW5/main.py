import argparse
import numpy as np
import math
from numpy import unravel_index
import pandas as pd

import re
MATCH = 0 
MISMATCH = 0
GAP = 0
profile_matrix = np.zeros([0])

def profile_matrix_calc(aligned_sequences):
    aln_len = len(aligned_sequences[0])
    global profile_matrix
    profile_matrix = np.zeros([5, aln_len])
    for j, line in enumerate(aligned_sequences):
        for i, element in enumerate(line):
            if element == 'A':
                profile_matrix[0][i] += 1
            elif element == 'C':
                profile_matrix[1][i] += 1
            elif element == 'G':
                profile_matrix[2][i] += 1
            elif element == 'T':
                profile_matrix[3][i] += 1
            elif element == '-':
                profile_matrix[4][i] += 1
    profile_matrix /= 5
    return profile_matrix   

def x_with_j(x, column_j):
    sum = 0
    for j, element in enumerate("ACGT"):
        if element == x:
            sum += MATCH * profile_matrix[j][column_j]
        else:
            sum += MISMATCH * profile_matrix[j][column_j]
    # add gap penalty at the end
    sum += GAP * profile_matrix[4][column_j]
    return(sum)

def _score(x, y):
    if x == y:
        return MATCH
    else:
        if x or y == '-':
            return GAP
        else:
            return MISMATCH

def sequence_to_profile(sequence, aligned_sequences):
    #pm = profile_matrix_calc(aligned_sequences)
    profile_matrix_calc(aligned_sequences)
    dim_j = len(aligned_sequences[0]) + 1
    dim_i = len(sequence) + 1
    V = np.zeros([dim_i, dim_j])
    # J YATAY, I DİKEY
    for j in range(1,dim_j):
        V[0][j] = j * GAP
    for i in range(1, dim_i):
        V[i][0] = i * GAP
    for j in range(1,dim_j):
        for i in range(1, dim_i):
            V[i][j] = max(
                V[i-1][j-1] + x_with_j(sequence[i-1], j - 1),
                V[i-1][j] + _score(sequence[i-1], '-'),
                V[i,j-1] + x_with_j('-', j - 1) 
            )
    # df = pd.DataFrame(V)
    # filepath = "my_excel.xlsx"
    # df.to_excel(filepath, index=False)
    # dpf = pd.DataFrame(profile_matrix)
    # dpf.to_excel("profile_matrix_excel.xlsx", index=False )
    aligned = ""
    tj = len(aligned_sequences[0])
    ti = len(sequence)
    while ti > 0 or tj > 0:
        if ti > 0 and tj > 0 and V[ti][tj] == (V[ti-1][tj-1] + x_with_j(sequence[ti-1], tj - 1)): # MATCH
            aligned += sequence[ti-1]
            ti -= 1
            tj -= 1
        elif tj > 0 and V[ti][tj] == V[ti][tj-1] + x_with_j('-', tj - 1):
            aligned += '-'
            tj -= 1
        elif ti > 0 and V[ti][tj] == V[ti-1][tj] + _score(sequence[ti-1], '-'):
            ti -= 1
    
    result = ''.join(reversed(aligned))
    aligned_sequences.append(result)
    profile_matrix_calc(aligned_sequences)
    print(''.join(reversed(aligned)))
    return ''.join(reversed(aligned))
    
def globs():
    print("match: ", MATCH, " mismatch: ", MISMATCH, " gap: ", GAP)

def main():
    parser = argparse.ArgumentParser(description='Sequence Alignment')
    parser.add_argument('-f', '--fasta', type=str, metavar='', required=True, help='Input Fasta File')
    parser.add_argument('-a', '--aln', type=str, metavar='', required=True, help='Aligned Sequences File')
    parser.add_argument('-o', '--out', type=str, metavar='', required=True, help='Output File')
    parser.add_argument('-g', '--gap', type=int, metavar='', required=True, help='Gap Penalty')
    parser.add_argument('-m', '--match', type=int, metavar='', required=True, help='Match Reward')
    parser.add_argument('-mm', '--mismatch', type=int, metavar='', required=True, help='Mismatch Penalty')
    args = parser.parse_args()
    global MATCH
    MATCH = args.match
    global MISMATCH
    MISMATCH = args.mismatch
    global GAP
    GAP = args.gap
    # READING FILES
    seq_file = open(args.fasta, 'r')
    seq_lines = seq_file.readlines()
    for line in seq_lines:
        if line[0] != '>':
            sequence = line.strip()
    # READING ALIGNED SEQUENCES
    aln_seq = open(args.aln, 'r')
    lines = aln_seq.readlines()
    aligned_sequences = []
    for line in lines:
        aligned_sequences.append(''.join(filter("ACGT-".__contains__, line)))

    if len(aligned_sequences[0]) > 500:
        print('invalid input')
        exit()
    # FUNCTION CALL
    globs()
    aligned = sequence_to_profile(sequence, aligned_sequences)
    file_object = open("seq.aln", "w")
    for i, element in enumerate(aligned_sequences):
        file_object.write('sequence' + str(i+1) + ' ' + element + '\n')
    file_object.write('sequence  ' + aligned)
main()