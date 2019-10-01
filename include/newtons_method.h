#include <Eigen/Dense>
#include <EigenTypes.h>

template<typename Objective, typename Jacobian, typename Hessian>
double newtons_method(Eigen::VectorXd &x0, Objective &f, Jacobian &g, Hessian &H, unsigned int maxSteps, Eigen::VectorXd &tmp_g, Eigen::SparseMatrixd &tmp_H) {
   
    Eigen::VectorXd dx;

    //compute newton step direction
    Eigen::SimplicialLDLT<Eigen::SparseMatrixd> solver;

    for(unsigned int ii=0; ii<maxSteps; ++ii) {
        
        //hessian matrix
        H(tmp_H, x0); 
        
        //compute gradient
        g(tmp_g, x0);
        
        //check for convergence
        if(tmp_g.norm() < 1e-1) {
            //return energy value
            return f(x0);
        }
        
        //factor the matrix 
        solver.compute(tmp_H);

        if(solver.info()!=Eigen::Success) {
            std::cout<<"newtons_method: decomposition failed\n";
            return std::numeric_limits<double>::infinity();
        }
    
         dx = -solver.solve(tmp_g);
    
        if(solver.info()!=Eigen::Success) {
            std::cout<<"newtons_method: solve failed\n";
            return std::numeric_limits<double>::infinity();
        }

        //perform backtracking on the newton search direction
        //Guarantee that step is a descent step and that it will make sufficient decreate
        double alpha = 1;
        double p = 0.5;
        double c = 1e-8;
        
        double f0 = f(x0);
        double s = f0 + c*tmp_g.transpose()*dx; //sufficient decrease
        
        //do this until you either converge to a stationary point or find a suitable step.
        while(alpha > 1e-8) {

            if(f(x0 + alpha*dx) < s) {
                break;
            }
            
            alpha *= p;
        }
       
        x0 += alpha*dx;
    }

    return f(x0);
}
