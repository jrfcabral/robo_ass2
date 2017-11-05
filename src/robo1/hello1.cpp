#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <topic_tools/MuxSelect.h>

float avgDistance0;
float avgDistance1;

void sensor0(const sensor_msgs::LaserScan &msg) {
    float sum = 0;

    for (int i = 0; i < msg.ranges.size(); i++) {
        if (sum < msg.ranges[i])
            sum = msg.ranges[i];
    }

    avgDistance0 = sum;
}

void sensor1(const sensor_msgs::LaserScan &msg) {
    float sum = 0;

    for (int i = 0; i < msg.ranges.size(); i++) {
        if (sum < msg.ranges[i])
            sum = msg.ranges[i];
    }

    avgDistance1 = sum;
}

void inhibit(ros::ServiceClient &client) {
    topic_tools::MuxSelect::Response resp;
    topic_tools::MuxSelect::Request req;
    req.topic = "/subsumption/level1";
    client.call(req, resp);
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "level1");
    ros::NodeHandle nh;
    ros::Rate rate(10);
    ros::Subscriber sub = nh.subscribe("/robot0/laser_0", 1000, &sensor0);
    ros::Subscriber sub1 = nh.subscribe("/robot0/laser_1", 1000, &sensor1);
    ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("/subsumption/level1", 1000);
    ros::ServiceClient selectService = nh.serviceClient<topic_tools::MuxSelect>("/mux/select");

    while (ros::ok()) {
        ros::spinOnce();
        geometry_msgs::Twist msg;
        if (avgDistance0 < 4)
            msg.angular.z -= 2*avgDistance0;
        if (avgDistance1 < 4 )
            msg.angular.z += 2*avgDistance1;
        if (avgDistance1 < 4 || avgDistance0 < 4)
            inhibit(selectService);
        msg.linear.x = 0.5;
        pub.publish(msg);

        rate.sleep();
    }
}