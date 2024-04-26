/*Follow priority order: First right, then front, then centre
https://www.youtube.com/watch?v=LB99PqeIIMw		Priority order scenario
https://www.youtube.com/watch?v=PrUjjPVVT6s		Cases*/

/*Regular wall-following+GlobalPlanner attempt*/


#include<ros/ros.h>

#include<cmath>
#include<iostream>
#include<sstream>
#include<algorithm>
#include<iterator>

#include<sensor_msgs/Imu.h>
#include<sensor_msgs/NavSatFix.h>
#include<sensor_msgs/LaserScan.h>
#include<geometry_msgs/Twist.h>
#include<tf/tf.h>

using namespace std;
class wallFollower{


    public:

        wallFollower(){
           
        twist_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 10);
        imu_sub = nh.subscribe<sensor_msgs::Imu>("imu",10,&wallFollower::imuCallback,this);
        gps_sub= nh.subscribe<sensor_msgs::NavSatFix>("fix",10,&wallFollower::gpsCallback,this);
        lidar_sub= nh.subscribe<sensor_msgs::LaserScan>("lidar_scan",10,&wallFollower::lidarCallback,this);
            ROS_INFO("Enter destination co-ordinates"/*(positive north and east)*/);
            
            cin>>lat_final>>long_final;
            
            ros::Rate loop_rate(2);
            //ros::spinOnce();

