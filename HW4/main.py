import argparse
import numpy as np
import math
from numpy import unravel_index

def file_len(fname):
    with open(fname) as f:
        for i, l in enumerate(f):
            pass
    return i + 1

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
    scoring_matrix = np.array([[2,-3,-3,-3], [-3,2,-3,-3], [-3,-3,2,-3], [-3,-3,-3,2]])
    if sequence_2[i-1] == sequence_1[j-1]:
        return 2
    else:
        return -3
    
def initialize_E(i, j, gap_open, gap_extension):
    if i > 0 and j == 0:
        return -math.inf
    else:
        if j > 0:
            return gap_open + (gap_extension * j)
        else:
            return 0

def initialize_F(i,j, gap_open, gap_extension):
    if j > 0 and i == 0:
        return -math.inf
    else:
        if i > 0:
            return gap_open + (gap_extension * i)
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


def global_affine(sequence_1, sequence_2, gap_open, gap_extension):
    dim_i = len(sequence_2) + 1
    dim_j = len(sequence_1) + 1
    E = [[initialize_E(i,j,gap_open,gap_extension) for j in range(0, dim_j)] for i in range(0, dim_i)]
    F = [[initialize_F(i,j,gap_open,gap_extension) for j in range(0, dim_j)] for i in range(0, dim_i)]
    G = [[initialize_G(i, j) for j in range(0, dim_j)] for i in range(0, dim_i)]

    for j in range(1, dim_j):
        for i in range(1, dim_i):
            E[i][j] = max((gap_open + gap_extension + G[i][j-1]), (gap_open + E[i][j-1]), (gap_open + gap_extension + F[i][j-1]))
            F[i][j] = max((gap_open + gap_extension + G[i-1][j]), (gap_open + gap_extension + E[i-1][j]), (gap_extension + F[i-1][j]))
            G[i][j] = max(score(sequence_1, sequence_2, i, j) + G[i-1][j-1], E[i][j], F[i][j])

    aligned_1, aligned_2 = backtrace(sequence_1, sequence_2, E,F,G)
    g_arr = np.array(G)    
    return aligned_1, aligned_2,g_arr[dim_i-1][dim_j-1] 

def local_affine(sequence_1, sequence_2, gap_open, gap_extension): 
    dim_i = len(sequence_2) + 1
    dim_j = len(sequence_1) + 1
    E = [[initialize_E(i,j,gap_open,gap_extension) for j in range(0, dim_j)] for i in range(0, dim_i)]
    F = [[initialize_F(i,j,gap_open,gap_extension) for j in range(0, dim_j)] for i in range(0, dim_i)]
    G = [[initialize_G(i, j) for j in range(0, dim_j)] for i in range(0, dim_i)]

    for j in range(1, dim_j):
        for i in range(1, dim_i):
            E[i][j] = max(0,(gap_open + gap_extension + G[i][j-1]), (gap_open + E[i][j-1]), (gap_open + gap_extension + F[i][j-1]))
            F[i][j] = max(0,(gap_open + gap_extension + G[i-1][j]), (gap_open + gap_extension + E[i-1][j]), (gap_extension + F[i-1][j]))
            G[i][j] = max(0, score(sequence_1, sequence_2, i, j) + G[i-1][j-1], E[i][j], F[i][j])

    V = np.zeros([dim_i, dim_j])
    for j in range(1,dim_j):
        for i in range(1,dim_i):
            V[i][j] = max(E[i][j], F[i][j], G[i][j])
    aligned_1, aligned_2 = backtrace(sequence_1, sequence_2, E,F,G)
    
    maxIndex = unravel_index(V.argmax(), V.shape)
    ti = maxIndex[0]
    tj = maxIndex[1]
    aligned = ""
    while ti > 0 and tj > 0:
        if(sequence_1[tj-1] == sequence_2[ti-1]):
            aligned = sequence_1[tj-1] + aligned
            ti = ti - 1
            tj = tj - 1
        else:
            break
    longest_sub = aligned
    return aligned_1, aligned_2, longest_sub


def global_naive(sequence_1, sequence_2, gap_penalty):

    dim_i = len(sequence_2) + 1
    dim_j = len(sequence_1) + 1
    M = np.zeros([dim_i, dim_j])

    match_check = np.zeros([dim_i, dim_j])
    for j in range(1, dim_j):
        for i in range(1,dim_i):
            if sequence_1[j-1] == sequence_2[i-1]:
                match_check[i][j] = 2
            else: 
                match_check[i][j] = - 3
    for j in range(1, dim_j):
        M[0][j] = j * gap_penalty
    for i in range(1, dim_i):
        M[i][0] = i * gap_penalty

    for j in range(1, dim_j):
        for i in range(1, dim_i):
            num_1 = -math.inf
            num_2 = -math.inf
            if sequence_1[j-1] == sequence_2[i-1]:
                num_1 = M[i-1][j-1] + 2
            elif sequence_1[j-1] != sequence_2[i-1]:
                num_2 = M[i-1][j-1] - 3
            M[i][j] = max( num_1, num_2, (M[i-1][j] + gap_penalty), (M[i][j-1] + gap_penalty))
    score_naive = M[dim_i - 1][dim_j - 1]    
    aligned_1 = ""
    aligned_2 = ""
    ti = len(sequence_2)
    tj = len(sequence_1)
    while ti > 0 or tj > 0 :
        if ti > 0 and tj > 0 and M[ti][tj] == (M[ti-1][tj-1] + match_check[ti][tj]) :
            aligned_1 += sequence_1[tj-1]
            aligned_2 += sequence_2[ti-1]
            ti = ti - 1
            tj = tj - 1
        elif tj > 0 and M[ti][tj] == (M[ti-1][tj] + gap_penalty):
            aligned_1 += '-'
            aligned_2 += sequence_2[ti-1]
            ti = ti - 1 
        else:
            aligned_1 += sequence_1[tj-1]
            aligned_2 += '-'
            tj = tj - 1
    
    return ''.join(reversed(aligned_1)),''.join(reversed(aligned_2)),score_naive


