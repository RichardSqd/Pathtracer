#include "camera.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "CGL/misc.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"

using std::cout;
using std::endl;
using std::max;
using std::min;
using std::ifstream;
using std::ofstream;

namespace CGL {

using Collada::CameraInfo;

Ray Camera::generate_ray_for_thin_lens(double x, double y, double rndR, double rndTheta) const {
  // Part 2, Task 4:
  // compute position and direction of ray from the input sensor sample coordinate.
  // Note: use rndR and rndTheta to uniformly sample a unit disk.

    //red segment
    double tanradh = tan(0.5*hFov*PI/180.0);
    double tanradv = tan(0.5*vFov*PI/180.0);
    Vector3D v = (Vector3D(-tanradh+2*tanradh*x,-tanradv+2*tanradv*y,-1));
    Ray ray(Vector3D(), v);

    Vector3D pLens(lensRadius*sqrt(rndR)*cos(rndTheta),lensRadius*sqrt(rndR)*sin(rndTheta),0);

    double t = -focalDistance/ray.d.z;
    Vector3D pFocus = ray.at_time(t);;
    Vector3D blue = pFocus - pLens;
    blue.normalize();

    Ray worldBlue (c2w *pLens+pos, c2w*blue);
    worldBlue.min_t = nClip;
    worldBlue.max_t = fClip;
    return  worldBlue;
}


} // namespace CGL
