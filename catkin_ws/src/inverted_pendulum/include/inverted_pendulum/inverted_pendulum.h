#ifndef inverted_pendulum_h
#define inverted_pendulum_h

#include "ros/ros.h"

#include <Eigen/Core>
#include <Eigen/Dense>

class CartPendulum
{
    private: enum State {x = 0, theta = 1};

    public: CartPendulum(double _dt);

    public: void initROS();

    private: std::unique_ptr<ros::NodeHandle> nodeHandle;

    public: double updateController();

    public: void statePropagation();

    private: double mass_cart = 1.0;

    private: double mass_pendulum = 1.0;

    private: double inertia_pendulum = 1.0;

    private: double l_c = 1.0;

    // x, theta, x_dot, theta_dot
    private: Eigen::Matrix<double, 2, 1> gen_pos = Eigen::Matrix<double, 2, 1>::Zero();

    private: Eigen::Matrix<double, 2, 1> gen_vel = Eigen::Matrix<double, 2, 1>::Zero();

    private: Eigen::Matrix<double, 2, 2> calculateMassMatrix();

    private: Eigen::Matrix<double, 2, 2> calculateCoreolisMatrix();

    private: Eigen::Matrix<double, 2, 1> calculateGravityMatrix();

    public: void printStatus();

    private: double dt = 0.01;

    private: double gravity = 9.81;

    private: double timer = 0.0;

    private: Eigen::Matrix<double, 2, 1> B_matrix;

    private: double kp_theta = 50.0;

    private: double kd_theta = 10.0;

    private: double wrapToPI(double _angle);

    private: double force = 0.0;

    private: double max_force = 100.0;
};

#endif