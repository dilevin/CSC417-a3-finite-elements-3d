#include <Eigen/Dense>
#include <EigenTypes.h>

void psi_neo_hookean(double &psi, Eigen::Ref<const Eigen::Matrix3d> F, double C, double D);