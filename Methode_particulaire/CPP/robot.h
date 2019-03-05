
#include <opencv2/core/core.hpp>
#include <fstream>
#include <vector>
#include "gnuplot-iostream.h"
#include <boost/tuple/tuple.hpp>
#include <math.h>
#include <random>


struct State{
  int ID;
  double t;
  double x;
  double y;
  double theta;
}typedef State;

typedef std::pair<double, double> point;


class Robot
{
private:
<<<<<<< HEAD
=======

>>>>>>> 6f95a9418c5cff820a3c3b32f382187711c7283b
  double dt;
  int m_ID;
  std::vector<State> m_state;

public:
  double t;

  //Variables Kalman
  cv::Mat x;
  cv::Mat Gx;
  cv::Mat u;
  cv::Mat C;
  cv::Mat A;
  cv::Mat B;
  cv::Mat Gbeta;
  cv::Mat Galpha;
  cv::Mat y;
  cv::Mat x_out, Gx_out;

  Robot(); // Constructeur par defaut
  //~Robot(); // Destructeur
  void P_theta(); //Porportionnel pour theta
  Robot(int); // Constructeur par defaut

  Robot(cv::Mat x, cv::Mat u, cv::Mat C, cv::Mat A, cv::Mat Galpha, cv::Mat y, cv::Mat Gbeta, cv::Mat Gx);// Constructeur
  void Show() const; // Affichage
  //Methodes
  void evolution();
  void draw(std::vector<point>*);
  void draw_x_y(std::vector<point>*);
  float scenario();

  void save_state();
  void Export(std::ofstream & fs);

  void kalman_predict( cv::Mat xup_k, cv::Mat Pup_k, cv::Mat* x_k1, cv::Mat* P_k1);
  void kalman_correct( cv::Mat* xup_k1, cv::Mat* Pup_k1);
  void kalman_x( cv::Mat* P_k1, cv::Mat* x_k1);

};
