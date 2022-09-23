/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32) { // optimized 8*8 block transpose is good, 287
        
        // divide the 32*32 matrix to 4*4 block matrix
        for (int out_i = 0; out_i < 4; out_i+=1) {
            for (int out_j = 0; out_j < 4; out_j+=1) {

                // now block matrix transpose, two cases
                if (out_i == out_j) { // optimize same block transpose as normal one will clash cacheline
                    int in_i, in_j, temp;

                    for (in_i = 0; in_i < 8; in_i+=1) {
                        for (in_j = 0; in_j < 8; in_j+=1) {

                            if (in_i == in_j) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                        }
                        B[out_j*8 + in_i][out_i*8 + in_i] = temp; // essence
                    }
                } else { // simple block move as cacheline doesn't clash
                    for (int in_i = 0; in_i < 8; in_i+=1) {
                        for (int in_j = 0; in_j < 8; in_j+=1) {
                            B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                        }
                    }
                }

            }
        }
        
    } else if (M == 64) {
        // func 0 (Transpose submission): hits:6906, misses:1291, evictions:1259
        // we can further optimize using //1's method, but no need; 8*4*8 + 18*56=1264
        // 
        int a1, a2, a3, a4, a5, a6, a7, a8, out_i, out_j, in_i, in_j;
        for (out_i = 0; out_i < 8; out_i+=1) {
            for (out_j = 0; out_j < 8; out_j+=1) {
                if (out_i == out_j) {
                    //1
                    a1 = A[out_i*8 +2][out_j*8 +0];
                    a2 = A[out_i*8 +2][out_j*8 +1];
                    a3 = A[out_i*8 +3][out_j*8 +0];
                    a4 = A[out_i*8 +3][out_j*8 +1];
                    a5 = A[out_i*8 +2][out_j*8 +2];
                    a6 = A[out_i*8 +2][out_j*8 +3];
                    a7 = A[out_i*8 +3][out_j*8 +2];
                    a8 = A[out_i*8 +3][out_j*8 +3];

                    for (in_i = 0; in_i < 2; in_i+=1) {
                        for (in_j = 2; in_j < 4; in_j+=1) {
                            B[out_j*8+in_j][out_i*8+in_i] = A[out_i*8+in_i][out_j*8+in_j];
                        }
                    }
                    B[out_j*8 +2][out_i*8 + 2] = a5;
                    B[out_j*8 +2][out_i*8 + 3] = a7;
                    B[out_j*8 +3][out_i*8 + 2] = a6;
                    B[out_j*8 +3][out_i*8 + 3] = a8;
                    a5 = A[out_i*8 +0][out_j*8 +0];
                    a6 = A[out_i*8 +0][out_j*8 +1];
                    a7 = A[out_i*8 +1][out_j*8 +0];
                    a8 = A[out_i*8 +1][out_j*8 +1];
                    B[out_j*8 +0][out_i*8 +0] = a5;
                    B[out_j*8 +0][out_i*8 +1] = a7;
                    B[out_j*8 +0][out_i*8 +2] = a1;
                    B[out_j*8 +0][out_i*8 +3] = a3;
                    B[out_j*8 +1][out_i*8 +0] = a6;
                    B[out_j*8 +1][out_i*8 +1] = a8;
                    B[out_j*8 +1][out_i*8 +2] = a2;
                    B[out_j*8 +1][out_i*8 +3] = a4;

                    int temp;
                    for (in_i = 4; in_i < 8; in_i+=1) {
                        for (in_j = 0; in_j < 4; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if (in_i == (in_j+4)) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i-4][out_i*8 + in_i] = temp; // essence
                    }

                    for (in_i = 4; in_i < 8; in_i+=1) {
                        for (in_j = 4; in_j < 8; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if (in_i == in_j) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i][out_i*8 + in_i] = temp; // essence
                    }

                    for (in_i = 0; in_i < 4; in_i+=1) {
                        for (in_j = 4; in_j < 8; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if ((in_i +4)== in_j) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i+4][out_i*8 + in_i] = temp; // essence
                    }
                    /*
                    //2
                    a1 = A[out_i*8 +2][out_j*8 +4 +0];
                    a2 = A[out_i*8 +2][out_j*8 +4 +1];
                    a3 = A[out_i*8 +3][out_j*8 +4 +0];
                    a4 = A[out_i*8 +3][out_j*8 +4 +1];
                    a5 = A[out_i*8 +2][out_j*8 +4 +2];
                    a6 = A[out_i*8 +2][out_j*8 +4 +3];
                    a7 = A[out_i*8 +3][out_j*8 +4 +2];
                    a8 = A[out_i*8 +3][out_j*8 +4 +3];

                    for (in_i = 0; in_i < 2; in_i+=1) {
                        for (in_j = 6; in_j < 8; in_j+=1) {
                            B[out_j*8+in_j][out_i*8+in_i] = A[out_i*8+in_i][out_j*8+in_j];
                        }
                    }
                    B[out_j*8 +4 +2][out_i*8 + 2] = a5;
                    B[out_j*8 +4 +2][out_i*8 + 3] = a7;
                    B[out_j*8 +4 +3][out_i*8 + 2] = a6;
                    B[out_j*8 +4 +3][out_i*8 + 3] = a8;
                    a5 = A[out_i*8 +4 +0][out_j*8 +0];
                    a6 = A[out_i*8 +4 +0][out_j*8 +1];
                    a7 = A[out_i*8 +4 +1][out_j*8 +0];
                    a8 = A[out_i*8 +4 +1][out_j*8 +1];
                    B[out_j*8 +4 +0][out_i*8 +0] = a5;
                    B[out_j*8 +4 +0][out_i*8 +1] = a7;
                    B[out_j*8 +4 +0][out_i*8 +2] = a1;
                    B[out_j*8 +4 +0][out_i*8 +3] = a3;
                    B[out_j*8 +4 +1][out_i*8 +0] = a6;
                    B[out_j*8 +4 +1][out_i*8 +1] = a8;
                    B[out_j*8 +4 +1][out_i*8 +2] = a2;
                    B[out_j*8 +4 +1][out_i*8 +3] = a4;

                    //3
                    a1 = A[out_i*8 +4 +2][out_j*8 +0];
                    a2 = A[out_i*8 +4 +2][out_j*8 +1];
                    a3 = A[out_i*8 +4 +3][out_j*8 +0];
                    a4 = A[out_i*8 +4 +3][out_j*8 +1];
                    a5 = A[out_i*8 +4 +2][out_j*8 +2];
                    a6 = A[out_i*8 +4 +2][out_j*8 +3];
                    a7 = A[out_i*8 +4 +3][out_j*8 +2];
                    a8 = A[out_i*8 +4 +3][out_j*8 +3];

                    for (in_i = 4; in_i < 6; in_i+=1) {
                        for (in_j = 2; in_j < 4; in_j+=1) {
                            B[out_j*8+in_j][out_i*8+in_i] = A[out_i*8+in_i][out_j*8+in_j];
                        }
                    }
                    B[out_j*8 +2][out_i*8 +4 +2] = a5;
                    B[out_j*8 +2][out_i*8 +4 +3] = a7;
                    B[out_j*8 +3][out_i*8 +4 +2] = a6;
                    B[out_j*8 +3][out_i*8 +4 +3] = a8;
                    a5 = A[out_i*8 +4 +0][out_j*8 +0];
                    a6 = A[out_i*8 +4 +0][out_j*8 +1];
                    a7 = A[out_i*8 +4 +1][out_j*8 +0];
                    a8 = A[out_i*8 +4 +1][out_j*8 +1];
                    B[out_j*8 +0][out_i*8 +4 +0] = a5;
                    B[out_j*8 +0][out_i*8 +4 +1] = a7;
                    B[out_j*8 +0][out_i*8 +4 +2] = a1;
                    B[out_j*8 +0][out_i*8 +4 +3] = a3;
                    B[out_j*8 +1][out_i*8 +4 +0] = a6;
                    B[out_j*8 +1][out_i*8 +4 +1] = a8;
                    B[out_j*8 +1][out_i*8 +4 +2] = a2;
                    B[out_j*8 +1][out_i*8 +4 +3] = a4;
                    */
                    /* 
                    // hits:6882, misses:1315, evictions:1283
                    int temp;
                    for (in_i = 0; in_i < 4; in_i+=1) {
                        for (in_j = 0; in_j < 4; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if (in_i == (in_j)) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i][out_i*8 + in_i] = temp; // essence
                    }
                    for (in_i = 4; in_i < 8; in_i+=1) {
                        for (in_j = 0; in_j < 4; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if (in_i == (in_j+4)) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i-4][out_i*8 + in_i] = temp; // essence
                    }

                    for (in_i = 4; in_i < 8; in_i+=1) {
                        for (in_j = 4; in_j < 8; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if (in_i == in_j) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i][out_i*8 + in_i] = temp; // essence
                    }

                    for (in_i = 0; in_i < 4; in_i+=1) {
                        for (in_j = 4; in_j < 8; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if ((in_i +4)== in_j) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i+4][out_i*8 + in_i] = temp; // essence
                    }
                    */
                    
                } else {

                    for (in_i = 0; in_i < 4; in_i+=1) {
                        for (in_j = 0; in_j < 4; in_j+=1) {
                            B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                        }
                    }
                    a1 = A[out_i*8 + 0][out_j*8 + 4];
                    a2 = A[out_i*8 + 0][out_j*8 + 5];
                    a3 = A[out_i*8 + 0][out_j*8 + 6];
                    a4 = A[out_i*8 + 0][out_j*8 + 7];
                    a5 = A[out_i*8 + 1][out_j*8 + 4];
                    a6 = A[out_i*8 + 1][out_j*8 + 5];
                    a7 = A[out_i*8 + 1][out_j*8 + 6];
                    a8 = A[out_i*8 + 1][out_j*8 + 7];
                    
                    for (in_i = 4; in_i < 8; in_i+=1) {
                        for (in_j = 0; in_j < 4; in_j+=1) {
                            B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                        }
                    }
 
                    for (in_i = 4; in_i < 8; in_i+=1) {
                        for (in_j = 4; in_j < 8; in_j+=1) {
                            B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                        }
                    }
                    
                    for (in_i = 2; in_i < 4; in_i+=1) {
                        for (in_j = 4; in_j < 8; in_j+=1) {
                            B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                        }
                    }
                    B[out_j*8 + 4][out_i*8 + 0] = a1;
                    B[out_j*8 + 5][out_i*8 + 0] = a2;
                    B[out_j*8 + 6][out_i*8 + 0] = a3;
                    B[out_j*8 + 7][out_i*8 + 0] = a4;
                    B[out_j*8 + 4][out_i*8 + 1] = a5;
                    B[out_j*8 + 5][out_i*8 + 1] = a6;
                    B[out_j*8 + 6][out_i*8 + 1] = a7;
                    B[out_j*8 + 7][out_i*8 + 1] = a8;
                }
            }
        }
        /*
        // 1427, optimize with C to U, i mean we try to optmize with following block in read or write
        for (int out_i = 0; out_i < 8; out_i+=1) {
            for (int out_j = 0; out_j < 8; out_j+=1) { 
                if (out_i == out_j) {
                    
                    int in_i, in_j;
                    int temp;

                    for (in_i = 0; in_i < 4; in_i+=1) {
                        for (in_j = 0; in_j < 4; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if (in_i == in_j) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i][out_i*8 + in_i] = temp; // essence
                    }

                    

                    for (in_i = 4; in_i < 8; in_i+=1) {
                        for (in_j = 0; in_j < 4; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if (in_i == (in_j+4)) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i-4][out_i*8 + in_i] = temp; // essence
                    }

                    for (in_i = 4; in_i < 8; in_i+=1) {
                        for (in_j = 4; in_j < 8; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if (in_i == in_j) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i][out_i*8 + in_i] = temp; // essence
                    }

                    for (in_i = 0; in_i < 4; in_i+=1) {
                        for (in_j = 4; in_j < 8; in_j+=1) {
                            //B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            
                            if ((in_i +4)== in_j) {  // essence
                                temp = A[out_i*8 + in_i][out_j*8 + in_j];
                            } else {
                                B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                            }   
                            
                        }
                        B[out_j*8 + in_i+4][out_i*8 + in_i] = temp; // essence
                    }
                    
                    
                    
                } else {
                    for (int in_i = 0; in_i < 4; in_i+=1) {
                        for (int in_j = 0; in_j < 4; in_j+=1) {
                            B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                        }
                    }
                    
                    for (int in_i = 0; in_i < 4; in_i+=1) {
                        for (int in_j = 4; in_j < 8; in_j+=1) {
                            B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                        }
                    }
                    for (int in_i = 4; in_i < 8; in_i+=1) {
                        for (int in_j = 4; in_j < 8; in_j+=1) {
                            B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                        }
                    }
                    for (int in_i = 4; in_i < 8; in_i+=1) {
                        for (int in_j = 0; in_j < 4; in_j+=1) {
                            B[out_j*8 + in_j][out_i*8 + in_i] = A[out_i*8 + in_i][out_j*8 + in_j];
                        }
                    }
                    
                }
            }
        }
        */
        /*
        // 1699, 4*4 block with optimize
        for (int out_i = 0; out_i < 16; out_i+=1) {
            for (int out_j = 0; out_j < 16; out_j+=1) {

                // now block matrix transpose, two cases
                if ((out_i == out_j) || (out_i - out_j == 1) || (out_j - out_i == 1)) { // optimize same block transpose as normal one will clash cacheline
                    int in_i, in_j; 
                    int temp;
                    for (in_i = 0; in_i < 4; in_i+=1) {
                        for (in_j = 0; in_j < 4; in_j+=1) {
                            
                            if (in_i == in_j) {  // essence
                                temp = A[out_i*4 + in_i][out_j*4 + in_j];
                            } else { 
                                B[out_j*4 + in_j][out_i*4 + in_i] = A[out_i*4 + in_i][out_j*4 + in_j];
                            }   
                            
                            //B[out_j*4 + in_j][out_i*4 + in_i] = A[out_i*4 + in_i][out_j*4 + in_j];
                        }
                        B[out_j*4 + in_i][out_i*4 + in_i] = temp; // essence
                    }
                } else { // simple block move as cacheline doesn't clash
                    for (int in_i = 0; in_i < 4; in_i+=1) {
                        for (int in_j = 0; in_j < 4; in_j+=1) {
                            B[out_j*4 + in_j][out_i*4 + in_i] = A[out_i*4 + in_i][out_j*4 + in_j];
                        }
                    }
                }

            }
        }
        */

    } else if (M == 61) {
        // we just check cache, so outer loop speed is not important, inner block size matter if we just use block movement
        // 1:4423 2:3115 3:4423 4:2425 5:2296 6:2224 7:2152 8:2118 9:2092 10:2076 11:2089 12:2057 13:2048 [14:1996] 15:2021
        // [16:1992] [17:1950] [18:1961] [19:1979] 20:2002 [21:1957] [22:1959] [23:1928] 24:2015 25:2107 26:2202 27:2298 28:2400
        // 29:2495 30:2595
        // I guess further is not going better, so the best is [23:1928]
        // 23*23 block transpose is good
        // WTH????? why????
        const int sz = 23;
        int x,y;
        for (int out_i = 0; out_i < 5; out_i+=1) {
            for (int out_j = 0; out_j < 5; out_j+=1) { 

                for (int in_i = 0; in_i < sz; in_i+=1) {
                        for (int in_j = 0; in_j < sz; in_j+=1) {

                            x = out_i*sz + in_i;
                            y = out_j*sz + in_j;
                            if ((y < 61) && (x < 67)) {
                                B[y][x] = A[x][y];
                            }
                        }
                    }
            }
        }
    }
    
}
/*
32 line of cache, each with 32 bytes = 8 ints, direct mapping
32*32, full if m < 300,  0 if m > 600
64*64, full if m < 1300,  0 if m > 2000
61*67, full if m < 2000,  0 if m > 3000


*/

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

