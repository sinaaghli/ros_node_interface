#include <stdio.h>
#include <Node/Node.h>
#include "../../../build/ros_node_interface/NinjaMsgs.pb.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "ackermann_msgs/AckermannDriveStamped.h"

node::node  ninja_node;
NinjaCommandMsg cmd;

int main(int argc, char **argv)
{
  bool bNodeSubsribed = false;
  ackermann_msgs::AckermannDriveStamped msg;

  // Initialize Ros
  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<ackermann_msgs::AckermannDriveStamped>("/ackermann_vehicle/ackermann_cmd", 1000);
  ros::Rate loop_rate(10);

  // Initialize Node
  ninja_node.init("ninja_node");

  // advertise that we will transmit state data
  if( ninja_node.advertise("state") == false ) {
    printf("Error setting publisher.\n");
  }

  while (ros::ok())
  {
      // subscribe to the ninja_command topic
      if( !bNodeSubsribed ){
        if( ninja_node.subscribe("commander_node/command") == false ){
          printf("Error subscribing to commander_node/command.\n");
        }else{
          bNodeSubsribed = true;
        }
      }

      // let's see what the controller is telling us to do
      NinjaCommandMsg cmd;
      if( ninja_node.receive("commander_node/command", cmd) ){
        // relay command to the car
        msg.drive.speed = cmd.speed();
        msg.drive.steering_angle = cmd.turnrate();
        ROS_INFO("speed:%.2f  steering_angle:%.2f",msg.drive.speed,msg.drive.steering_angle);
        chatter_pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
     }
  }


  return 0;
}
