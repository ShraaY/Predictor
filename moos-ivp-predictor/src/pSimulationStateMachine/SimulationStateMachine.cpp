/************************************************************/
/*    NAME:                                                 */
/*    ORGN: MIT                                             */
/*    FILE: SimulationStateMachine.cpp                      */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "SimulationStateMachine.h"
#include <math.h>
#include <string.h>

using namespace std;

//---------------------------------------------------------
// Constructor

SimulationStateMachine::SimulationStateMachine()
{
  //states state
  state_1 = 1;  // UP
  state_2 = 0;  // Down et orientation
  state_3 = 0;  // S_cap
  desired_depth = 0;
  gps_lat = 0;
  gps_long = 0;
  //state3 variables
	begin_time = clock();
  time_passed = 0;

  propulsion = 0;
  speed = 0;


	//state2 variables
  desired_heading = 0;
  err_heading = 0;

	//state variables
  err_depth = 0;
  compteur_gps = 0 ;
  gps_m_lat = 0 ;
  gps_m_long  = 0 ;
  time_croisiere = 0;
  dist_croisiere = 0;

  // chemin a suivre
  a_to_b = 1;
  b_to_c = 0;
  c_to_a = 0;
}

//---------------------------------------------------------
// Destructor

SimulationStateMachine::~SimulationStateMachine()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SimulationStateMachine::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;


    string key = msg.GetKey();

    //heading
  if (key == "NAV_HEADING"){
    heading = msg.GetDouble()/360 * 2 * M_PI ; //heading in radians
  }
    // Profondeur
  if (key == "NAV_DEPTH"){
      actual_depth  = msg.GetDouble();   //in m
    }
   // GPS
  if (key == "Latitude"){
       gps_lat = msg.GetDouble();
       if (gps_lat!=0){
         compteur_gps = compteur_gps +1 ;
       }

       else {compteur_gps=0;}
     }

  if (key == "Longitude"){
      gps_long  = msg.GetDouble();
       }


#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SimulationStateMachine::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);

   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SimulationStateMachine::Iterate()
{
  if (state_1 == 1){ // UP
    desired_depth = 2;  // a fond vers le haut pour sortir de l'eau
    Notify("DESIRED_DEPTH", desired_depth);  // active le suivi de profondeur à 2m (donc 0m)
    propulsion = 0;
    Notify("DESIRED_THRUST",propulsion);
    speed = 0;
    Notify("DESIRED_SPEED",speed);
      

    err_depth = abs(desired_depth - actual_depth) ;

    if (err_depth<=eps_prof && compteur_gps>=nbr_gps){ // si on est dehors et que le GPS à sorti 10 valeurs
      state_1 = 0;
      state_2 = 1;
      gps_m_lat = gps_lat; // valeure sorite par le GPS apres filtre médian
      gps_m_long = gps_long;

      if (a_to_b == 1){
        dist_croisiere = pow((B_lat - gps_m_lat),2)+pow((B_long - gps_m_long),2) ;
        time_croisiere = sqrt(dist_croisiere) / vitesse;

        desired_heading = 270 - atan( (B_lat-gps_m_lat) /  (B_long-gps_m_long))*180/ M_PI;
      }

      else if (b_to_c == 1){
        dist_croisiere = pow((C_lat - gps_m_lat),2)+pow((C_long - gps_m_long),2) ;
        time_croisiere = sqrt(dist_croisiere) / vitesse;
        desired_heading = 270 - atan( (C_lat-gps_m_lat) /  (C_long-gps_m_long))*180/ M_PI;
      }
      else if (c_to_a == 1){
        dist_croisiere = pow((A_lat - gps_m_lat),2)+pow((A_long - gps_m_long),2) ;
        time_croisiere = sqrt(dist_croisiere) / vitesse;
        desired_heading = 270 - atan( (C_lat-gps_m_lat) /  (C_long-gps_m_long))*180/ M_PI;
      }

    }
  }

  else if (state_2 == 1){ //Down Ori
    desired_depth = nav_depth;  // a fond vers le profond
    Notify("DESIRED_DEPTH", desired_depth);  // active le suivi de profondeur
    Notify("DESIRED_HEADING",desired_heading);
    propulsion = 0;
    Notify("DESIRED_THRUST",propulsion);
    speed = 0;
    Notify("DESIRED_SPEED",speed);
    err_heading = abs(desired_heading-heading);
    err_depth = abs(desired_depth-actual_depth);
    if (err_heading<eps_yaw && err_depth<eps_prof){
      state_2 = 0;
      state_3 = 1;

    }

  }

  else if (state_3 == 1){ // SuiviCap

   begin_time = clock();
   Notify("DESIRED_DEPTH", desired_depth);  // active le suivi de profondeur
   Notify("DESIRED_HEADING",desired_heading);
   propulsion = 100;
   Notify("DESIRED_THRUST",propulsion);
   speed = 2;
   Notify("DESIRED_SPEED",speed);

   time_passed = double( clock() - begin_time)/CLOCKS_PER_SEC;
   if (time_passed >= time_croisiere){
     state_1 = 1;
     state_3 = 0;
     if (a_to_b == 1){
       a_to_b = 0;
       b_to_c = 1;
     }

     else if (b_to_c == 1){
       b_to_c = 0;
       c_to_a = 1;
     }
     else if (c_to_a == 1){
       c_to_a = 0;
       a_to_b = 1;
     }
        // changer a to b en b to a
   }

  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SimulationStateMachine::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;

      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }

      if (param == "nbr_gps") {
        nbr_gps = stold(value);
      }

      if (param == "eps_prof") {
        eps_prof = stold(value);
      }

      if (param == "eps_yaw") {
        eps_yaw = stold(value);
      }

      if (param == "nav_depth") {
        nav_depth = stold(value);
      }

      if (param == "vitesse") {
        vitesse = stold(value);
      }

      if (param == "A_lat") {
        A_lat = stold(value);
      }

      if (param == "A_long") {
        A_long = stold(value);
      }

      if (param == "B_lat") {
        B_lat = stold(value);
      }

      if (param == "B_long") {
        B_long = stold(value);
      }

      if (param == "C_lat") {
        C_lat = stold(value);
      }

      if (param == "C_long") {
        C_long = stold(value);
      }

    }
  }

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void SimulationStateMachine::RegisterVariables()
{
  // Register("FOOBAR", 0);
  Register("NAV_HEADING", 0);
  Register("NAV_DEPTH", 0);
}
