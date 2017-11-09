#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <topic_tools/MuxSelect.h>

float avgDistance0;
float avgDistance1;
bool inhibited = false;

void sensor0(const sensor_msgs::LaserScan &msg) {
    avgDistance0 = *std::min_element(msg.ranges.begin(), msg.ranges.end());
}

void sensor1(const sensor_msgs::LaserScan &msg) {
    avgDistance1 = *std::min_element(msg.ranges.begin(), msg.ranges.end());
}

void inhibit(ros::ServiceClient &client) {
    topic_tools::MuxSelect::Response resp;
    topic_tools::MuxSelect::Request req;
    req.topic = "/subsumption/level1";
    inhibited = client.call(req, resp);
    ROS_INFO_STREAM("INHIBITING");
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "level1");
    ros::NodeHandle nh;
    ros::Rate rate(10);
    ros::Subscriber sub = nh.subscribe("/robot0/laser_0", 1000, &sensor0);
    ros::Subscriber sub1 = nh.subscribe("/robot0/laser_1", 1000, &sensor1);
    ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("/subsumption/level1", 1000);
    ros::ServiceClient selectService = nh.serviceClient<topic_tools::MuxSelect>("/mux/select");
    bool wallLeft = false;

    while (ros::ok()) {
        ros::spinOnce();
        geometry_msgs::Twist msg;
        if(avgDistance1 == 0 || avgDistance0 == 0) {
            ROS_INFO_STREAM("Skip");
            continue;
        }

        if (avgDistance0 < std::numeric_limits<float>::infinity())
            msg.angular.z -= 2*(1/avgDistance0);
        if (avgDistance1 < std::numeric_limits<float>::infinity() )
            msg.angular.z += 2*(1/avgDistance1);
        else {
            if (wallLeft)
                msg.angular.z = -1.7;
            else
                msg.angular.z = 1.7;
        }
        if (!inhibited && (avgDistance1 < std::numeric_limits<float>::infinity() || avgDistance0 < std::numeric_limits<float>::infinity())) {
            ROS_INFO_STREAM(avgDistance0);
            ROS_INFO_STREAM(avgDistance1);
            if (avgDistance1 < avgDistance0) {
                wallLeft = true;
            }
            inhibit(selectService);
        }

        msg.linear.x = 0.5;
        pub.publish(msg);
        /*ROS_INFO_STREAM(avgDistance0);
        ROS_INFO_STREAM(avgDistance1);
        ROS_INFO_STREAM(msg.angular.z);*/

        rate.sleep();
    }
}