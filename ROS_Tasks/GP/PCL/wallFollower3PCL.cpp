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

#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud2.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>

#include<tf/tf.h>
#include <tf/transform_datatypes.h>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <pcl_ros/transforms.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/io/pcd_io.h>

#include <pcl/filters/passthrough.h> 
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/common/common.h> 

#include <pcl/segmentation/extract_clusters.h>
#include <pcl/features/normal_3d.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/features/normal_3d.h>




using namespace std;
class wallFollower{


    public:

        wallFollower(){
           
            twist_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 10);
            imu_sub = nh.subscribe<sensor_msgs::Imu>("imu",10,&wallFollower::imuCallback,this);
            gps_sub= nh.subscribe<sensor_msgs::NavSatFix>("fix",10,&wallFollower::gpsCallback,this);
            //lidar_sub= nh.subscribe<sensor_msgs::LaserScan>("lidar_scan",10,&wallFollower::lidarCallback,this);
            processed_pcl_sub=nh.subscribe<sensor_msgs::PointCloud2>("output_cloud",10,&wallFollower::pclCallback,this);
            
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
        ros::Subscriber imu_sub,gps_sub,lidar_sub,processed_pcl_sub;   
        
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
     

        double boundary=1.5,closest_dist,closest_angle;
        double tooCloseThreshold=0.8;
        //float left,right,front;
        //float tooCloseThreshold=0.8;

        bool reached=false;
        bool onLeft=false,onRight=false,inFront=false,obstacle_det;
       
        int state=0; //0 means no normal, 1 means normal global navigation
       
        std::vector<pcl::PointCloud<pcl::PointXYZ>::Ptr> cluster_list;
        std::vector<double> closest_distances;
        std::vector<pcl::PointXYZ> closest_points;

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

        void pclCallback(const sensor_msgs::PointCloud2::ConstPtr& pcl_data){
                
            pcl::PointCloud<pcl::PointXYZ>::Ptr output_cloud(new pcl::PointCloud<pcl::PointXYZ>);
               
            pcl::fromROSMsg(*pcl_data, *output_cloud);

            pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);
            tree->setInputCloud (output_cloud);

            std::vector<pcl::PointIndices> cluster_indices;
            pcl::EuclideanClusterExtraction<pcl::PointXYZ> ec;
            ec.setClusterTolerance(0.1); 
            ec.setMinClusterSize(5);
            ec.setMaxClusterSize(50000);
            ec.setSearchMethod(tree);
            ec.setInputCloud(output_cloud);
            ec.extract(cluster_indices);

            
        
            
            //double boundary=1.5,x_val,y_val,z_val;
            
     
            
            int clust_ind=0;

            pcl::PointCloud<pcl::PointXYZ>::Ptr output_cloud_clustered(new pcl::PointCloud<pcl::PointXYZ>);
            
            //find closest cluster to determine case for turning at that instant
            

            for (const auto& cluster : cluster_indices){
                
                pcl::PointCloud<pcl::PointXYZ>::Ptr cluster_cloud(new pcl::PointCloud<pcl::PointXYZ>);
                
                for (const auto& index : cluster.indices){
                        
                        cluster_cloud->push_back((*output_cloud)[index]);                    
                    }
                    //make each cluster thing a row-vector so it's easier to deal with.
                    /*cluster_cloud->width = cloud_cluster->points.size();
                    cluster_cloud->height = 1;*/
                    
                //ROS_INFO("Cluster %d is at %f ", clust_ind++,closest_dist);
                *output_cloud_clustered += *cluster_cloud;
                cluster_list.push_back(cluster_cloud);

            }


            for(const auto& cluster :cluster_list){
                
                closest_dist=999999;
                pcl::PointXYZ closest_point;

                for (auto& point: *cluster) {
                        
                        double distance = std::sqrt(std::pow(point.x, 2) + std::pow(point.y, 2) + std::pow(point.z, 2));
                        if (distance < closest_dist) {
                            closest_dist = distance;
                            closest_point = point;
                        
                    }
                }
                closest_points.push_back(closest_point);
                
            }
        

        //Findng the cluster corresponding to the shortest overall distance 
        pcl::PointXYZ overall_closest_point;
        closest_dist=999999;

        for (const auto& point : closest_points) {
            double distance = std::sqrt(std::pow(point.x, 2) + std::pow(point.y, 2) + std::pow(point.z, 2));
            
            if (distance < closest_dist) {
                closest_dist = distance;
                overall_closest_point = point;
            }
        }

            closest_angle=atan2(overall_closest_point.z,overall_closest_point.x);
            
            //Cases:
            
            ROS_INFO("Closest point is at a distance of: %f and an angle of %f",closest_dist,closest_angle);
            
