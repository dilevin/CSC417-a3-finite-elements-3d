#include <Eigen/Dense>
#include <EigenTypes.h>

//Input:
//  qdot - generalized velocity for the FEM system
//  element - the 1x4 vertex indices for this tetrahedron
//  density - density of material
//  volume - the undeformed tetrahedron volume
//Output:
//  M - dense 12x12 per-tetrahedron mass matrix
void mass_matrix_linear_tetrahedron(Eigen::Matrix1212d &M, Eigen::Ref<const Eigen::VectorXd> qdot, Eigen::Ref<const Eigen::RowVectorXi> element, double density, double volume);
