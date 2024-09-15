#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <geometry_msgs/PoseStamped.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// サブスクライバのコールバック関数
void callback(const geometry_msgs::PoseStamped::ConstPtr& msg) {
    auto pos = msg->pose.position;
    auto orient = msg->pose.orientation;
    ROS_INFO("%f,%f,0.0,0.0,0.0,%f,%f,", pos.x, pos.y, orient.z, orient.w);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "waypoint_manager");
    ros::NodeHandle nh;

    ros::Publisher pub = nh.advertise<visualization_msgs::Marker>("waypoint", 10);
    ros::Subscriber sub = nh.subscribe("/move_base_simple/goal", 10, callback); // 変更：アクションを使わずにサブスクライブ

    ros::Rate rate(25);

    while (ros::ok()) {
        std::ifstream file("/home/yamaguchi-a/catkin_ws/src/waypoints_visualizer/csv/waypoints.csv");
        if (!file.is_open()) {
            ROS_ERROR("waypoint.csvのオープンに失敗しました");
            return 1;
        }

        std::string line;
        int counter = 0;

        // ヘッダー行をスキップ
        std::getline(file, line);

        while (std::getline(file, line)) {
            std::istringstream stream(line);
            std::vector<float> row;
            std::string cell;
            while (std::getline(stream, cell, ',')) {
                row.push_back(std::stof(cell));
            }

            // 列数が8未満の場合はエラー
            if (row.size() < 8) {
                ROS_ERROR("行のフォーマットが不正です");
                continue;
            }

            // 矢印マーカーの設定
            visualization_msgs::Marker marker;
            marker.header.frame_id = "map";
            marker.header.stamp = ros::Time::now();
            marker.ns = "basic_shapes";
            marker.id = counter++;
            marker.action = visualization_msgs::Marker::ADD;
            marker.pose.position.x = row[1];
            marker.pose.position.y = row[2];
            marker.pose.position.z = row[3];
            marker.pose.orientation.x = row[4];
            marker.pose.orientation.y = row[5];
            marker.pose.orientation.z = row[6];
            marker.pose.orientation.w = row[7];
            marker.color.r = 1.0;
            marker.color.g = 0.0;
            marker.color.b = 0.0;
            marker.color.a = 1.0;
            marker.scale.x = 0.5;
            marker.scale.y = 0.5;
            marker.scale.z = 0.1;
            marker.lifetime = ros::Duration();
            marker.type = visualization_msgs::Marker::ARROW;
            pub.publish(marker);

            // テキストマーカーの設定
            marker.ns = "basic_shapes";
            marker.id = counter++;
            marker.pose.position.x = row[1];
            marker.pose.position.y = row[2];
            marker.pose.position.z = row[3];
            marker.scale.x = 5.0;
            marker.scale.y = 5.0;
            marker.scale.z = 5.0;
            marker.color.r = 0.0;
            marker.color.g = 0.0;
            marker.color.b = 0.0;
            marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
            marker.text = std::to_string(static_cast<int>(row[0]));
            pub.publish(marker);
        }

        file.close();
        ros::spinOnce();  // コールバックを適切に呼び出す
        rate.sleep();
    }

    return 0;
}
