#include <ros/ros.h>
#include <chatroom/message.h>

void CallBack(const chatroom::message::ConstPtr& msg){

	if(!(msg->message).empty()){ //ignores empty messages
			ROS_INFO("[%s]:%s",msg->user.c_str(),msg->message.c_str());
		}
	}



int main(int argc, char **argv){

	ros::init(argc,argv,"user2");
	ros::NodeHandle nh;

	ros::Publisher pub = nh.advertise<chatroom::message>("chat", 1000);
	ros::Subscriber sub = nh.subscribe("chat", 1000, CallBack);
	
	chatroom::message msg;

  
  	std::cout << "Enter username: ";
  	std::getline(std::cin, msg.user);
  	
  	ros::AsyncSpinner sp(3);  //3 threads for the 3 nodes, setting 0 would let it use as many threads as there's processors available on the machine.
  	sp.start();
  	
  	while(ros::ok()){
  	
  		std::getline(std::cin, msg.message);
  		pub.publish(msg);
  		//ros::Duration(0.1).sleep();
  		//ros::spinOnce();
  		//ros::waitForShutdown();
  	}
	
	sp.stop();
return 0;

}
