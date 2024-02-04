/* 
### ROS OOP style reference :
    https://www.youtube.com/watch?v=BSRgwKAZKhs&pp=ygUNZGVlcCBkYXJrIHJvcw%3D%3D (Deep Dark ROS)
    htps://nu-msr.github.io/navigation_site/lectures/node_structure.html
    https://yuzhangbit.netlify.app/2018-10-01-several-ways-of-writing-a-ros-node/?highlight=several+ways
    https://github.com/yuzhangbit/ros_node_pattern/blob/master/demo/include/simple_node_class.hpp

    
### References:

    ### General motion control :
        https://f1tenth.org/slides/l2.pdf
        https://www.youtube.com/watch?v=2Qs6A3Zsbfk&t=72s
        https://www.youtube.com/watch?v=eJ4QPrYqMlw
        https://www.youtube.com/watch?v=2-Taue1Ue_8&list=PLLgA_SIipbdw8LemIRy4NYtLvMRlyIDqn&index=3
        https://docs.ros.org/en/melodic/api/robot_localization/html/integrating_gps.html

    ### Rotation logic:
        Determining direction of rotation: 
        https://stackoverflow.com/questions/16180595/find-the-angle-between-two-bearings
        
        (Rotate until yaw~bearing.)

    ### Haversine :
        https://stackoverflow.com/questions/1311049/how-to-map-atan2-to-degrees-0-360
        https://www.movable-type.co.uk/scripts/latlong.html
        https://www.math.ksu.edu/~dbski/writings/haversine.pdf

*/


#include<ros/ros.h>

#include<iostream>
#include <sstream>
#include<cmath>

#include<sensor_msgs/Imu.h>
#include<sensor_msgs/NavSatFix.h>
#include<geometry_msgs/Twist.h>
#include<tf/tf.h>

class globalPlanner{


    public:

        globalPlanner(){
            twist_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 10);
            imu_sub = nh.subscribe<sensor_msgs::Imu>("imu",10,&globalPlanner::imuCallback,this);
            gps_sub= nh.subscribe<sensor_msgs::NavSatFix>("fix",10,&globalPlanner::gpsCallback,this);
            
            ROS_INFO("Enter destination co-ordinates"/*(positive north and east)*/);
            
            std::cin>>lat_final>>long_final;
            
            ros::Rate loop_rate(100);
            //ros::spinOnce();

