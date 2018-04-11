#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>

ros::Subscriber goal_sub;
ros::Publisher pose_pub;
geometry_msgs::PoseStamped msg;


class dummy_copter {


	public:

	static float x, y, z;
	static float t_x, t_y, t_z;

	void static listenGoal(const geometry_msgs::PoseStamped& msg)
	{
		t_x = msg.pose.position.x;
		t_y = msg.pose.position.y;
		t_z = msg.pose.position.z;
		ROS_INFO("I heard goal: x: %f, y: %f, z: %f", t_x, t_y, t_z);
	}

	void static currentPose()
	{
		msg.pose.position.x = x;
		msg.pose.position.y = y;
		msg.pose.position.z = z;
		ROS_INFO("Current Pose: x: %f, y: %f, z: %f", x, y, z);
		pose_pub.publish(msg);
	}
};

float dummy_copter::x = 5.8f, dummy_copter::y = 5.8f, dummy_copter::z = 5.8f;
float dummy_copter::t_x = 0.0f, dummy_copter::t_y = 0.0f, dummy_copter::t_z = 0.0f;

int main(int argc, char **argv) {


	ROS_INFO("Dummy Copter has started");
	ros::init(argc, argv, "dummy_copter");
	ros::NodeHandle nh;
	goal_sub = nh.subscribe("pose",1, &dummy_copter::listenGoal);
	pose_pub = nh.advertise<geometry_msgs::PoseStamped>("currentPose",1);

	ros::Rate loop_rate(50);

	//ros::spin();

	while (ros::ok()) 
	{
		dummy_copter::x += 1.0;
		dummy_copter::y += 1.0;
		dummy_copter::z += 1.0;
		if(dummy_copter::x > 27.0) {
			dummy_copter::x = 8.8;
			dummy_copter::y = 8.8;
			dummy_copter::z = 8.8;
		}
		dummy_copter::currentPose();
		ros::spinOnce();
		loop_rate.sleep();
	}

	return 0;

}

