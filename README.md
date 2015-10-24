# openGLCrowdSimulator:

- This is a simple pedestrian movement simulator that is created based on a heuristic model with some tweeks.

- The implementation is also accelerated using Intel Threaded Building Blocks (TBB).

- All Agents are initially assigned a random destination somewhere off screen. Whenever an Agent reaches a destination they are re-assigned a new one immediately. This makes it seem as though there is a continuous flow of pedestrian movement.

- Agents try to reach their destination as quickly as possible while avoiding collision with others.


####Screenshot Samples:

![Agets start off screen](http://i.imgur.com/2EZZPWR.png)
![After a few Seconds](http://i.imgur.com/Eg2lsXL.png)
![After a few more Seconds](http://i.imgur.com/8LsnrM5.png)

