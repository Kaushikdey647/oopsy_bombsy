class ball{
  int* pos;
  int radius;
  float* vel;
  public:
  ball(int x, int y, int rad){
    radius = rad;
    pos = new int[2];
    vel = new float[2];
    pos[0] = x;
    pos[1] = y;
  }
  
  int* get_pos(){
    return pos;
  }

  int* get_vel(){
    return vel;
  }

  void update_px(){
    pos[0] += vel[0];
  }
  
  void update_py(){
    pos[1] += vel[1];
  }

  void update_vx(float x_accel){
    vel[0] += x_accel;
  }

  void update_vy(float y_accel){
    vel[1] += y_accel;
  }

  void stop_x(){
    vel[0] = 0;
  }

  void stop_y(){
    vel[1] = 0;
  }
  
};
