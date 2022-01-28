import argparse
import numpy as np
import math
from numpy import unravel_index
import re
from itertools import combinations
MATCH  = 0
MISMATCH = 0
GAP = 0
gapopen = 0
gapext = 0

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
    return aligned_sequences

def backtrace(sequence_1, sequence_2, E, F, G):
    aligned_1 = ""
    aligned_2 = ""
    i = len(sequence_2)
    j = len(sequence_1)
    while (i > 0 or j > 0):
        if (i > 0 and j > 0 and G[i][j] == G[i-1][j-1] + score(sequence_1, sequence_2, i, j)):
            aligned_1 += sequence_1[j-1]
            aligned_2 += sequence_2[i-1]
            i = i - 1
            j = j - 1
        elif (i > 0 and G[i][j] == F[i][j]):
            aligned_1 += '-'
            aligned_2 += sequence_2[i-1]
            i = i - 1 
        elif (j > 0 and G[i][j] == E[i][j]):
            aligned_1 += sequence_1[j-1]
            aligned_2 += '-'
            j = j - 1
    return ''.join(reversed(aligned_1)), ''.join(reversed(aligned_2))

def score(sequence_1, sequence_2, i, j):  # x is sequence_1[i] and y is sequence_2[j]
    if sequence_2[i-1] == sequence_1[j-1]:
        return MATCH 
    else:
        return MISMATCH
    
def initialize_E(i, j):
    if i > 0 and j == 0:
        return -math.inf
    else:
        if j > 0:
            return gapopen + (gapext * j)
        else:
            return 0

def initialize_F(i,j):
    if j > 0 and i == 0:
        return -math.inf
    else:
        if i > 0:
            return gapopen + (gapext * i)
        else:
            return 0

def initialize_G(i,j):
    if j == 0 and i == 0:
        return 0
    else:
        if j == 0 or i == 0:
            return -math.inf
        else:
            return 0

def global_affine(sequence_1, sequence_2):
    dim_i = len(sequence_2) + 1
    dim_j = len(sequence_1) + 1
    E = [[initialize_E(i,j) for j in range(0, dim_j)] for i in range(0, dim_i)]
    F = [[initialize_F(i,j) for j in range(0, dim_j)] for i in range(0, dim_i)]
    G = [[initialize_G(i, j) for j in range(0, dim_j)] for i in range(0, dim_i)]

    for j in range(1, dim_j):
        for i in range(1, dim_i):
            E[i][j] = max((gapopen + gapext + G[i][j-1]), (gapopen + E[i][j-1]), (gapopen + gapext + F[i][j-1]))
            F[i][j] = max((gapopen + gapext + G[i-1][j]), (gapopen + gapext + E[i-1][j]), (gapext + F[i-1][j]))
            G[i][j] = max(score(sequence_1, sequence_2, i, j) + G[i-1][j-1], E[i][j], F[i][j])

    aligned_1, aligned_2 = backtrace(sequence_1, sequence_2, E,F,G)
    profile = []
    profile.append(aligned_1)
    profile.append(aligned_2)
    return profile


def hammington_score(sequence_a, sequence_b):
    score = 0
    for a, b in zip(sequence_a, sequence_b):
        if a != b:
            score += 1
        # else:
        #     score -= 1
    return score

def find_closest_tuples(sequences):
    possib = math.comb(len(sequences), 2)
    #print("possibles ",possib)    
    comb = combinations( [ x for x in range(len(sequences))] , 2)
    biggest = -math.inf
    for index, i in enumerate(list(comb)):
        score = hammington_score(sequences[i[0]], sequences[i[1]])
        if score > biggest:
            biggest = score
            max_tup = i
    return max_tup

