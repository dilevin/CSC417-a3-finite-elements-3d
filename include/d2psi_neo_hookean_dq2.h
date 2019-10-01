#include <Eigen/Dense>
#include <EigenTypes.h>

void d2psi_neo_hookean_dF2(Eigen::Matrix99d &psi, Eigen::Ref<const Eigen::Matrix3d> F, double C, double D);