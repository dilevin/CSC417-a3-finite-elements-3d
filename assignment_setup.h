#ifndef ASSIGNMENT_SETUP_H
#define ASSIGNMENT_SETUP_H

/*Assignment 2 code 
//load data and compute edge lengths for springs
#include <igl/edges.h>
#include <igl/edge_lengths.h>
#include <igl/readMESH.h>
#include <igl/boundary_facets.h>

//assignment files for implementing simulation and interaction
#include <visualization.h>
#include <init_state.h>
#include <mass_matrix_particles.h>
#include <find_min_vertices.h>
#include <fixed_point_constraints.h>
#include <dV_spring_particle_particle_dq.h>
#include <dV_gravity_particle_dq.h>
#include <d2V_spring_particle_particle_dq2.h>
#include <assemble_forces.h>
#include <assemble_stiffness.h>
#include <linearly_implicit_euler.h>
#include <T_particle.h>
#include <V_gravity_particle.h>
#include <V_spring_particle_particle.h>

//Assignment variables
Eigen::SparseMatrixd M;
Eigen::SparseMatrixd P; //fixed point constraints 
Eigen::VectorXd x0; //fixed point constraints 

double k  = 1e5;
double k_selected = 1e5; //stiff spring for pulling on object
double m = 1.;

Eigen::MatrixXd V; //vertices of simulation mesh 
Eigen::MatrixXi T; //faces of simulation mesh
Eigen::MatrixXi F; //faces of simulation mesh
Eigen::MatrixXi E; //edges of simulation mesh (which will become springs)
Eigen::VectorXd l0; //original length of all edges in the mesh

//working memory for integrator
Eigen::VectorXd tmp_force;
Eigen::SparseMatrixd tmp_stiffness;

std::vector<unsigned int> fixed_point_indices;

//Assignment methods 
inline void simulate(Eigen::VectorXd &q, Eigen::VectorXd &qdot, double dt) {  
    //assemble force vector
    auto force = [&](Eigen::VectorXd &f, Eigen::Ref<const Eigen::VectorXd> q, Eigen::Ref<const Eigen::VectorXd> qdot) { 
        assemble_forces(f, P.transpose()*q+x0, P.transpose()*qdot, V, E, l0, m, k);

        //Interaction spring
        Eigen::Vector3d mouse;
        Eigen::Vector6d dV_mouse;

        for(unsigned int pickedi = 0; pickedi < Visualize::picked_vertices().size(); pickedi++) {   
            mouse = (P.transpose()*q+x0).segment<3>(3*Visualize::picked_vertices()[pickedi]) + Visualize::mouse_drag_world() + Eigen::Vector3d::Constant(1e-6);
            dV_spring_particle_particle_dq(dV_mouse, mouse, (P.transpose()*q+x0).segment<3>(3*Visualize::picked_vertices()[pickedi]), 0.0, (Visualize::is_mouse_dragging() ? k_selected : 0.));
            f.segment<3>(3*Visualize::picked_vertices()[pickedi]) -= dV_mouse.segment<3>(3);
        }

        f = P*f; 
    };

    //assemble stiffness matrix,
    auto stiffness = [&](Eigen::SparseMatrixd &K, Eigen::Ref<const Eigen::VectorXd> q, Eigen::Ref<const Eigen::VectorXd> qdot) { 
        assemble_stiffness(K, P.transpose()*q+x0, P.transpose()*qdot, V, E, l0, k);
        K = P*K*P.transpose();
    };

    linearly_implicit_euler(q, qdot, dt, M, force, stiffness, tmp_force, tmp_stiffness);
}

inline void draw(Eigen::Ref<const Eigen::VectorXd> q, Eigen::Ref<const Eigen::VectorXd> qdot, double t) {

        double V_spring, V_gravity, T_p, KE,PE;
        KE = 0;
        PE = 0;

        for(unsigned int p = 0; p < V.rows(); p++) {
            T_particle(T_p, (P.transpose()*qdot).segment<3>(3*p), m);
            V_gravity_particle(V_gravity, (P.transpose()*q+x0).segment<3>(3*p), m, Eigen::Vector3d(0., -9.8, 0.));

            PE += V_gravity;
            KE += T_p;
        }
        
        for(unsigned int ei = 0; ei < E.rows(); ei++) {
            V_spring_particle_particle(V_spring, (P.transpose()*q+x0).segment<3>(3*E(ei,0)), (P.transpose()*q+x0).segment<3>(3*E(ei,1)), l0(ei), k);

            PE += V_spring;
        }
        
        Visualize::add_energy(t, KE, PE);

    //update vertex positions using simulation
    Visualize::update_vertex_positions(0, P.transpose()*q + x0);
}

inline void assignment_setup(Eigen::VectorXd &q, Eigen::VectorXd &qdot) {
    
    //load geometric data 
    igl::readMESH("../data/coarse_bunny.mesh",V,T, F);
    igl::boundary_facets(T, F);
    F = F.rowwise().reverse().eval();
    igl::edges(T,E);
    igl::edge_lengths(V,E,l0);

    //setup simulation 
    init_state(q,qdot,V);
    mass_matrix_particles(M, q, m);
    
    //setup constraint matrix
    find_min_vertices(fixed_point_indices, V, 3);
    P.resize(q.rows(),q.rows());
    P.setIdentity();
    fixed_point_constraints(P, q.rows(), fixed_point_indices);
    
    x0 = q - P.transpose()*P*q; //vector x0 contains position of all fixed nodes, zero for everything else
    
    //correct M, q and qdot so they are the right size
    q = P*q;
    qdot = P*qdot;
    M = P*M*P.transpose();

    Visualize::add_object_to_scene(V,F, Eigen::RowVector3d(244,165,130)/255.);

    //igl additional menu setup
    // Add content to the default menu window
    Visualize::viewer_menu().callback_draw_custom_window = [&]()
    {
        // Define next window position + size
        ImGui::SetNextWindowPos(ImVec2(180.f * Visualize::viewer_menu().menu_scaling(), 10), ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(
            "Energy Plot", nullptr,
            ImGuiWindowFlags_NoSavedSettings

        );

        ImVec2 min = ImGui::GetWindowContentRegionMin();
        ImVec2 max = ImGui::GetWindowContentRegionMax();

        max.x = ( max.x - min.x ) / 2;
        max.y -= min.y + ImGui::GetItemsLineHeightWithSpacing() * 3;

        Visualize::plot_energy("T", 1, ImVec2(-15,10), ImVec2(0,2e6), ImGui::GetColorU32(ImGuiCol_PlotLines));
        Visualize::plot_energy("V", 2, ImVec2(-15,10), ImVec2(0,2e6), ImGui::GetColorU32(ImGuiCol_HeaderActive));
        Visualize::plot_energy("T+V", 3, ImVec2(-15,10), ImVec2(0,4e6), ImGui::GetColorU32(ImGuiCol_ColumnActive));

        ImGui::End();
    };
    
}*/