            closest_angle*=180.0/M_PI;
            if(closest_angle<60&&closest_angle>0&&closest_dist<boundary)
                {onRight=true;}
            if(closest_angle>60&&closest_angle>120&&closest_dist<boundary)
                {inFront=true;
                obstacle_det=closest_dist<boundary-0.25?true:false;}
            if(closest_angle>120&&closest_angle<180&&closest_dist<boundary)
                {onLeft=true;}
            
            
                
        


    }

        //Obstacle detection with LIDAR
       /*void lidarCallback(const sensor_msgs::LaserScan::ConstPtr& lidar_data) {
        
            float boundary=1.5;
            auto i=begin(lidar_data->ranges); //iterator
            onLeft=onRight=inFront=false;
            /*float frontC,left,right,frontR,frontL;
            right=min(*min_element(i,i+81),10.0f);
            frontR=min(*min_element(i+82,i+117),10.0f);
            frontC=min(*min_element(i+118,i+241),10.0f);
            frontL=min(*min_element(i+242,i+277),10.0f);
            left=min(*min_element(i+278,i+359),10.0f);*/

            /*   float boundary=1.5;
            auto i=begin(lidar_data->ranges); //iterator
            float frontC,left,right,frontR,frontL;
            right=min(*min_element(i,i+71),10.0f);
            frontR=min(*min_element(i+72,i+107),10.0f);
            frontC=min(*min_element(i+108,i+251),10.0f);
            frontL=min(*min_element(i+252,i+287),10.0f);
            left=min(*min_element(i+288,i+359),10.0f);*/
            


            //Didn't work properly, split into more cases and see
           
            //Didn't work properly, split into more cases and see
           /*if(frontL < boundary &&  frontC> boundary && frontR > boundary ){
                ROS_INFO("Front left");
                //turnLeft();
                
            }

            else if(frontL > boundary &&  frontC < boundary && frontR > boundary ){
                ROS_INFO("front centre");
                //turnLeft();
               
            }

            else if(frontL > boundary &&  frontC > boundary && frontR < boundary){
                ROS_INFO("front right");
                //turnLeft();
                //keepMoving();
            }


            else if(frontL > boundary &&  frontC < boundary && frontR < boundary ){
                ROS_INFO("front right, front centre");
                //turnLeft();
                
            }

            else if(frontL < boundary &&  frontC > boundary && frontR < boundary ){
                ROS_INFO("front left, front right blocked");
                //keepMoving();
               
            }

            else if(frontL < boundary &&  frontC < boundary && frontR > boundary ){
                ROS_INFO("front left, front centre");
                //turnLeft();
               
            }
            else if(frontL < boundary &&  frontC < boundary && frontR < boundary ){
                ROS_INFO("front left front centre front right");
               /* twist.linear.x = -0.3;
                twist.angular.z = 0.1;
                twist_pub.publish(twist);*/
                
            //}
            //else if(frontC > boundary && frontL >boundary && frontR>boundary /*&& right>boundary && left>boundary*/){
              //  ROS_INFO("All clear");
            //}

        
            /*right=min(*min_element(i,i+143),10.0f);
            front=min(*min_element(i+144,i+215),10.0f);
            left=min(*min_element(i+216,i+359),10.0f);*/
           
            
            
            //000,001,010,011,100,101,110,111 right front left

          
          

            
/*}*/

          
        
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

    bool isTooClose(){
        return (closest_dist<tooCloseThreshold);
    }

    void wallFollow(){
        determineHaversineData(lat_current,long_current);
        if(distance<0.8)
           {    reached=true;
                twist.linear.x=0;
                twist.angular.z=0;
                twist_pub.publish(twist);
                ROS_INFO("Reached destination.");

           }
        else{
            
            if(inFront&&!onLeft&&!onRight)
            {
                ROS_INFO("Front");
                ROS_INFO("Should turn left in this scenario");
                if(isTooClose())
                quickLeft();
                else
                gradualLeft();

                return;
            }
             else if(onRight&&!onLeft&&!inFront)//4
            {
                ROS_INFO("Right");
               
                ROS_INFO("Should turn left in this case");
                if(isTooClose())
                quickLeft();
                else gradualLeft();
                return;
            }
            else if(onRight&&inFront&&!onLeft)//6
            {
                ROS_INFO("Front and Right");
                ROS_INFO("Should turn left in this case");
                if(isTooClose())
                quickLeft();
                else
                gradualLeft();
                return;
            }
           
            else if(!onRight&&!inFront&&onLeft)//1
            {
                ROS_INFO("Left");
                ROS_INFO("Should go straight in this scenario");
                keepMoving();
                return;
            }
            
            else if(onLeft&&onRight&&!inFront)//5
            {
                ROS_INFO("Left and Right blocked");
              
                ROS_INFO("Should keep going straight here");
                keepMoving();
                return;
            }

            else if(onLeft&&inFront&&!onRight)//3
            {
                ROS_INFO("Front and Left");
               
                ROS_INFO("Should turn right in this case");
                if(isTooClose())
                quickRight();
                else
                gradualRight();
                return;
            }
            
            else if(onRight&&inFront&&onLeft&&closest_dist<boundary-0.5)
            {
                ROS_INFO("Should turn around");
                onLeft=onRight=inFront=true;
                twist.linear.x=-0.05;
                twist.angular.z=-0.3;
                twist_pub.publish(twist);
                return;
            }
            else if (!onRight&&!onLeft&&!inFront)//0
            {
                ROS_INFO("All clear");
               
                ROS_INFO("Will resume navigation to point");
                state=1;//perform globalNavigation now
                return;
                
            }
        }
        
}
    
    /*bool isTooClose(){
        if(right < tooCloseThreshold || left < tooCloseThreshold)
            return true;
        return false;
    }*/

    void autoNav(){
        determineHaversineData(lat_current,long_current); //Get Haversine distance and Bearing.
                
            
                    
                   
                    ROS_INFO("autoNav running");
                    
                    if(distance<0.8){
                    
                    twist.linear.x=0;
                    twist.angular.z=0;
                    twist_pub.publish(twist);
                    ROS_INFO("Reached destination point.");
                    reached=true;
                    
                    //break;

                    }
                    if(!reached&&obstacle_det){
                        state=0; 
                         ROS_INFO("switching to obstacle case");
                         return;

                    }
                    else
                    {
                    ROS_INFO("No obstacle detected yet");
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
