## ROS Chatroom Task:  

The task involved making a basic chatroom app using ROS with at least 3 users/nodes.


<u>Approach:</u>
I used the publisher-subscriber concept. I've used a single topic which every node is subscribed to and also publishes to.
The message has two fields, `user` and `message`.

I first attempted to use `ros::spin()` and `ros::spinOnce()`. However, `spin()` blocks until the node's killed. This is not useful because we want to keep publishing and subscribing.  

`spinOnce()` is also not useful because it's single threaded(`spin()` is also single-threaded) and would deal with callbacks sequentially. This meant that it would not show new messages until the node published(in my program) and then the`spinOnce()` was called.   

Since I needed simultaneous publishing and subscribing, I used `AsyncSpinner`which would allow multiple threads to run and thus different messages were dealt with simultaneously.


RQT Graphs:

![enter image description here](https://media.discordapp.net/attachments/1192918029160423494/1193882957543387216/image.png?ex=65ae5526&is=659be026&hm=53825a7206616315bf739d9757ed896f8846082f340a6a99927d3b000f20260e&=&format=webp&quality=lossless&width=1529&height=860)
![enter image description here](https://media.discordapp.net/attachments/1192918029160423494/1193884252480229448/image.png?ex=65ae565b&is=659be15b&hm=c1c239d15797131e6def0cfc4a01b4bfaa94c449e5d3b57370dbe662feba8a0e&=&format=webp&quality=lossless&width=1525&height=860)  


YouTube link:  
https://youtu.be/5QxY-Eo6QkQ