//Assignment 3 code 
#include <igl/readMESH.h>
#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <igl/readOFF.h>
#include <read_tetgen.h>
#include <igl/boundary_facets.h>
#include <igl/volume.h>

//assignment files for implementing simulation and interaction
#include <visualization.h>
#include <init_state.h>
#include <find_min_vertices.h>
#include <fixed_point_constraints.h>
#include <dphi_linear_tetrahedron_dX.h>
#include <psi_neo_hookean.h>
#include <quadrature_single_point.h>
#include <T_linear_tetrahedron.h>
#include <V_linear_tetrahedron.h>
#include <V_spring_particle_particle.h>
#include <dV_linear_tetrahedron_dq.h>
#include <dV_spring_particle_particle_dq.h>
#include <d2V_linear_tetrahedron_dq2.h>
#include <mass_matrix_mesh.h>
#include <assemble_forces.h>
#include <assemble_stiffness.h>
#include <linearly_implicit_euler.h>
#include <implicit_euler.h>
#include <build_skinning_matrix.h>

//Variable for geometry
Eigen::MatrixXd V; //vertices of simulation mesh 
Eigen::MatrixXi T; //faces of simulation mesh
Eigen::MatrixXi F; //faces of simulation mesh

//variables for skinning
Eigen::MatrixXd V_skin;
Eigen::MatrixXi F_skin;
Eigen::SparseMatrixd N; 

//material parameters
double density = 0.1;
double YM = 6e5; //young's modulus
double mu = 0.4; //poissons ratio
double D = 0.5*(YM*mu)/((1.0+mu)*(1.0-2.0*mu));
double C = 0.5*YM/(2.0*(1.0+mu));

//BC
std::vector<unsigned int> fixed_point_indices;
Eigen::SparseMatrixd P;
Eigen::VectorXd x0; 

