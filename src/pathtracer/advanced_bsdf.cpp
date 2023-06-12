#include "bsdf.h"

#include <algorithm>
#include <iostream>
#include <utility>


using std::max;
using std::min;
using std::swap;

namespace CGL {

// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO:
  // Implement MirrorBSDF
  *pdf = 1.;
  reflect(wo,wi);
  return reflectance / abs_cos_theta(*wi);

  return Spectrum();
}

// Microfacet BSDF //

double MicrofacetBSDF::G(const Vector3D& wo, const Vector3D& wi) {
  return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double MicrofacetBSDF::D(const Vector3D& h) {
  // TODO: proj3-2, part 3
  // Compute Beckmann normal distribution function (NDF) here.
  // You will need the roughness alpha.
  //return std::pow(cos_theta(h), 100.0);;
  double thetaH = getTheta(h);
  return exp(- pow(tan(thetaH),2)/(alpha*alpha)) / (PI * alpha*alpha*pow(cos(thetaH),4));

}

Spectrum MicrofacetBSDF::F(const Vector3D& wi) {
  // TODO: proj3-2, part 3
  // Compute Fresnel term for reflection on dielectric-conductor interface.
  // You will need both eta and etaK, both of which are Spectrum.
  double cosThetaI = getTheta(wi);
  Vector3D n2Plusk2 = (eta*eta+k*k);
  Vector3D twoNCosThetaI = 2*eta*cosThetaI;
  Vector3D cosThetaISquare = cosThetaI*cosThetaI;
  Vector3D Rs = (n2Plusk2 - twoNCosThetaI + cosThetaISquare)/(n2Plusk2+twoNCosThetaI+cosThetaISquare);
  Vector3D Rp = (n2Plusk2*cosThetaISquare-twoNCosThetaI+1)/(n2Plusk2*cosThetaISquare+twoNCosThetaI+1);
  return (Rs+Rp)/2.0;
  //return Spectrum();
}

Spectrum MicrofacetBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  // TODO: proj3-2, part 3
  // Implement microfacet model here.
  if(wi.z<0 || wo.z<0) return Spectrum();
  Vector3D n (0,0,1);
  Vector3D h = (wo+wi).unit();

  return F(wi)*G(wo,wi)*D(h)/(4*dot(n,wo)*dot(n,wi));

}

Spectrum MicrofacetBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO: proj3-2, part 3
  // *Importance* sample Beckmann normal distribution function (NDF) here.
  // Note: You should fill in the sampled direction *wi and the corresponding *pdf,
  //       and return the sampled BRDF value.
  //*wi = cosineHemisphereSampler.get_sample(pdf); //placeholder

  Vector2D random2D = sampler.get_sample();
  double thetaH = atan(sqrt(-alpha*alpha*log(1-random2D.x)));
  double phiH = 2*PI * random2D.y;
  Vector3D normalH (sin(thetaH)*cos(phiH),sin(thetaH)*sin(phiH),cos(thetaH));
  *wi = -wo + 2*(dot(wo,normalH))*normalH;
  wi->normalize();
  if(wi->z<=0){
      *pdf = 0;
      return Spectrum();
  }

  double phTheta = (2 * sin(thetaH) * exp(- pow(tan(thetaH),2)/pow(alpha,2)))/ (alpha*alpha*pow(cos(thetaH),3));
  double phPhi = 1/(2*PI);
  double pw = phTheta * phPhi / sin(thetaH);
  *pdf = pw / (4*dot(*wi,normalH));

  return MicrofacetBSDF::f(wo, *wi);
}

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO:
  // Implement RefractionBSDF
  return Spectrum();
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO:
  // Compute Fresnel coefficient and use it as the probability of reflection
  // - Fundamentals of Computer Graphics page 305
  if(!refract(wo,wi,ior)){
      reflect(wo,wi);
      *pdf = 1;
      return reflectance / abs_cos_theta(*wi);
  }else{
      bool f = false;
      //Schlick's approximation
      double r0sqrt = (1-ior)/(1+ior);
      double r0 = r0sqrt * r0sqrt;
      double R = r0 + (1-r0)*pow(1-abs_cos_theta(wo),5);

      if(coin_flip(R)){

          reflect(wo,wi);
          *pdf = R;
          return R * reflectance / abs_cos_theta(*wi);

      }else{
          double eta = wo.z>0 ? 1./ior : ior / 1.;
          refract(wo,wi,ior);
          *pdf = 1-R;
          return (eta*eta) * (1-R) * transmittance / abs_cos_theta(*wi) ;
      }

  }
  return Spectrum();
}

void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {
  // TODO:
  // Implement reflection of wo about normal (0,0,1) and store result in wi.

  *wi = Vector3D(-wo.x,-wo.y,wo.z);
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {
  // TODO:
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.
  double eta;

  eta = wo.z>0 ? 1./ior : ior / 1.;
  double tir = 1-eta*eta*(1-wo.z*wo.z);
  if(tir<0) return false;
  wi->x = -eta * wo.x;
  wi->y = -eta *wo.y;
  wi->z = wo.z>0 ? -sqrt(tir) : sqrt(tir);

  return true;
}

} // namespace CGL
