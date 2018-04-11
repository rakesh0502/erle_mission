#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <ros/package.h>
#include <fstream>
#include <string.h>
#include <vector>
#include <iostream>
#include <stdlib.h> 
#include <boost/algorithm/string.hpp>

using namespace std;

ros::Publisher goal_pub;
ros::Subscriber goal_sub;
geometry_msgs::PoseStamped msg;
geometry_msgs::Pose pose;
vector<string> vec;
string line;
string delimiter = ",";


class erle_mission {

	constexpr static float x=0.0, y=0.0, z=0.0;


	public:
	constexpr static float pre=0.5f;
	static float p_x, p_y, p_z;
	static float g_x, g_y, g_z;
	static bool reached;
	static bool publish;
	
	void static publish_goal()
	{ 
		msg.pose.position.x = g_x;
		msg.pose.position.y = g_y;
		msg.pose.position.z = g_z;
		erle_mission::publish = false;
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
		}else{
			reached = false;
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
bool erle_mission::publish = true;

int main(int argc, char **argv) {


	ROS_INFO("Erle mission has started");
	ros::init(argc, argv, "erle_mission");
	ros::NodeHandle nh;
	ros::Rate loop_rate(100);
	std::ifstream myFile;
	myFile.open("/home/rakesh-ubuntu/simulation/ros_catkin_ws/src/erle_mission/src/mission.txt", std::ios::app);

	if (myFile.is_open()){
	    ROS_INFO("File is open");
	    myFile >> line;
	    ROS_INFO("Protocol number:%s\n", line.c_str());
	    while(myFile >> line) {
	        vec.push_back(line);
	        ROS_INFO("Line is %s\n",line.c_str());
	    }
   		myFile.close();

	}else {
		ROS_INFO("Unable to open file");
	}

	goal_pub = nh.advertise<geometry_msgs::PoseStamped>("pose",1);
    goal_sub = nh.subscribe("currentPose",1, &erle_mission::listenPose);
	
	float delta = 0.0;
	int i = 0;
	vector<string> v;
	while (ros::ok()) 
	{
		if(i<vec.size()) {
			boost::split(v, vec[i], boost::is_any_of(delimiter));
			erle_mission::g_x = atof(v[0].c_str());
			erle_mission::g_y = atof(v[1].c_str());
			erle_mission::g_z = atof(v[2].c_str());
			if(erle_mission::publish) {
				ROS_INFO("Goal is x: %f, y: %f, z: %f\n", erle_mission::g_x, erle_mission::g_y, erle_mission::g_z);
				erle_mission::publish_goal();
			}
			
			if(erle_mission::reached) {
				erle_mission::publish = true;
				erle_mission::reached = false;
				++i;
			}
		}
		ros::spinOnce();
		loop_rate.sleep();
	}

	return 0;
}
