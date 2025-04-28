// int getHammingDistance()
// {
//     int distance = 0;
//     int n = board.size();
//     for (int i = 0; i < n; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             if (board[i][j] != 0 && board[i][j] != i * n + j + 1)
//             {
//                 distance++;
//             }
//         }
//     }
//     return distance;
// }

// int getManhattanDistance()
// {
//     int distance = 0;
//     int n = board.size();
//     for (int i = 0; i < n; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             if (board[i][j] != 0)
//             {
//                 int targetRow = (board[i][j] - 1) / n;
//                 int targetCol = (board[i][j] - 1) % n;
//                 distance += abs(i - targetRow) + abs(j - targetCol);
//             }
//         }
//     }
//     return distance;
// }

// int getEuclideanDistance()
// {
//     int distance = 0;
//     int n = board.size();
//     for (int i = 0; i < n; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             if (board[i][j] != 0)
//             {
//                 int targetRow = (board[i][j] - 1) / n;
//                 int targetCol = (board[i][j] - 1) % n;
//                 distance += sqrt(pow(i - targetRow, 2) + pow(j - targetCol, 2));
//             }
//         }
//     }
//     return distance;
// }

// int getLinearConflict()
// {
//     int conflict = 0;
//     int n = board.size();

//     // Row conflicts
//     /*
//     for (each row) {
//         for (each pair of tiles in that row) {
//             if (both tiles are in their correct row,
//                 and they are in the wrong order) {
//                 conflict++;
//             }
//         }
//     }
//     */
//     for (int row = 0; row < n; row++)
//     {
//         for (int i = 0; i < n; i++)
//         {
//             int tile1 = board[row][i];
//             if (tile1 == 0)
//             {
//                 continue;
//             }
//             int goalRow1 = (tile1 - 1) / n;
//             if (goalRow1 != row)
//             {
//                 continue;
//             }

//             for (int j = i + 1; j < n; j++)
//             {
//                 int tile2 = board[row][j];
//                 if (tile2 == 0)
//                 {
//                     continue;
//                 }
//                 int goalRow2 = (tile2 - 1) / n;
//                 if (goalRow2 != row)
//                 {
//                     continue;
//                 }

//                 int goalCol1 = (tile1 - 1) % n;
//                 int goalCol2 = (tile2 - 1) % n;
//                 if (goalCol1 > goalCol2)
//                 {
//                     conflict++;
//                 }
//             }
//         }
//     }

//     // Column conflicts
//     /*
//     for (each column) {
//         for (each pair of tiles in that column) {
//             if (both tiles are in their correct column,
//                 and they are in the wrong order) {
//                 conflict++;
//             }
//         }
//     }
//     */
//     for (int col = 0; col < n; col++)
//     {
//         for (int i = 0; i < n; i++)
//         {
//             int tile1 = board[i][col];
//             if (tile1 == 0)
//             {
//                 continue;
//             }
//             int goalCol1 = (tile1 - 1) % n;
//             if (goalCol1 != col)
//             {
//                 continue;
//             }

//             for (int j = i + 1; j < n; j++)
//             {
//                 int tile2 = board[j][col];
//                 if (tile2 == 0)
//                 {
//                     continue;
//                 }
//                 int goalCol2 = (tile2 - 1) % n;
//                 if (goalCol2 != col)
//                 {
//                     continue;
//                 }

//                 int goalRow1 = (tile1 - 1) / n;
//                 int goalRow2 = (tile2 - 1) / n;
//                 if (goalRow1 > goalRow2)
//                 {
//                     conflict++;
//                 }
//             }
//         }
//     }
//     return getManhattanDistance() + 2 * conflict;
// }