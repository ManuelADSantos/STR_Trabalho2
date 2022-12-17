#include <iostream>

#include <vector>
#include <array>

#include <ros/ros.h>
#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud_conversion.h>

ros::Publisher newPointCloud;
bool runflag = false;
sensor_msgs::PointCloud2::ConstPtr pointcloud;

//Create a std vector with a 3 double array
std::vector<std::array<double, 3>> points;

void handlePointCloud(sensor_msgs::PointCloud2::ConstPtr scan_out)
{
    pointcloud = scan_out;
    sensor_msgs::PointCloud output;
    sensor_msgs::convertPointCloud2ToPointCloud(*pointcloud, output);
    std::cout << "Points: " << scan_out->height * scan_out->width << std::endl;
    std::cout << "Points: " << output.points.size() << std::endl;
    runflag = true;
}


void f1(sensor_msgs::PointCloud2::ConstPtr pc)
{
    int width = pc->width;
    int height = pc->height;

    // Go through all points
    for (size_t i = 0; i < width * height; i++)
    {
        int arrayPosition = i * pc->point_step; // Get the position of the point in the array
        int arrayPosX = arrayPosition + pc->fields[0].offset; // X has an offset of 0
        int arrayPosY = arrayPosition + pc->fields[1].offset; // Y has an offset of 4
        int arrayPosZ = arrayPosition + pc->fields[2].offset; // Z has an offset of 8

        /**
         * |1|2|3|4|5|6|7|8|9|A|B|C|
         * |X X X X|Y Y Y Y|Z Z Z Z|
         * 
         */

        double X = 0.0;
        double Y = 0.0;
        double Z = 0.0;

        //Get the X, Y and Z values from the array
        memcpy(&X, &pc->data[arrayPosX], sizeof(double));
        memcpy(&Y, &pc->data[arrayPosY], sizeof(double));
        memcpy(&Z, &pc->data[arrayPosZ], sizeof(double));

        //Create a 3D point and add it to the vector
        std::array<double, 3> point = {X, Y, Z};
        points.push_back(point);
    }    
}


void f2()
{
    // Part I 2)
    // Go through all points
    // Starting from the end of the vector until the beginning
    for (size_t i = points.size(); i > 0; i--)
    {
        // Get the X, Y and Z values from the point
        double X = points[i][0];
        double Y = points[i][1];
        double Z = points[i][2];
        
        // check if the point is in the area to be deleted
        if ((X < 0.0) || ((X <= 2.0) && (abs(Y <= 0.2))) || (X > 15.0) || (abs(Y) > 10.0) || Y > 1.9 || (Y < -3 && X > 2 && X < 4 && Z > -0.8))
        {
            points.erase(points.begin() + i); // Delete the point  
        }
    }
}
void f3()
{
    double minZ = 0.0;
    double maxZ = 1.5;

    int invalid = 0;

    double xx, yy;

    xx = 0.0;
    yy = 0.0;

    double gridX = 20.0;
    double gridY = 20.0;
    double offset = 0.5;

    while (xx < gridX)
    {
        while (yy < gridY)
        {
            for (size_t i = 0; i < points.size(); i++)
            {
                // Get the X, Y and Z values from the point
                double X = points[i][0];
                double Y = points[i][1];
                double Z = points[i][2];

                // search for min and max values of z in the current grid cell
                if (((X >= xx) && (X <= (xx + offset))) && ((Y + 10.0 >= yy) && (Y + 10.0 <= (yy + offset))))
                {
                    if (Z < minZ)
                    {
                        minZ = points[i][2];
                    }
                    if (Z > maxZ)
                    {
                        maxZ = points[i][2];
                    }
                }
            }

            // mark the existence of invalid points
            if (abs(maxZ - minZ) < 0.1 || maxZ > 1.5)
                invalid = 1;

            if (invalid == 1)
            {
                for (size_t i = points.size(); i > 0; i--)
                {
                    // Get the X, Y and Z values from the point
                    double X = points[i][0];
                    double Y = points[i][1];
                    double Z = points[i][2];
                    
                    // remove unwanted points
                    if (((X>= xx) && (X<= (xx + offset))) && ((Y + 10.0 >= yy) && (Y + 10.0 <= (yy + offset))))
                    {
                        points.erase(points.begin() + i); // Delete the point   
                    }
                }
            }
            // reset min and max
            minZ = 0.0;
            maxZ = 1.5;

            yy += offset;
            invalid = 0;
        }
        xx += offset;
        yy = 0.0;
    }


    //define the output message
    sensor_msgs::PointCloud output;

    //set the size of the output message
    for (size_t i = 0; i < points.size(); i++)
    {
        output.data[i].x = points[i][0];
        output.data[i].y = points[i][1];
        output.data[i].z = points[i][2];
    }
    
    output.header = pointcloud->header; // set message header (using the same from /velodyne_points)
    //Part I 3)
    newPointCloud.publish(output);
}
int main(int argc, char **argv)
{
    ros::init(argc, argv, "strdemo");
    ros::NodeHandle nh("~");
    newPointCloud = nh.advertise<sensor_msgs::PointCloud>("/output_results", 100);
    ros::Subscriber PointCloudHandlervelodyne = nh.subscribe<sensor_msgs::PointCloud2>("/velodyne_points", 100, handlePointCloud);
    ros::Rate rate(100.0);
    while (nh.ok())
    {
        if (runflag)
        {
            f1(pointcloud);
            f2();
            f3();
            runflag = false;
        }
        ros::spinOnce();
        rate.sleep();
    }
return 1;
}