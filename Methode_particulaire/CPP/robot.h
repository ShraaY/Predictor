#include "Kalman.h"
#include <opencv2/core/core.hpp>
#include <fstream>
#include <vector>
#include "gnuplot-iostream.h"


struct State{
  int ID,
  double t,
  double x,
  double y,
  double theta
}typedef State;

typedef std::pair<double, double> Point;

class Robot
{
private:
  Kalman m_kalman;
  int m_ID;
  std::vector<Point> plot;
  std::vector<State> m_state;
  double m_t;

public:
  //Variables Kalman
  Mat x;
  Mat Gx;
  Mat u;
  Mat C;
  Mat A;
  Mat Gbeta;
  Mat Galpha;
  Mat y;
  Mat x_out, Gx_out;

  Robot(); // Constructeur par defaut
  Robot(Mat x, Mat u, Mat C, Mat A, Mat Galpha, Mat y, Mat Gbeta, Kalman m_kalman, int m_number, Mat Gx);// Constructeur
  void Show() const; // Affichage
  //Methodes
  void evolution();
  void draw(Gnuplot gp);
  float scenario();
  void InitValues();

  void save_state();
  void export(std::fstream fs);

  void kalman_predict(Mat xup_k,Mat Pup_k, Mat Q, Mat A, Mat u, Mat* x_k1, Mat* P_k1);
  void kalman_correct(Mat x_k1, Mat P_k1, Mat  C, Mat R, Mat y, Mat* xup_k1, Mat* Pup_k1);
  void kalman_x(Mat x_k,Mat P_k,Mat u,Mat y,Mat Q, Mat R,Mat A ,Mat C, Mat* P_k1, Mat* x_k1);

};
