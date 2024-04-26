#!/usr/bin/env python3
import rospy
import socket
from sensor_msgs.msg import Imu

# from rover_package.srv import imurequest, imurequestresponse
from tf.transformations import quaternion_from_euler, euler_from_quaternion


host_addr = "172.20.10.3"
port = 1234

r = 0
p = 0
y = 0


def SwitchRequestCallback(request):
    global r, p, y
    return imurequestresponse(r, p, y)


def app_data_callback():
    global r, p, y
    imu_data_pub = rospy.Publisher("imu_data_topic", Imu, queue_size=10)
    rospy.init_node("imu_server", anonymous=True)
    # service = rospy.Service("toggle_orientation", imurequest, SwitchRequestCallback)
    rate = rospy.Rate(10)

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server_socket.bind((host_addr, port))
    server_socket.listen()

    rospy.loginfo(f"Server is at {host_addr}, port number's {port}\n")

    client_socket, client_addr = server_socket.accept()

    rospy.loginfo(f"Client's at {client_addr}\n")

    while not rospy.is_shutdown():
        try:
            data = client_socket.recv(1024)
            if not data:
                raise socket.timeout("No data, will timeout")
        except socket.timeout as e:
            rospy.logerr(f"Socket error: {e}\n")
            break
        except Exception as e:
            rospy.logerr(f"Unexpected error: {e}\n")
            break

        imu_data = (data.decode()).replace("\r", ",").split(",")
        imu_data_msg = Imu()
        imu_data_msg.header.stamp = rospy.Time.now()
        imu_data_msg.header.frame_id = "footprint"

        # Oreintation, linear, angular
        if len(imu_data) > 8:
            try:
                r = float(imu_data[6])
                p = float(imu_data[7])
                y = float(imu_data[8])

                w, x, y, z = quaternion_from_euler(r, p, y)

                imu_data_msg.orientation.w = w
                imu_data_msg.orientation.x = x
                imu_data_msg.orientation.y = y
                imu_data_msg.orientation.z = z

                imu_data_msg.linear_acceleration.x = float(imu_data[3])
                imu_data_msg.linear_acceleration.y = float(imu_data[4])
                imu_data_msg.linear_acceleration.z = float(imu_data[5])

                imu_data_msg.angular_velocity.x = float(imu_data[0])
                imu_data_msg.angular_velocity.y = float(imu_data[1])
                imu_data_msg.angular_velocity.z = float(imu_data[2])

                imu_data_pub.publish(imu_data_msg)
                rospy.loginfo("Publishing to the topic 'imu_data_topic'")
            except ValueError as e:
                rospy.logwarn(f"Error parsing data: {e}")
    rate.sleep()

    client_socket.close()
    server_socket.close()


if __name__ == "__main__":
    app_data_callback()