def combine(profile):
    result = ""
    print(profile[0])
    print(profile[1])
    for char_a, char_b in zip(profile[0], profile[1]):
        if char_a == char_b:
            result += char_a
        elif char_a == '-' or char_b == '-':
            if char_a != '-':
                result += char_a
            else:
                result += char_b
        else:
            temp = ""
            temp += char_a.lower()
            temp += "/".lower()
            temp += char_b.lower()
            result += temp
    return result

def upgma(seq_dict):
    profile = []
    for d in seq_dict:
        profile.append(seq_dict[d])
    print(profile)   

    dim = len(profile)
    distance_matrix = np.zeros((dim,dim))
    
    comb = combinations( [ x for x in range(len(profile))] , 2)
    for i in range(0, dim - 1):
        for j in range(1 , dim):
            distance_matrix[i][j] = hammington_score( profile[i], profile[j])
    distance_matrix = np.triu(distance_matrix, k = 0)
    print("differences between sequences")
    print(distance_matrix)
    non_zero = np.nonzero(distance_matrix)
    print(non_zero)
    min_index = unravel_index(distance_matrix.argmin(), distance_matrix.shape)
    print(min_index)



def align(seq_dict):
    s1 = "GATTCA"
    s2 = "GTCTGA"
    s3 = "GATATT"
    s4 = "GTCAGC"
    
    print(seq_dict)
    seq_list = []
    for d in seq_dict:
        seq_list.append(seq_dict[d])
    #print(seq_list)
    closest_tuples = find_closest_tuples(seq_list)   
    index_a = closest_tuples[0]
    index_b = closest_tuples[1]
    result_list = []
    profile = global_affine(seq_list[closest_tuples[0]], seq_list[closest_tuples[1]])    
    for i, elem in enumerate(seq_list):
        if i != index_a and i != index_b:
            result_list.append(elem)
    seq_list = result_list
    for element in seq_list:
        aligned_seq = sequence_to_profile(element, profile)
    print("After Sequence to Profile Alignment ", profile)

    dim = len(profile)
    distance_matrix = np.zeros((dim,dim))
    
    comb = combinations( [ x for x in range(len(profile))] , 2)
    for i in range(0, dim - 1):
        for j in range(1 , dim):
            distance_matrix[i][j] = len(profile[0]) - hammington_score( profile[i], profile[j])
    print("differences between sequences")
    print(distance_matrix)
    
def globs():
    print('utku')
    print(match)
    print(mismatch)
    print(gapopen)
    print(gapext)

def readfile(filename):
    # READING FILES
    seq_file = open(filename, 'r')
    seq_lines = seq_file.readlines()
    sequences_labels = []
    sequences = []
    for line in seq_lines:
        if line[0] == '>':
            sequences_labels.append(line[1].strip())
        else:
            sequences.append(line.strip())
    Dict = {}
    for seq, label in zip(sequences, sequences_labels):
        Dict[label] = seq
    return Dict
    
def main():
    parser = argparse.ArgumentParser(description='Sequence Alignment')
    parser.add_argument('-f', '--fasta', type=str, metavar='', required=True, help='Input Fasta File')
    parser.add_argument('-m', '--match', type=int, metavar='', required=True, help='Match Reward')
    parser.add_argument('-mm', '--mismatch', type=int, metavar='', required=True, help='Mismatch Penalty')
    parser.add_argument('-go', '--gapopen', type=int, metavar='', required=True, help='Gap Opening Penalty')
    parser.add_argument('-ge', '--gapext', type=int, metavar='', required=True, help='Gap Extension Penalty')
    parser.add_argument('-o', '--out', type=str, metavar='', required=True, help='Output File')
    args = parser.parse_args()
    global MATCH, MISMATCH, gapopen, gapext, GAP
    MATCH = args.match
    MISMATCH = args.mismatch
    gapopen = args.gapopen
    gapext = args.gapext
    GAP = args.gapopen
    #globs()
    seq_dict = readfile(args.fasta)
    #for d in seq_dict:
        #print(d, "    ", seq_dict[d])
    #align(seq_dict)
    upgma(seq_dict)
main()
