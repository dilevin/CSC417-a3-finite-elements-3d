#include <Eigen/Dense>
#include <EigenTypes.h>

void d2V_linear_tetrahedron_dq2(Eigen::Matrix1212d &dV, Eigen::Ref<const Eigen::VectorXd> q, 
                          Eigen::Ref<const Eigen::MatrixXd> V, Eigen::Ref<const Eigen::RowVectorXi> element, double volume,
                          double C, double D);