def local_naive(sequence_1, sequence_2, gap_penalty):
    dim_j = len(sequence_1) + 1 # 7 
    dim_i = len(sequence_2) + 1 # 8
    M = np.zeros([dim_i,dim_j])
    for j in range(1,dim_j):
        for i in range(1, dim_i):
            num_1 = -math.inf
            num_2 = -math.inf
            if sequence_1[j-1] == sequence_2[i-1]: # eğer matchse
                num_1 = M[i-1][j-1] + 2
            else:
                num_2 = M[i-1][j-1] - 3

            M[i][j] = max(  0, 
                            num_1,
                            num_2,
                            M[i-1][j] + gap_penalty,
                            M[i][j-1] + gap_penalty
                        )
    maxIndex = unravel_index(M.argmax(), M.shape)
    ti = maxIndex[0]
    tj = maxIndex[1]
    aligned = ""
    while ti > 0 and tj > 0:
        if(sequence_1[tj-1] == sequence_2[ti-1]):
            aligned = sequence_1[tj-1] + aligned
            ti = ti - 1
            tj = tj - 1
        else:
            break
    print('longest subsequence', aligned)
    return aligned, M[ti][tj]

def main():
    parser = argparse.ArgumentParser(description='Sequence Alignment')
    parser.add_argument('-m', '--mode', type=str, metavar='', required=True, help='Mode of Alignment')
    parser.add_argument('-i', '--input', type=str, metavar='', required=True, help='Input of Alignment')
    parser.add_argument('-g', '--gapopen', type=int, metavar='', required=True, help='Gap Opening Penalty of Alignment')
    parser.add_argument('-ge', '--gapext', type=int, metavar='', required=False, help='Gap Extension Penalty of Alignment')
    args = parser.parse_args()
    print(args.mode, args.input, args.gapopen, args.gapext)
    fasta_file = open(args.input, 'r')
    lines = fasta_file.readlines()
    sequences = []
    for line in lines:
        if line[0] != '>':
            sequences.append(line.strip())
    sequence_1 = ""
    sequence_2 = ""
    length_ = len(sequences)
    for i in range(int(length_ / 2)):
        sequence_1 += sequences[i]
    for i in range(int(length_ / 2), length_):
        sequence_2 += sequences[i]
    sequence_1 = sequences[0] + sequences[1]
    sequence_2 = sequences[2] + sequences[3]
    print('my_first_sequence', sequence_1)
    print('another_sequence', sequence_2)
    if args.mode == "global":
        print("Naive-Global")
        aligned_1, aligned_2, score_naive = global_naive(sequence_1,sequence_2, args.gapopen)
        len_x = len(aligned_1)
        turn_x = len_x / 60
        file_object = open("global-naiveGap.aln", "w")
        count = 0
        condition = True
        file_object.write('Score:' +  str(score_naive) + '\n')
        while condition:
            file_object.write('my_first_sequence: ' + aligned_1[60*count:60*(count+1)] + '\n')          
            file_object.write('another_sequence:  ' + aligned_2[60*count:60*(count+1)] + '\n')
            count += 1
            if count >= turn_x:
                condition = False
    elif args.mode == "aglobal":
        print("Affine-Global")
        aligned_1, aligned_2, afine_score = global_affine(sequence_1, sequence_2, args.gapopen, args.gapext)
        len_x = len(aligned_1)
        turn_x = len_x / 60
        file_object = open("global-affineGap.aln", "w")
        count = 0
        condition = True
        file_object.write('Score:' + str(afine_score) + '\n')
        while condition:
            file_object.write('my_first_sequence: ' + aligned_1[60*count:60*(count+1)] + '\n')          
            file_object.write('another_sequence:  ' + aligned_2[60*count:60*(count+1)] + '\n')
            count += 1
            if count >= turn_x:
                condition = False
    elif args.mode == "local":
        print('Naive-Local')
        aligned,local_score = local_naive(sequence_1, sequence_2, args.gapopen)
        len_x = len(aligned)
        turn_x = len_x / 60
        file_object = open("local-naiveGap.aln", "w")
        count = 0
        condition = True
        while condition:
            file_object.write('longest local subsequence: ' + aligned[60*count:60*(count+1)] + '\n')          
            count += 1
            if count >= turn_x:
                condition = False        
    elif args.mode == "alocal":
        print('Affine-Local')
        aligned,local_affine_score, longest_sub = local_affine(sequence_1, sequence_2, args.gapopen, args.gapext)
        len_x = len(longest_sub)
        turn_x = len_x / 60
        file_object = open("local-affineGap.aln", "w")
        count = 0
        condition = True
        while condition:
            file_object.write('longest local subsequence: ' + longest_sub[60*count:60*(count+1)] + '\n')          
            count += 1
            if count >= turn_x:
                condition = False           
    else:
        print('selected mode does not exist')
main()