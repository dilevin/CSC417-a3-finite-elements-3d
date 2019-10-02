#include <Eigen/Dense>
#include <EigenTypes.h>

//Input:
// qdot - generalied velocity of FEM system
// element - vertex indices of the element
// density - material density
// volume - volume of tetrahedron
//Output:
//  T - kinetic energy of tetrahedron
void T_linear_tetrahedron(double &T, Eigen::Ref<const Eigen::VectorXd> qdot, Eigen::Ref<const Eigen::RowVectorXi> element, double density, double volume);