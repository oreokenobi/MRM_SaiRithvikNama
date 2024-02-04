#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/Imu.h>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include<tf/tf.h>
#include <geometry_msgs/Twist.h>
#include <cmath>


using namespace std;
double bearing;


void imu_callback(const sensor_msgs::Imu::ConstPtr& msg) {
 
    tf::Quaternion quat(msg->orientation.x, msg->orientation.y, msg->orientation.z, msg->orientation.w);

    tf::Matrix3x3 orient(quat);
    double roll, pitch, yaw,roll_deg,pitch_deg,yaw_deg;
    orient.getRPY(roll, pitch, yaw);
    
    ROS_INFO("roll:%f\t",roll);
    ROS_INFO("pitch:%f\t",pitch);
    ROS_INFO("yaw:%f\n",yaw);
    roll_deg=roll*(180.0/M_PI);
    pitch_deg=pitch*(180.0/M_PI);
    yaw_deg=yaw*(180.0/M_PI);
    ROS_INFO("bearing:%f\nroll degrees:%f\t",bearing,roll_deg);
    ROS_INFO("pitch degrees:%f\t",pitch_deg);
    ROS_INFO("yaw degrees:%f\n",yaw_deg);
    double yaw_normalised=fmod((yaw_deg+360.0),360.0);                                                                                                                         //modulo only takes integer operands in c,cpp
    //ROS_INFO("roll normalised:%f\t",roll_deg);
    //ROS_INFO("pitch normalised:%f\t",pitch_deg);
    ROS_INFO("yaw normalised:%f\n",yaw_normalised);
    
    
    double difference1= fmod((bearing-yaw_normalised),360.0)+540.0;
    double difference2 =fmod(difference1,360.0)-180.0;
    
    if(difference2>0) 
        ROS_INFO("Difference is:%f, turn anticlockwise\n",difference2);
    else
        ROS_INFO("Difference is:%f, turn clockwise\n",difference2);

    //((((bearing - heading) % 360) + 540) % 360) - 180

}


int main(int argc, char **argv) {

    ros::init(argc, argv, "rpy_node");
    ros::NodeHandle nh;
//  gpssub = n.subscribe<sensor_msgs::NavSatFix>("/gps/fix", 100, gpscallback);
    ros::Subscriber imusub = nh.subscribe<sensor_msgs::Imu>("imu", 100, imu_callback);
    cout<<"enter bearing"<<endl;
    cin>>bearing;
    ros::Rate loop_rate(10);
    
    while(ros::ok()){
    ros::spinOnce();
    loop_rate.sleep();
    }
//    ros::spin();
    
    return 0;
    }