//mass matrix
Eigen::SparseMatrixd M;
Eigen::VectorXd v0;

//scratch memory for assembly
Eigen::VectorXd tmp_qdot;
Eigen::VectorXd tmp_force;
Eigen::SparseMatrixd tmp_stiffness;

std::vector<std::pair<Eigen::Vector3d, unsigned int>> spring_points;

bool skinning_on = true;
bool fully_implicit = false;
bool bunny = true; 

//selection spring
double k_selected = 1e5;

inline void simulate(Eigen::VectorXd &q, Eigen::VectorXd &qdot, double dt, double t) {  

    double V_ele, T_ele, KE,PE;

    spring_points.clear();

    //Interaction spring
    Eigen::Vector3d mouse;
    Eigen::Vector6d dV_mouse;
    double k_selected_now = (Visualize::is_mouse_dragging() ? k_selected : 0.);
    
    for(unsigned int pickedi = 0; pickedi < Visualize::picked_vertices().size(); pickedi++) {   
        spring_points.push_back(std::make_pair((P.transpose()*q+x0).segment<3>(3*Visualize::picked_vertices()[pickedi]) + Visualize::mouse_drag_world() + Eigen::Vector3d::Constant(1e-6),3*Visualize::picked_vertices()[pickedi]));
    }

    auto energy = [&](Eigen::Ref<const Eigen::VectorXd> qdot_1)->double {
        double E = 0;
        Eigen::VectorXd newq = P.transpose()*(q+dt*qdot_1)+x0;

        for(unsigned int ei=0; ei<T.rows(); ++ei) {
            
            V_linear_tetrahedron(V_ele,newq , V, T.row(ei), v0(ei), C, D);
            E += V_ele;
        }

        for(unsigned int pickedi = 0; pickedi < spring_points.size(); pickedi++) {   
            V_spring_particle_particle(V_ele, spring_points[pickedi].first, newq.segment<3>(spring_points[pickedi].second), 0.0, k_selected_now);
            E += V_ele;
        }

        E += 0.5*(qdot_1 - qdot).transpose()*M*(qdot_1 - qdot);

        return E;
    };

    auto force = [&](Eigen::VectorXd &f, Eigen::Ref<const Eigen::VectorXd> q2, Eigen::Ref<const Eigen::VectorXd> qdot2) { 
        
            assemble_forces(f, P.transpose()*q2+x0, P.transpose()*qdot2, V, T, v0, C,D);

            for(unsigned int pickedi = 0; pickedi < spring_points.size(); pickedi++) {
                dV_spring_particle_particle_dq(dV_mouse, spring_points[pickedi].first, (P.transpose()*q2+x0).segment<3>(spring_points[pickedi].second), 0.0, k_selected_now);
                f.segment<3>(3*Visualize::picked_vertices()[pickedi]) -= dV_mouse.segment<3>(3);
            }

            f = P*f; 
        };

        //assemble stiffness matrix,
        auto stiffness = [&](Eigen::SparseMatrixd &K, Eigen::Ref<const Eigen::VectorXd> q2, Eigen::Ref<const Eigen::VectorXd> qdot2) { 
            assemble_stiffness(K, P.transpose()*q2+x0, P.transpose()*qdot2, V, T, v0, C, D);
            K = P*K*P.transpose();
        };

        if(fully_implicit)
            implicit_euler(q, qdot, dt, M, energy, force, stiffness, tmp_qdot, tmp_force, tmp_stiffness);
        else
            linearly_implicit_euler(q, qdot, dt, M, force, stiffness, tmp_force, tmp_stiffness);
        

        
    KE = 0;
    PE = 0;

    for(unsigned int ei=0; ei<T.rows(); ++ei) {
        T_linear_tetrahedron(T_ele, P.transpose()*qdot, T.row(ei), density, v0(ei));
        KE += T_ele;

        V_linear_tetrahedron(V_ele, P.transpose()*q+x0, V, T.row(ei), v0(ei), C, D);
        PE += V_ele;
    }
    
    Visualize::add_energy(t, KE, PE);
        
}

inline void draw(Eigen::Ref<const Eigen::VectorXd> q, Eigen::Ref<const Eigen::VectorXd> qdot, double t) {

    //update vertex positions using simulation
    Visualize::update_vertex_positions(0, P.transpose()*q + x0);

}

