#ifndef VEC3_CLASS_H
#define VEC3_CLASS_H

// This macro is used by the vec3 class to generate
// the code for overloaded operators. You will not be
// calling this directly yourself.
#define VEC3_BINARY_COMPONENT_OPERATOR(_oper_) \
   vec3<element_type> operator _oper_ (const_reference v) const\
   { \
      vec3<element_type> a; \
      a.x = x _oper_ v.x; \
      a.y = y _oper_ v.y; \
      a.z = z _oper_ v.z; \
      return a; \
   }

// This macro is used by the vec3 class to generate
// the code for overloaded operators. You will not be
// calling this directly yourself.
#define VEC3_BINARY_BROADCAST_OPERATOR(_oper_) \
   template <typename FT> \
   vec3<element_type> operator _oper_ (FT fValue) const \
   { \
      vec3<element_type> a; \
      a.x = x _oper_ fValue; \
      a.y = y _oper_ fValue; \
      a.z = z _oper_ fValue; \
      return a; \
   }

// This macro is used by the vec3 class to generate
// the code for overloaded operators. You will not be
// calling this directly yourself.
#define VEC3_COMPONENT_OPERATOR(_oper_) \
   reference operator _oper_ (const_reference v) \
   { \
      if (static_cast<void *>(this) != static_cast<const void *>(&v)) \
      { \
         x _oper_ v.x; \
         y _oper_ v.y; \
         z _oper_ v.z; \
      } \
      return *this; \
   }

// This macro is used by the vec3 class to generate
// the code for overloaded operators. You will not be
// calling this directly yourself.
#define VEC3_BROADCAST_OPERATOR(_oper_) \
   template <typename FT> \
   reference operator _oper_ (FT fValue) \
   { \
      x _oper_ fValue; \
      y _oper_ fValue; \
      z _oper_ fValue; \
      return *this; \
   }

// vec3
//
// Templated class for 3-Dimensional vectors.
//
// Use vec3<float> or vec3f for floats.
// Use vec3<double> or vec3d for doubles.
template <typename T>
struct vec3
{
   //typedef to create an alias for vec3& named reference
   typedef vec3 &reference;
  
   //typedef to create an alias for const vec3& named const_reference
   typedef vec3 const &const_reference;

   // typedef to create an alias for T named element_type
   typedef T element_type;

   // This union allows the data of the vec3 to be accessed
   // in different ways with no additional overhead.
   //
   // Given the vector,
   // 
   //	vec3f myVector;
   //
   // You can access it's data the following ways...
   union
   {
	  // Access the vector as an array of floating point values by using myVector.v
      element_type v[3];

	  // Access the individuals components of the vector with myVector.x, myVector.y, or myVector.z
      struct
      {
         element_type x;
         element_type y;
         element_type z;
      };
   };

   // Constructor
   vec3(void) { }
   // Destructor
   ~vec3(void) { }
   // Constructor given X,Y,Z values
   vec3(element_type vx, element_type vy, element_type vz) : x(vx), y(vy), z(vz) { }

   // Copy Constructor
   template <typename VT>
   vec3(const vec3<VT> &v) : x(v.x), y(v.y), z(v.z) { }

   // These are simply macros that generate the code for the
   // various overloaded operators. Simply use +,-,*,/,=,+=,
   // -=, *=, /= operators with vec3 objects to use them.
   // 
   // Ex:
   //
   //	vec3f vecA( 1.0f, 0.0f, 0.0f);
   //	vec3f vecB( 0.0f, 1.0f, 0.0f);
   //
   //	vecA += vecB;
   //
   VEC3_BINARY_COMPONENT_OPERATOR(+);
   VEC3_BINARY_COMPONENT_OPERATOR(-);
   VEC3_BINARY_COMPONENT_OPERATOR(*);
   VEC3_BINARY_COMPONENT_OPERATOR(/);

   VEC3_BINARY_BROADCAST_OPERATOR(+);
   VEC3_BINARY_BROADCAST_OPERATOR(-);
   VEC3_BINARY_BROADCAST_OPERATOR(*);
   VEC3_BINARY_BROADCAST_OPERATOR(/);

   VEC3_COMPONENT_OPERATOR(=);
   VEC3_COMPONENT_OPERATOR(+=);
   VEC3_COMPONENT_OPERATOR(-=);
   VEC3_COMPONENT_OPERATOR(*=);
   VEC3_COMPONENT_OPERATOR(/=);

   VEC3_BROADCAST_OPERATOR(=);
   VEC3_BROADCAST_OPERATOR(+=);
   VEC3_BROADCAST_OPERATOR(-=);
   VEC3_BROADCAST_OPERATOR(*=);
   VEC3_BROADCAST_OPERATOR(/=);

   // Zero out the vector
   reference make_zero(void)
   {
      x = y = z = 0;
      return *this;
   }

   // Normalize the vector
   reference normalize(void)
   {
      element_type rsqrt = 1 / magnitude();
      x *= rsqrt;
      y *= rsqrt;
      z *= rsqrt;
      return *this;
   }

   // Negate the vector
   reference negate(void)
   {
      x = -x;
      y = -y;
      z = -z;
      return *this;
   }

   // Calculate and return the magnitude of the vector
   element_type magnitude(void)
   {
      element_type fMagnitude;
      fMagnitude = (T)sqrt(x*x + y*y + z*z);
      return fMagnitude;
   }

   element_type SqrMagnitude(void)
   {
	   element_type fMagnitude;
	   fMagnitude = (T)(x*x + y*y + z*z);
	   return fMagnitude;
   }

   // dot_product
   //
   // Calculat the dot product of two vectors
   //
   // In:
   //	const_reference vectorA - constant reference to the first vector
   //	const_reference vectorB - constant reference to the second vector
   //
   // Return:
   //	element_type - the result of the dotproduct
   inline friend element_type dot_product(const_reference vectorA, const_reference vectorB)
   {
		return vectorA.x*vectorB.x+vectorA.y*vectorB.y+vectorA.z*vectorB.z;
   }

   // cross_product
   //
   // Calculate the cross product of two vectors
   //
   // In:
   //	const_reference vectorA - constant reference to the first vector
   //	const_reference vectorB - constant reference to the second vector
   //
   // Out:
   //	reference vectorO - Where to store the result of the cross product
   //	
   // Remarks:
   //	
   //	vectorO must not be a reference to the same object as vectorA or vectorB
   inline friend void cross_product(reference vectorO, const_reference vectorA, const_reference vectorB )
   {
		vectorO.x = vectorA.y*vectorB.z - vectorA.z*vectorB.y;
		vectorO.y = vectorA.z*vectorB.x - vectorA.x*vectorB.z;
		vectorO.z = vectorA.x*vectorB.y - vectorA.y*vectorB.x;
   }

   // interpolate
   //
   // Linear interpolate two vectors based on a lamda value and return the resulting vector.
   //
   // In:
   //	const_reference vectorA - constant reference to the first vector
   //	const_reference vectorB - constant reference to the second vector
   //
   // Return:
   //
   //	vec3<T> - The resulting vector from the interpolation	
   inline friend vec3<T> interpolate(const_reference vectorA, const_reference vectorB, float lamda)
   {
       return vectorA + ((vectorB - vectorA) * lamda);
   }

   element_type *array(void) { return this->v; }
   element_type const *array(void) const { return this->v; }
};

// typedef to create an alias for vec3<float> named vec3f
typedef vec3<float> vec3f;
// typedef to create an alias for vec3<double> named vec3d
typedef vec3<double> vec3d;

#endif
