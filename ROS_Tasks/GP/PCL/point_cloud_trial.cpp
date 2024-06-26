
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
#include <pcl/segmentation/extract_clusters.h>

#include <pcl/features/normal_3d.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/features/normal_3d.h>
#include <pcl/common/common.h> 

#include <ros/ros.h>

#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud2.h>

#include <tf/transform_datatypes.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>

#include <cmath>
#include <algorithm>


class PCL_Processor{
    
    public:

    PCL_Processor(){


        pcl_data_pub=nh.advertise<sensor_msgs::PointCloud2>("/output_cloud", 100);
        pcl_data_sub=nh.subscribe("zed2/depth/points", 10, &PCL_Processor::pclCallback,this);

        ros::Rate loop_rate(100);
            

            while (ros::ok()){
                
                ros::spinOnce();
                loop_rate.sleep();
    
            }


    }

    //camera's spitting out /depth/points thing with Point<T> type data, you need to publish sensor_msgs/PCLPointCloud2 
    //can directly be converted from XYZ to PCL2 etc
    //need to convert to XYZ to access indices/points 


    private:

        // ROS Node
        ros::NodeHandle nh;
        
        // Publishers and subscribers
        ros::Publisher pcl_data_pub;
        ros::Subscriber pcl_data_sub;

    
        void pclCallback(const sensor_msgs::PointCloud2::ConstPtr &pcl_data){


            pcl::PointCloud<pcl::PointXYZ>::Ptr input_cloud(new pcl::PointCloud<pcl::PointXYZ>);
            pcl::PointCloud<pcl::PointXYZ>::Ptr output_cloud(new pcl::PointCloud<pcl::PointXYZ>);    
            pcl::fromROSMsg(*pcl_data, *input_cloud);
            
            sensor_msgs::PointCloud2 output_pcl2;

            //Voxel thing
            pcl::VoxelGrid<pcl::PointXYZ> voxel_grid;
            voxel_grid.setInputCloud(input_cloud);
            voxel_grid.setLeafSize(0.1, 0.1, 0.1); 
            voxel_grid.filter(*output_cloud);
            
            //Passthrough filter
            pcl::PassThrough<pcl::PointXYZ> pt;
            pt.setInputCloud (output_cloud);
            pt.setFilterFieldName ("z");
            pt.setFilterLimits(0.0, 5.0);
            pt.filter(*output_cloud);

            //Finding ground plane points and getting those indices(We find the largest plane which ideally would be the ground plane)
            pcl::ModelCoefficients::Ptr coefficients(new pcl::ModelCoefficients);
            pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
            pcl::SACSegmentation<pcl::PointXYZ> seg;

	 
            seg.setOptimizeCoefficients(true);
            seg.setModelType(pcl::SACMODEL_PLANE);
            seg.setMethodType(pcl::SAC_RANSAC);
            seg.setMaxIterations(1000);
            seg.setDistanceThreshold(0.1); 
            seg.setInputCloud(output_cloud);
            seg.segment(*inliers, *coefficients);

            pcl::ExtractIndices<pcl::PointXYZ> extract;
            extract.setInputCloud(output_cloud);
            extract.setIndices(inliers);
            extract.setNegative(true);
            extract.filter(*output_cloud);

            
            pcl::toROSMsg(*output_cloud, output_pcl2);

            output_pcl2.header = pcl_data->header;
            pcl_data_pub.publish(output_pcl2);

        }

};



int main(int argc, char** argv) {
    
    ros::init(argc, argv, "PCL_Processor");
    
    PCL_Processor processor;
    
    return 0;

}
