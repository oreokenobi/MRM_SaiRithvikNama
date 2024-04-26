#!/usr/bin/env python3
from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QWidget,
    QVBoxLayout,
    QLabel,
    QPushButton,
    QMessageBox,
    QHBoxLayout,
)

from PyQt5.QtGui import QPixmap, QImage, QColor
from PyQt5.QtCore import QTimer, Qt, QObject, pyqtSignal
from sensor_msgs.msg import Imu, NavSatFix, LaserScan, Image
from tf.transformations import quaternion_from_euler, euler_from_quaternion

# from rover_package.srv import imurequest


import rospy
import sys
import roslib
import rviz

import cv2 as cv
from cv_bridge import CvBridge
import csv
import os
import pandas as pd


class MainWindow(QMainWindow):
    counter = 1

    def __init__(self):
        super(MainWindow, self).__init__()

        self.imu_text_data = None
        self.gps_data = None
        self.show_quaternions = True
        self.is_recording = False
        self.video_writer = None
        self.video_frame = None
        self.log_files = []
        self.bridge = CvBridge()

        rospy.init_node("gui_trial_3")
        rospy.Subscriber("/imu", Imu, self.imu_callback)
        rospy.Subscriber("/fix", NavSatFix, self.gps_callback)
        rospy.Subscriber("/lidar_scan", LaserScan, self.lidar_callback)
        rospy.Subscriber("/camera1/image_raw", Image, self.camera_callback)

        self.setWindowTitle("GUI/IMU Task")

        self.dummy_widget = QWidget()
        self.setCentralWidget(self.dummy_widget)
        self.outer_layout = QVBoxLayout(self.dummy_widget)
        self.inner_layout = QHBoxLayout()

        self.gps_text = QLabel("GPS Data:")
        self.imu_text = QLabel("IMU Data:")
        self.video_feed_label = QLabel()
        self.video_feed_label.setFixedSize(600, 300)
        self.obstacle_status_text = QLabel("")
        self.video = None
        self.video_writer = None

        self.toggle_button = QPushButton("Toggle IMU Data")
        self.toggle_button.clicked.connect(self.toggle_imu_data)

        self.take_screenshot_button = QPushButton("Take a screenshot")
        self.take_screenshot_button.clicked.connect(self.take_screenshot)

        self.take_video_button = QPushButton("Record a video")
        self.take_video_button.clicked.connect(self.take_video)

        """self.rviz_frame = rviz.VisualizationFrame()
        self.rviz_frame.setSplashPath("")
        self.rviz_frame.initialize()

        self.rviz_manager = self.rviz_frame.getManager()
        self.rviz_manager.initialize()

        self.outer_layout.addWidget(self.rviz_frame)

        try:
            self.imu_display = self.rviz_manager.createDisplay("rviz/Imu", "IMU", True)
            if self.imu_display is None:
                raise RuntimeError("Failed to create RViz IMU display")
        except RuntimeError as e:
            print("Error:", e)

        try:
            self.imu_display.subProp("Topic").setValue("/imu_text_data_topic")
        except AttributeError as e:
            print("Error:", e)

        self.inner_layout.addWidget(self.video_feed_label)
        self.inner_layout.addWidget(self.imu_display)"""

        self.outer_layout.addLayout(self.inner_layout)

        self.outer_layout.addWidget(self.video_feed_label)

        self.outer_layout.addWidget(self.gps_text)
        self.outer_layout.addWidget(self.imu_text)
        self.outer_layout.addWidget(self.obstacle_status_text)

        self.inner_layout.addWidget(self.toggle_button)
        self.inner_layout.addWidget(self.take_screenshot_button)
        self.inner_layout.addWidget(self.take_video_button)

    def gps_callback(self, msg):
        self.gps_text.setText(
            f"GPS Data:\n Latitude: {msg.latitude}\t Longitude: {msg.longitude}"
        )
        self.log_files.append({"GPS->": (msg.latitude, msg.longitude)})

    def toggle_imu_data(self):
        self.show_quaternions = not self.show_quaternions

    def imu_callback(self, msg):
        self.imu_text_data = msg
        if self.imu_text_data:
            imu_text_data = f"Linear Acceleration: {msg.linear_acceleration}\nAngular Velocity: {msg.angular_velocity}\n"

            if self.show_quaternions:
                imu_text_data += f"Orientation:  W={msg.orientation.w}, X={msg.orientation.x},Y={msg.orientation.y}, Z={msg.orientation.z}"

            elif not self.show_quaternions:
                # rospy.wait_for_service("toggle_orientation")
                # try:
                # response = rospy.ServiceProxy("toggle_orientation", imuSwitchRequest)
                quaternion_data = (
                    msg.orientation.w,
                    msg.orientation.x,
                    msg.orientation.y,
                    msg.orientation.z,
                )

                roll, pitch, yaw = euler_from_quaternion(quaternion_data)
                imu_text_data += (
                    f"Orientation: Roll= {roll}, Pitch= {pitch}, Yaw= {yaw}"
                )
            # except rospy.ServiceException as e:
            #    print(e)

        self.imu_text.setText(f"IMU:\n {imu_text_data}")

        self.log_files.append(
            {
                "Orientation->": (
                    msg.orientation.x,
                    msg.orientation.y,
                    msg.orientation.z,
                    msg.orientation.w,
                ),
                "Linear Acceleration->": (
                    msg.linear_acceleration.x,
                    msg.linear_acceleration.y,
                    msg.linear_acceleration.z,
                ),
                "Angular Velocity->": (
                    msg.angular_velocity.x,
                    msg.angular_velocity.y,
                    msg.angular_velocity.z,
                ),
            }
        )

    def lidar_callback(self, msg):

        d = 1.65

        right = min(min(msg.ranges[0:143]), 10)
        front = min(min(msg.ranges[144:215]), 10)
        left = min(min(msg.ranges[216:359]), 10)
        text = " "
        if right > d and front < d and left > d:  # 2
            text = "Front"
            self.obstacle_status_text.setText(text)

        elif right < d and front > d and left > d:  # 4
            text = "Right"
            self.obstacle_status_text.setText(text)

        elif right < d and front < d and left > d:  # 6
            text = "Front and Right"
            self.obstacle_status_text.setText(text)

        elif right > d and front > d and left < d:  # 1
            text = "Left"
            self.obstacle_status_text.setText(text)

        elif right < d and front > d and left < d:  # 5
            text = "Left and Right"
            self.obstacle_status_text.setText(text)

        elif right > d and front < d and left < d:  # 3
            text = "Front and Left"
            self.obstacle_status_text.setText(text)

        elif right < d - 0.65 and front < d and left < d - 0.65:  # 7
            text = "All Around"
            self.obstacle_status_text.setText(text)

        elif right > d and front > d and left > d:  # 0
            text = "All Clear"
            self.obstacle_status_text.setText(text)

        self.log_files.append({"Obstacle Detection Status is": text})

    def camera_callback(self, msg):
        cv_image = self.bridge.imgmsg_to_cv2(msg, "bgr8")
        cv_image_rgb = cv.cvtColor(cv_image, cv.COLOR_BGR2RGB)
        height, width, channels = cv_image.shape
        bytes_per_line = 3 * width
        q_image = QImage(
            cv_image_rgb.data, width, height, bytes_per_line, QImage.Format_RGB888
        )
        pixmap = QPixmap.fromImage(q_image)
        self.video_feed_label.setPixmap(pixmap)

        # Write to video if recording
        if self.is_recording:
            if self.video_writer is None:
                fourcc = cv.VideoWriter_fourcc(*"XVID")
                self.video_writer = cv.VideoWriter(
                    "/home/rithvik/Desktop/recorded_video.avi",
                    fourcc,
                    20.0,
                    (width, height),
                )

            self.video_writer.write(cv_image)

    def take_screenshot(self):
        pixmap = self.video_feed_label.pixmap()
        if pixmap:
            save_file = (
                f"/home/rithvik/Desktop/gui_task_pics/screenshot_{self.counter}.png"
            )
            pixmap.save(save_file)
            self.counter += 1

    def take_video(self):
        if not self.is_recording:
            self.is_recording = True
            self.take_video_button.setText("Stop Recording")
            self.take_video_button.setStyleSheet("background-color:red")
        else:
            self.is_recording = False
            self.take_video_button.setText("Record a Video")
            self.take_video_button.setStyleSheet("background-color:white")
            if self.video_writer:
                self.video_writer.release()
                self.video_writer = None

    def closeEvent(self, event):
        if len(self.log_files) > 0:
            df = pd.DataFrame(self.log_files)
            save_file = "/home/rithvik/Desktop/log_file.csv"
            df.to_csv(f"{save_file}", index=False)
            print("Saved log file")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