bool key_down_callback(igl::opengl::glfw::Viewer &viewer, unsigned char key, int modifiers) {

    if(key =='N') {
        std::cout<<"toggle integrators \n";
        fully_implicit = !fully_implicit;
    } else if(key == 'S') {
        
        skinning_on = !skinning_on;
        Visualize::toggle_skinning(skinning_on);
    }

    return false;
}
inline void assignment_setup(int argc, char **argv, Eigen::VectorXd &q, Eigen::VectorXd &qdot) {

    //load geometric data 
    igl::readMESH("../data/coarser_bunny.mesh",V,T, F);
    igl::readOBJ("../data/bunny_skin.obj", V_skin, F_skin);

    if(argc > 1) {
        if(strcmp(argv[1], "arma") == 0) {
            read_tetgen(V,T, "../data/arma_6.node", "../data/arma_6.ele");
            igl::readOBJ("../data/armadillo.obj", V_skin, F_skin);
        
            bunny = false;
            fully_implicit = true;
        }
    }
    
    igl::boundary_facets(T, F);
    F = F.rowwise().reverse().eval();
    
    build_skinning_matrix(N, V, T, V_skin);

    //setup simulation 
    init_state(q,qdot,V);

    //add geometry to scene
    Visualize::add_object_to_scene(V,F, V_skin, F_skin, N, Eigen::RowVector3d(244,165,130)/255.);
    Visualize::toggle_skinning(false);
    
    //bunny
    if(bunny)
        Visualize::set_picking_tolerance(1.);
    else
        Visualize::set_picking_tolerance(0.01);

    //volumes of all elements
    igl::volume(V,T, v0);

    //Mass Matrix
    mass_matrix_mesh(M, qdot, T, density, v0);
    
    if(M.rows() == 0) {
        std::cout<<"Mass Matrix not implemented, quitting \n";
        std::exit(0);
    }
    
    //setup constraint matrix
    if(bunny)
        find_min_vertices(fixed_point_indices, V, 3);
    else
        find_min_vertices(fixed_point_indices, V, 0.1);

    //material properties
    //bunny
    if(bunny) {
        YM = 6e6; //young's modulus
        mu = 0.4; //poissons ratio
        D = 0.5*(YM*mu)/((1.0+mu)*(1.0-2.0*mu));
        C = 0.5*YM/(2.0*(1.0+mu));
        k_selected = 1e8;
    } else {
        //arma
        YM = 6e5; //young's modulus
        mu = 0.4; //poissons ratio
        D = 0.5*(YM*mu)/((1.0+mu)*(1.0-2.0*mu));
        C = 0.5*YM/(2.0*(1.0+mu));
        k_selected = 1e5;
    }

    P.resize(q.rows(),q.rows());
    P.setIdentity();
    fixed_point_constraints(P, q.rows(), fixed_point_indices);
    
    x0 = q - P.transpose()*P*q; //vector x0 contains position of all fixed nodes, zero for everything else    
    //correct M, q and qdot so they are the right size
    q = P*q;
    qdot = P*qdot;
    M = P*M*P.transpose();

    //igl additional menu setup
    // Add content to the default menu window
    Visualize::viewer_menu().callback_draw_custom_window = [&]()
    {
        // Define next window position + size
        ImGui::SetNextWindowPos(ImVec2(180.f * Visualize::viewer_menu().menu_scaling(), 10), ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(
            "Energy Plot", nullptr,
            ImGuiWindowFlags_NoSavedSettings

        );

        ImVec2 min = ImGui::GetWindowContentRegionMin();
        ImVec2 max = ImGui::GetWindowContentRegionMax();

        max.x = ( max.x - min.x ) / 2;
        max.y -= min.y + ImGui::GetItemsLineHeightWithSpacing() * 3;

        Visualize::plot_energy("T", 1, ImVec2(-15,10), ImVec2(0,2e8), ImGui::GetColorU32(ImGuiCol_PlotLines));
        Visualize::plot_energy("V", 2, ImVec2(-15,10), ImVec2(0,2e7), ImGui::GetColorU32(ImGuiCol_HeaderActive));
        Visualize::plot_energy("T+V", 3, ImVec2(-15,10), ImVec2(0,4e8), ImGui::GetColorU32(ImGuiCol_ColumnActive));

        ImGui::End();
    };

    Visualize::viewer().callback_key_down = key_down_callback;

}

#endif

