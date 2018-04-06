#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <ros/package.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <stdlib.h> 

ros::Publisher goal_pub;
ros::Subscriber goal_sub;
geometry_msgs::PoseStamped msg;
geometry_msgs::Pose pose;
std::vector<float> vec;
float value;


class erle_mission {

	constexpr static float x=0.0, y=0.0, z=0.0;


	public:
	constexpr static float pre=1.0f;
	static float p_x, p_y, p_z;
	static float g_x, g_y, g_z;
	static bool reached;
	
	void static publish_goal()
	{ 
		msg.pose.position.x = g_x;
		msg.pose.position.y = g_y;
		msg.pose.position.z = g_z;
		ROS_INFO("Publishing goal: x: %f, y: %f, z: %f", g_x, g_y, g_z);
		goal_pub.publish(msg);
	}

	void static listenPose(const geometry_msgs::PoseStamped& erlePose)
	{
		p_x = erlePose.pose.position.x;
		p_y = erlePose.pose.position.y;
		p_z = erlePose.pose.position.z;
		ROS_INFO("I heard pose: x: %f, y: %f, z: %f", p_x, p_y, p_z);
		if(abs(p_x-g_x)<pre && abs(p_y-g_y)<pre && abs(p_z-g_z)<pre){
			reached = true;
			ROS_INFO("Goal reached: x: %f, y: %f, z: %f", g_x, g_y, g_z);
		}
	}

};

float erle_mission::p_x = 0.0f;
float erle_mission::p_y = 0.0f;
float erle_mission::p_z = 0.0f;

float erle_mission::g_x = 0.0f;
float erle_mission::g_y = 0.0f;
float erle_mission::g_z = 0.0f;

bool erle_mission::reached = false;

int main(int argc, char **argv) {


	ROS_INFO("Erle mission has started");
	ros::init(argc, argv, "erle_mission");
	ros::NodeHandle nh;
	ros::Rate loop_rate(100);
	std::ifstream myFile;
	myFile.open("/home/rakesh-ubuntu/simulation/ros_catkin_ws/src/erle_mission/src/mission.txt", std::ios::app);

	if (myFile.is_open()){
	    ROS_INFO("File is open");
	    while(myFile >> value) {
	        vec.push_back(value);
	        ROS_INFO("value is %f",value);
	    }
   		myFile.close();

	}else {
		ROS_INFO("Unable to open file");
	}

	goal_pub = nh.advertise<geometry_msgs::PoseStamped>("pose",1);
    goal_sub = nh.subscribe("currentPose",1, &erle_mission::listenPose);
	
	float delta = 0.0;


	while (ros::ok()) 
	{
		erle_mission::g_x = vec[0]+delta;
		erle_mission::g_y = vec[1]+delta;
		erle_mission::g_z = vec[2]+delta;
		erle_mission::publish_goal();

		if(erle_mission::reached) {
			delta += 1;
			if(delta == 3.0) {
				delta = 0.0;
			}
			erle_mission::reached = false;
		}
		ros::spinOnce();
		loop_rate.sleep();
	}

	return 0;
}
