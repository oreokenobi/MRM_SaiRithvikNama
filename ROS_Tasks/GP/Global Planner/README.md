

#### Description of the task:

"**Implement the Autonomous Traversal of the 4 wheel bot designed in the Group Task from one GPS Coordinate to another using C++ and OOP Concepts (consider no obstacles in the path).**"

##### Note:
I made a new urdf model as I was experiencing problems with the old one.
#### Approach:

1. I used the [Haversine equation](https://www.movable-type.co.uk/scripts/latlong.html) to find the shortest great-circle distance and also the bearing angle at which the bot needs to orient itself before it starts moving.
   
2. The smaller rotation(clockwise or counterclockwise) was determined to start rotating to the right orientation.
   
3. The distance to the destination as well as the angle needed to turn are continually updated.
   
4. Proportional control: According to [this](https://f1tenth.org/slides/l2.pdf) and a few other references, we multiply the difference in positions (linear and angular) by a proportionality constant for the linear/angular motion, which means that the bot will slow down as the separation gets smaller.
   
5. When both the distance left and  the orientation difference are sufficiently small, we publish 0 linear and angular velocities to make the bot stop.


#### ROS Nodes:

Only one node was made, called `globalPlanner`, which had two callbacks for the IMU and the GPS data. It would publish to `cmd_vel`, which the `skid_steer_controller` was subscribed to.

#### Topics and Messages:
```
GPS data:
	Topic: /fix
	Message type: sensor_msgs/NavSatFix
IMU data:
	Topic: /imu
	Message type: sensor_msgs/Imu
Movement data:
	Topic: /cmd_vel
	Message type: geometry_msgs/Twist
```

#### RQT Graphs:

1. ![Image 1](https://media.discordapp.net/attachments/1189820669039493224/1203708824070918144/Pasted_image_20240204145936.png?ex=65d21433&is=65bf9f33&hm=b9b0ec79691c21ea091b015184bf537d04440c43a055d1f7dd8a3bdd788a641a&=&format=webp&quality=lossless&width=1578&height=870)

2. ![Image 2](https://media.discordapp.net/attachments/1189820669039493224/1203708935719092244/Pasted_image_20240204145058.png?ex=65d2144e&is=65bf9f4e&hm=79304b107783dfb8117fe5565b5e279854e7574782869c6b80805f842cc025cb&=&format=webp&quality=lossless&width=1577&height=870)

#### YouTube link:
[VIdeo of the global planner](https://youtu.be/T8cuzletHKE)