            while (ros::ok()){
                
                //Get current position and orientation
                ros::spinOnce();
                
                if(state==0)
                wallFollow();
                else if(state==1)
                autoNav();
                loop_rate.sleep();
    
        }

    }
                


    private:

        // ROS Node
        ros::NodeHandle nh;
        
        // Publishers and subscribers
        ros::Publisher twist_pub;
        ros::Subscriber imu_sub,gps_sub,lidar_sub;   
        
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

        float boundary=1.5;
        float left,right,front;
        float tooCloseThreshold=0.8;


        bool reached=false;
        bool onLeft,onRight,inFront; 
        
        int state=0; //0 means no normal, 1 means normal global navigation
       

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

        //Obstacle detection with LIDAR
       void lidarCallback(const sensor_msgs::LaserScan::ConstPtr& lidar_data) {
        
            float boundary=1.5;
            auto i=begin(lidar_data->ranges); //iterator
            onLeft=onRight=inFront=false;
           
            right=min(*min_element(i,i+143),10.0f);
            front=min(*min_element(i+144,i+215),10.0f);
            left=min(*min_element(i+216,i+359),10.0f);
           
            //000,001,010,011,100,101,110,111 right front left

            
}

          
        
        void determineHaversineData(double lat_c,double long_c) {
            const double radius = 6378137.0;
            double lat_diff = (lat_final - lat_c) * M_PI / 180.0;
            double long_diff = (long_final - long_c) * M_PI / 180.0;
            
            double a = pow(sin(lat_diff / 2), 2) + cos(lat_current* M_PI / 180.0) * cos(lat_final * M_PI / 180.0) * pow(sin(long_diff / 2), 2);
            double c = 2 * atan2(sqrt(a), sqrt(1 - a));
            distance= c*radius;
            
            ROS_INFO("Distance to destination:%f",distance);

            double x = sin(long_diff) * cos(lat_final*M_PI/180.0);
            double y = cos(lat_c*M_PI/180.0) * sin(lat_final*M_PI/180.0) - sin(lat_c*M_PI/180.0) * cos(lat_final*M_PI/180.0) * cos(long_diff);
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
            
            K_rot=((diff2)/fabs(diff2))*0.2;

            return K_rot;

    }


    void wallFollow(){
        determineHaversineData(lat_current,long_current);
        if(distance<0.1)
           {    reached=true;
                twist.linear.x=0;
                twist.angular.z=0;
                twist_pub.publish(twist);
                ROS_INFO("Reached destination.");

           }
        else{
            
            if(right>boundary&&front<boundary&&left>boundary)//2
            {
                ROS_INFO("Front");
                inFront=true;
                onLeft=onRight=false;
                ROS_INFO("Should turn left in this scenario");
                if(isTooClose())
                quickLeft();
                else
                gradualLeft();

                return;
            }
             else if(right<boundary&&front>boundary&&left>boundary)//4
            {
                ROS_INFO("Right");
                onRight=true;
                onLeft=inFront=false;
                ROS_INFO("Should turn left in this case");
                if(isTooClose())
                quickLeft();
                else gradualLeft();
                return;
            }
            else if(right<boundary&&front<boundary&&left>boundary)//6
            {
                ROS_INFO("Front and Right");
                onRight=inFront=true;
                onLeft=false;
                ROS_INFO("Should turn left in this case");
                if(isTooClose())
                quickLeft();
                else
                gradualLeft();
                return;
            }
           
            else if(right>boundary&&front>boundary&&left<boundary)//1
            {
                ROS_INFO("Left");
                onLeft=true;
                inFront=onRight=false;
                ROS_INFO("Should go straight in this scenario");
                keepMoving();
                return;
            }
            
            else if(right<boundary&&front>boundary&&left<boundary)//5
            {
                ROS_INFO("Left and Right blocked");
                inFront=false;
                onLeft=onRight=true;    
                ROS_INFO("Should keep going straight here");
                keepMoving();
                return;
            }

            else if(right>boundary&&front<boundary&&left<boundary&&left<front)//3
            {
                ROS_INFO("Front and Left");
                onLeft=inFront=true;
                onRight=false;
                ROS_INFO("Should turn right in this case");
                if(isTooClose())
                quickRight();
                else
                gradualRight();
                return;
            }
            
            else if(right<boundary-0.5&&front<boundary&&left<boundary-0.5)//7
            {
                ROS_INFO("Should turn around");
                onLeft=onRight=inFront=true;
                twist.linear.x=-0.05;
                twist.angular.z=-0.3;
                twist_pub.publish(twist);
                return;
            }
            else if (right>boundary&&front>boundary&&left>boundary)//0
            {
                ROS_INFO("All clear");
                onLeft=onRight=inFront=false;
                ROS_INFO("Will resume navigation to point");
                state=1;//perform globalNavigation now
                return;
                
            }
        }
        
}
    
    bool isTooClose(){
        if(right < tooCloseThreshold || left < tooCloseThreshold)
            return true;
        return false;
    }

    void autoNav(){
        determineHaversineData(lat_current,long_current); //Get Haversine distance and Bearing.
                
               // ros::Rate rate(2);

               
                    
                    
                   
                    ROS_INFO("autoNav running");
                    
                    if(distance<0.1){
                    
                    twist.linear.x=0;
                    twist.angular.z=0;
                    twist_pub.publish(twist);
                    ROS_INFO("Reached destination point.");
                    reached=true;
                    
                    //break;

                    }
                    if(!reached&& front<boundary-0.25){
                        state=0; 
                         ROS_INFO("switching to obstacle case");
                         return;

                    }
                    else
                    {ROS_INFO("No obstacle detected yet");
                    if (!reached&&fabs(bearing_normalised-yaw_normalised)<=0.1){
                        ROS_INFO("Aligned with goal");
                
                    twist.linear.x=K_lin*(distance)*0.2;
                    twist.angular.z=0;
                    //std::cout<<"Distance when angle is small:"<<distance<<std::endl;
                    //ROS_INFO("Distance left:%f",distance);
                    }

                    else if(!reached&&fabs(bearing_normalised-yaw_normalised)>0.1){
                    ROS_INFO("Rotating to align with goal");
                    //determineRotation();
                    
                    twist.linear.x=0.15;
                    twist.angular.z=determineRotation();
                    
                    //ROS_INFO("Distance left:%f",distance);
                    }
                    }
                    twist_pub.publish(twist);
                    //rate.sleep();
                
    }

     void quickLeft(){

     
        twist.linear.x=0;
        twist.angular.z=0.3;
        twist_pub.publish(twist);

     }
     
     void quickRight(){

       
        twist.linear.x=0;
        twist.angular.z=-0.3;
        twist_pub.publish(twist);

     }
     void gradualLeft(){

     
        twist.linear.x=0.075;
        twist.angular.z=0.3;
        twist_pub.publish(twist);

     }
     
     void gradualRight(){

       
        twist.linear.x=0.075;
        twist.angular.z=-0.3;
        twist_pub.publish(twist);

     }
      void keepMoving(){

    
        twist.linear.x=0.2;
        twist.angular.z=0;
        twist_pub.publish(twist);

     }


};



int main(int argc, char** argv) {
    
    ros::init(argc, argv, "wallFollower");
    
    wallFollower follower;
    
    return 0;

}

