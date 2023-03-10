/*
 * Author: Johan Vertens (vertensj@informatik.uni-freiburg.de)
 * Project: FreiCAR
 * Do NOT distribute this code to anyone outside the FreiCAR project
 */

#ifndef FREICAR_CONTROL_CONTROLLER_H
#define FREICAR_CONTROL_CONTROLLER_H

#include <filesystem>
#include <ros/package.h>

#include <string>
#include <cmath>
#include <algorithm>
#include <ros/ros.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2/transform_datatypes.h>
#include <tf2/transform_storage.h>
#include <tf2/buffer_core.h>
#include <tf2/convert.h>
#include <tf2/utils.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/Vector3.h>
#include <nav_msgs/Path.h>
#include <nav_msgs/Odometry.h>
#include "raiscar_msgs/ControlCommand.h"
#include "std_msgs/Bool.h"
#include "raiscar_msgs/ControllerPath.h"
#include "freicar_common/FreiCarControl.h"

class PID{
public:
    PID(double p, double i, double d){
        p_ = p;
        i_ = i;
        d_ = d;
        prev_t = ros::Time::now();
        prev_e = 0.0;
        integral = 0.0;
    };

    float step(const float error, const ros::Time stamp);
    void resetIntegral();

private:
    double p_;
    double i_;
    double d_;
    double integral;
    double prev_e;
    ros::Time prev_t;

};

class controller {
public:
    controller();
    virtual void controller_step(nav_msgs::Odometry odom);
    void receivePath(raiscar_msgs::ControllerPath new_path);
    void sendGoalMsg(const bool reached);
    void stop_step(std_msgs::Bool stopline_status);
    void getRightOfWay(std_msgs::Bool right_of_way_status);
    void getStandingVehicle(std_msgs::Bool standing_vehicle);
    void ExtControlCallback(const freicar_common::FreiCarControl::ConstPtr &ctrl_cmd);
    void LoadParametersFromYaml();
    std::vector<tf2::Transform> transformPath(nav_msgs::Path &path, const std::string target_frame);
    std::vector<tf2::Transform> discretizePath(std::vector<tf2::Transform> &path, float dist);
    // Vehicle parameters
    double L_;
    double ld_dist_;
    // Algorithm variables
    // Position tolerace is measured along the x-axis of the robot!
    double pos_tol_;
    // Control variables for Ackermann steering
    // Steering angle is denoted by delta
    double delta_max_;
    std::vector<tf2::Transform> path_;
    unsigned idx_;
    bool flag;
    int count = 0;
    bool goal_reached_;
    bool completion_advertised_;
    bool IsVehicleStanding,Request_Overtake_Plan;
    float max_position_error = 10.0;
    std::vector<float>  error_list;
    raiscar_msgs::ControlCommand cmd_control_;
    std::vector<float> _path_desc;

//    std::vector<float> D_;

    // Ros infrastructure
    ros::NodeHandle nh_, nh_private_;
    ros::Subscriber sub_odom_, sub_path_, sub_stop_status, sub_right_of_way_status,Standing_vehicle;
    ros::Subscriber external_control_sub;
    ros::Publisher pub_acker_, pub_goal_reached_, pub_overtake_request_;
    tf2_ros::Buffer tf_buffer_;
    tf2_ros::TransformListener tf_listener_;
    tf2_ros::TransformBroadcaster tf_broadcaster_;

    geometry_msgs::TransformStamped target_p_;
    std::string map_frame_id_, front_axis_frame_id_, rear_axis_frame_id_, target_frame_id_, tracker_frame_id;
    float current_steering_angle_;

    PID vel_pid;
    float vmax_;
    float des_v_;
    float throttle_limit_, curvature_vel_limit_factor, steering_vel_limit_factor, dist_vel_limit_factor, minimum_throttle_limit;
    float pos_error;
    bool car_stop_status;
    bool rightOfWay_status;
    int current_gt_index;
    int number_of_retries;
    int standing_vehicle_counter;
    std::string stp_resume_cmd;

    geometry_msgs::TransformStamped projection_t_;
    bool use_yaml_spawn_;
};



#endif //FREICAR_CONTROL_CONTROLLER_H