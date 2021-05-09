#include "inverted_pendulum/inverted_pendulum.h"

CartPendulum::CartPendulum(double _dt)
{
    dt = _dt;

    gen_pos(theta) = M_PI-0.01;

    B_matrix(0) = 1.0;
    B_matrix(1) = 0.0;
}

void CartPendulum::initROS()
{
    if(!ros::isInitialized())
    {
        int argc = 0;
        char **argv = NULL;
        ros::init
        (
            argc,
            argv,
            "cart_pendulum_node",
            ros::init_options::NoSigintHandler
        );
    }

    nodeHandle.reset(new ros::NodeHandle("cart_pendulum_node"));
}

Eigen::Matrix<double, 2, 2> CartPendulum::calculateMassMatrix()
{
    Eigen::Matrix<double, 2, 2> M = Eigen::Matrix<double, 2, 2>::Zero();

    M(0, 0) = mass_cart + mass_pendulum;

    M(1, 1) = mass_pendulum*l_c*l_c + inertia_pendulum;

    M(0, 1) = mass_pendulum*l_c*cos(gen_pos(theta));

    M(1, 0) = M(0, 1);

    return M;
}

Eigen::Matrix<double, 2, 2> CartPendulum::calculateCoreolisMatrix()
{
    Eigen::Matrix<double, 2, 2> C = Eigen::Matrix<double, 2, 2>::Zero();

    C(0, 1) = -mass_pendulum*l_c*sin(gen_pos(theta))*gen_pos(theta);

    return C;
}

Eigen::Matrix<double, 2, 1> CartPendulum::calculateGravityMatrix()
{
    Eigen::Matrix<double, 2, 1> G = Eigen::Matrix<double, 2, 1>::Zero();

    G(1) = - mass_pendulum*gravity*l_c*sin(gen_pos(theta));

    return G;
}

void CartPendulum::statePropagation()
{
    Eigen::Matrix<double, 2, 2> M = calculateMassMatrix();
    Eigen::Matrix<double, 2, 2> C = calculateCoreolisMatrix();
    Eigen::Matrix<double, 2, 1> G = calculateGravityMatrix();

    Eigen::Matrix<double, 2, 1> gen_acc = M.inverse()*(B_matrix*force - G - C*gen_vel);

    gen_vel += gen_acc*dt;

    gen_pos += gen_vel*dt;

    timer += dt;
}

double CartPendulum::updateController()
{
    //double mu = (mass_pendulum*l_c*l_c + inertia_pendulum)*(-kp_theta*wrapToPI(gen_pos(theta)) - kd_theta*theta_dot);
    //double u = -mass_cart*gravity*l_c*sin(gen_pos(theta)) + mu;
    //double x_dd_ref = - u/(- mass_cart*l_c*cos(gen_pos(theta)));

    Eigen::Matrix<double, 2, 2> M = calculateMassMatrix();
    Eigen::Matrix<double, 2, 2> M_inv = M.inverse();
    Eigen::Matrix<double, 2, 2> C = calculateCoreolisMatrix();
    Eigen::Matrix<double, 2, 1> G = calculateGravityMatrix();

    Eigen::Matrix<double, 2, 1> gen_acc_cancel = C*gen_vel + G;

    double tau_cl = -kp_theta*wrapToPI(gen_pos(theta)) - kd_theta*gen_vel(theta);

    double tau = (-M_inv(1, 1)*G(1) + tau_cl)/M_inv(1, 0);

    //tau += -1.0*gen_pos(x) - 0.1*gen_vel(x);

    if(tau > max_force)
    {
        tau = max_force;
    }
    else if (tau < -max_force)
    {
        tau = - max_force;
    }

    force = tau;

    return tau;
}

double CartPendulum::wrapToPI(double _angle)
{
    return fmod(_angle, M_PI);
}

void CartPendulum::printStatus()
{
    ROS_INFO("Time: %f\tForce: %f\tx: %f\ttheta: %f\tx_d: %f\ttheta_d: %f", 
    timer, force, gen_pos(x), gen_pos(theta), gen_vel(x), gen_vel(theta));
}