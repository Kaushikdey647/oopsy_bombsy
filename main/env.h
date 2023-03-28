#include "ball.h"

class env{
    int* obstacles;
    int wedge_size;
    int ball_rad;
    //48-49 88-89 128
    ball b;
    int offset;
    bool falling;
public:
    env(int x, int y, int rad, int ws){
        b = ball(x, y, rad);
        obstacles = new int[3];
        obstacles[0] = random(0, 127-ws);
        obstacles[1] = random(0, 127-ws);
        obstacles[2] = random(0, 127-ws);
        wedge_size = ws;
        ball_rad = rad;
        falling = true;
    }
    int* get_obstacles(){
        return obstacles;
    }
    bool detect_x_collision(int px, int vx){
        return (px < ball_rad && vx < 0)||(px + ball_rad > 127 && px > 0);
    }
    bool detect_y_collision(int py, int obs, int ind){
        //CODE TO find obs, the index of obstacle just below
        return ( py + ball_rad >= obs )&&(( py >= obstacles[ind] - ball_rad )||( py <= obstacles[ind] + wedge_size + ball_rad ));
    }
    void update_obstacles(){
        obstacles[0] = obstacles[2];
        obstacles[1] = random(0, 127-wedge_size);
        obstacles[2] = random(0, 127-wedge_size);
    }
};