            while (ros::ok()){
                
                //Get current position and orientation
                ros::spinOnce();

                determineHaversineData(); //Get Haversine distance and Bearing.
                

                if(distance<0.1){
                    
                    twist.linear.x=0;
                    twist.angular.z=0;
                    twist_pub.publish(twist);
                    ROS_INFO("Reached destination point.");
                    break;

                }

                else if (fabs(bearing_normalised-yaw_normalised)<=0.07){
                
                    twist.linear.x=K_lin*(distance);
                    twist.angular.z=0;
                    //std::cout<<"Distance when angle is small:"<<distance<<std::endl;
                    //ROS_INFO("Distance left:%f",distance);
                }

                else if(fabs(bearing_normalised-yaw_normalised)>0.07){
                    
                    determineRotation();
                    
                    twist.linear.x=0.15;
                    twist.angular.z=K_rot*fabs(bearing_normalised-yaw_normalised);
                    
                    //ROS_INFO("Distance left:%f",distance);
                }

                twist_pub.publish(twist);
                loop_rate.sleep();
    
        }

                

    }
                


    private:

        // ROS Node
        ros::NodeHandle nh;
        
        // Publishers and subscribers
        ros::Publisher twist_pub;
        ros::Subscriber imu_sub,gps_sub;   
        
        // Message types
        geometry_msgs::Twist twist;
        
        
        /*sensor_msgs::NavSatFix current_location;
        sensor_msgs::Imu current_orientation;*/


        // Coordinate variables, heading and bearing variables, difference variables to determine rotation direction
        double lat_current,long_current;
        double lat_final, long_final;
        double distance,bearing, bearing_deg;
        double roll, pitch, yaw,roll_deg,pitch_deg,yaw_deg;
        double bearing_normalised, yaw_normalised;
        double diff1, diff2;  
        double K_rot,K_lin=0.3;

        // Getting Euler angles from Quaternion data
        void imuCallback(const sensor_msgs::Imu::ConstPtr& imu_data) {
    
            tf::Quaternion quaternion(imu_data->orientation.x, imu_data->orientation.y, imu_data->orientation.z, imu_data->orientation.w);
            tf::Matrix3x3 orientation(quaternion);
            
            orientation.getRPY(roll, pitch, yaw);
            
            //ROS_INFO("IMU Callback working");
            //ROS_INFO("roll:%f\tpitch:%f\tyaw:%f\n",roll,pitch,yaw);
            
        }
        
        // Setting current coordinates
        void gpsCallback(const sensor_msgs::NavSatFix::ConstPtr& gps_data){

            lat_current=gps_data->latitude;
            long_current=gps_data->longitude;
            //ROS_INFO("GPS Callback working");
            //ROS_INFO("Current latitude:%f\tCurrent longitude:%f\n",lat_current,long_current);
        }


        
        void determineHaversineData(/*double lat_current,double long_current, double lat_final, double long_final*/) {
            const double radius = 6378137.0;
            double lat_diff = (lat_final - lat_current) * M_PI / 180.0;
            double long_diff = (long_final - long_current) * M_PI / 180.0;
            
            double a = pow(sin(lat_diff / 2), 2) + cos(lat_current* M_PI / 180.0) * cos(lat_final * M_PI / 180.0) * pow(sin(long_diff / 2), 2);
            double c = 2 * atan2(sqrt(a), sqrt(1 - a));
            distance= c*radius;
            
            ROS_INFO("Distance to destination:%f",distance);

            double x = sin(long_diff) * cos(lat_final*M_PI/180.0);
            double y = cos(lat_current*M_PI/180.0) * sin(lat_final*M_PI/180.0) - sin(lat_current*M_PI/180.0) * cos(lat_final*M_PI/180.0) * cos(long_diff);
            bearing = atan2(y, x);
            
            
            bearing_deg=bearing*(180.0/M_PI);
            roll_deg=roll*(180.0/M_PI);
            pitch_deg=pitch*(180.0/M_PI);
            yaw_deg=yaw*(180.0/M_PI);
        
            //ROS_INFO("\nbearing_deg:%f\nroll_deg:%f\tpitch_deg:%f\tyaw_deg:%f\n",bearing_deg,roll_deg,pitch_deg,yaw_deg);
        
            //modulo only takes integer operands in c and c++
            yaw_normalised=fmod((yaw_deg+360.0),360.0);  
            bearing_normalised=fmod((bearing_deg+360.0),360.0);
            
            ROS_INFO("orientation:%f,required turn angle:%f\n",yaw_normalised, bearing_normalised-yaw_normalised);    
        }

        double determineRotation(){                                                      
    
            // Using: ((((bearing - heading) % 360) + 540) % 360) - 180                                                                                                                                              
            diff1= fmod((bearing_normalised-yaw_normalised),360.0)+540.0;
            diff2 =fmod(diff1,360.0)-180.0;
            
            if(diff2>0){
                K_rot=+0.2;
                //ROS_INFO(/*Difference is:%f,*/ "Turning anticlockwise\n"/*,difference2*/);
                    
            }
            
            else{
                    K_rot=-0.2;
                   // ROS_INFO(/*Difference is:%f, */"Turning clockwise\n"/*,difference2*/);
                    
            }

            return K_rot;

    }


};



int main(int argc, char** argv) {
    
    ros::init(argc, argv, "globalPlanner");
    
    globalPlanner planner;
    
    return 0;

}

