#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <EigenTypes.h>

#include <vector>

//Input:
//  q_size - total number of scalar generalized coordinates (3n for n vertices)
//  indices - m indices (row ids in V) for fixed vertices
//Output:
//  P - 3(n-m)x3n sparse matrix which projects out fixed vertices
void fixed_point_constraints(Eigen::SparseMatrixd &P, unsigned int q_size, const std::vector<unsigned int> indices);
