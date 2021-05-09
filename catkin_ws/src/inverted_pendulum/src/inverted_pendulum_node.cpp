#include "ros/ros.h"
#include <ros/console.h>

#include "inverted_pendulum/inverted_pendulum.h"

int main(int argc, char **argv)
{
    CartPendulum cart_pendulum(0.01);

    cart_pendulum.initROS();

    ros::Rate control_rate(10);

    while(ros::ok())
    {
        cart_pendulum.printStatus();
        cart_pendulum.updateController();
        cart_pendulum.statePropagation();
        control_rate.sleep();
    }

    return 0;
}