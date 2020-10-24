#include <Eigen/Dense>
#include <EigenTypes.h>

//Input:
//  V - the nx3 matrix of undeformed vertex positions. Each row is a single undeformed vertex position.
//  element - the 1x4 vertex indices for this tetrahedron
//  X - the position in the underformed space at which to compute the energy density
//Output:
//  dphi - the 4x3 gradient of the the basis functions wrt to X. The i'th row stores d phi_i/dX
void dphi_linear_tetrahedron_dX(Eigen::Matrix43d &dphi, Eigen::Ref<const Eigen::MatrixXd> V, Eigen::Ref<const Eigen::RowVectorXi> element, Eigen::Ref<const Eigen::Vector3d> X);
