#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    ros::init(argc, argv, "wander_randomly");
    ros::NodeHandle nh;
    ros::Rate rate(1);
    ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("/robot0/cmd_vel", 1000);

    while (ros::ok()) {
        geometry_msgs::Twist msg;
        msg.linear.x = double(rand())/double(RAND_MAX);
        msg.angular.z = 2*double(rand())/double(RAND_MAX) -1;

        pub.publish(msg);
        rate.sleep();
    }
}

