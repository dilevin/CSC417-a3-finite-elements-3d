#include <Eigen/Dense>
#include <EigenTypes.h>

//Input:
//  F - the dense 3x3 deformation gradient
//  C,D - material parameters for the Neo-Hookean model
//Output:
//  psi - the 9x1 gradient of the potential energy wrt to the deformation gradient
void dpsi_neo_hookean_dF(Eigen::Vector9d &psi, Eigen::Ref<const Eigen::Matrix3d> F, double C, double D);

