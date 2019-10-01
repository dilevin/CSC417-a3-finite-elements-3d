#include <iostream>
#include <thread>

#include <assignment_setup.h>
#include <visualization.h>

//Simulation State
Eigen::VectorXd q;
Eigen::VectorXd qdot;

//simulation time and time step
double t = 0; //simulation time 
double dt = 0.01; //time step

//simulation loop
bool simulating = true;

bool simulation_callback() {

    while(simulating) {
        simulate(q, qdot, dt, t);
        t += dt;
    }

    return false;
}

bool draw_callback(igl::opengl::glfw::Viewer &viewer) {
    
    draw(q, qdot, t);

    return false;
}

void f(int &a, int b, int c) {
    a = b + c;


}

void g(Eigen::Vector3d &a, Eigen::Vector3d b, Eigen::Vector3d c) {
    a = b + c;
}

template<typename Ret, typename B, typename C>
void h(Ret &&a, B b, C c, void (*func)(Ret, B, C)) {
    func(a,b,c);
}

int main(int argc, char **argv) {

    std::cout<<"Start A3\n";

    //assignment specific setup
    assignment_setup(argc, argv, q, qdot);

    //run simulation in seperate thread to avoid slowing down the UI
    std::thread simulation_thread(simulation_callback);
    simulation_thread.detach();

    //setup libigl viewer and activate 
    Visualize::setup(q, qdot, true);
    Visualize::viewer().callback_post_draw = &draw_callback;
    Visualize::viewer().launch();

    /* Function pointer example code so I don't forget 
     int a1;
    Eigen::Vector3d a2;

    h(a1, 1,2, &f);
    h(a2, Eigen::Vector3d(1,0,0), Eigen::Vector3d(0,1,0), &g);

    std::cout<<a1<<"\n";
    std::cout<<a2.transpose()<<"\n";*/

    return 1; 

}