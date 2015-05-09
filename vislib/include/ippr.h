/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2006-2007 Intel Corporation. All Rights Reserved.
//
//
//                  Intel(R) Performance Primitives
//                  Realistic Rendering Library (ippRR)
//
//  Created: Mon Oct 31 19:39:49 2005
*/
#if !defined( __IPPR_H__ ) || defined( _OWN_BLDPCS )
#define __IPPR_H__

#if defined (_WIN32_WCE) && defined (_M_IX86) && defined (__stdcall)
  #define _IPP_STDCALL_CDECL
  #undef __stdcall
#endif

#ifndef __IPPDEFS_H__
  #include "ippdefs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined( _OWN_BLDPCS )

typedef float           IppPoint2D_32f[2];
typedef float           IppPoint3D_32f[3];
typedef float           IppVector3D_32f[4];
typedef IppPoint3D_32f  IppBox3D_32f[2];
typedef IppPoint3D_32f  IppTriangle3D_32f[3];

typedef struct TriangleAccel IpprTriangleAccel;

typedef struct KDTreeNode{
    Ipp32s  flag_k_ofs;
    union _tree_data{
        Ipp32f  split;
        Ipp32s  items;
    }tree_data;
}IpprKDTreeNode;

typedef struct IntersectContext{
    IppBox3D_32f        *pBound;    /* pointer to bounding box for a whole object */
    IpprTriangleAccel   *pAccel;    /* pointer to triangle acceleration structure */
    IpprKDTreeNode      *pRootNode; /* pointer to KD-tree root node */
}IpprIntersectContext;

/* Tree building algorithm identifiers */
typedef enum {
    ippKDTBuildSimple    = 0x499d3dc2,  // Simple building mode
    ippKDTBuildPureSAH   = 0x2d07705b   // SAH building mode
}IpprKDTreeBuildAlg;

/* Context for simple building mode */
typedef struct SimpleBuilderContext{
    IpprKDTreeBuildAlg   Alg;           // Must be equal to ippKDTBuildSimple constant
    Ipp32s               MaxDepth;      // Subdivision depth (with middle point subdivision)
}IpprSmplBldContext;

/* Context for SAH building mode */
typedef struct PSAHBuilderContext{
    IpprKDTreeBuildAlg   Alg;           // Must be equal to ippKDTBuildPureSAH constant
    Ipp32s               MaxDepth;      // Maximum tree subdivision depth (minimum - 0, maximum - 51)
    Ipp32f               QoS;           // Termination criteria modifier
    Ipp32s               AvailMemory;   // Maximum available memory in Mb
    IppBox3D_32f        *Bounds;        // Cut-off bounding box
}IpprPSAHBldContext;

typedef enum {
    ippNormInd  = 3,
    ippTriInd   = 4
} IpprIndexType;
#endif /* _OWN_BLDPCS */

/* /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Functions declarations
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////////
//  Name:       ipprGetLibVersion
//  Purpose:    getting of the library version
//  Returns:    the structure of information about version
//              of ippRR library
//  Parameters:
//
//  Notes:      not necessary to release the returned structure
*/
IPPAPI( const IppLibraryVersion*, ipprGetLibVersion, (void) )

/* /////////////////////////////////////////////////////////////////////////////
//           Acceleration Functions
//////////////////////////////////////////////////////////////////////////////// */

/* ///////////////////////////////////////////////////////////////////////////
//  Name:
//    ipprTriangleAccelInit
//  Purpose:
//    Initialize a IpprtTriangleAccel for future usage in ipprIntersect...
//  Input Arguments:
//    pVertexCoord    - pointer to the array of vertex coordinate.
//    pTrnglIndex     - pointer to the triangle's indexes.
//     cntTrngl       - the number of triangles.
// Input Arguments:
//    pTrnglAccel     - pointer to the structure IpprTriangleAccel
//  Returns:
//    ippStsNoErr        No error.
*/
IPPAPI(IppStatus, ipprTriangleAccelInit,(
       IpprTriangleAccel* pTrnglAccel,const Ipp32f* pVertexCoord,const Ipp32s* pTrnglIndex,int cntTrngl ))

/* ///////////////////////////////////////////////////////////////////////////
//  Name:
//    ipprTriangleAccelGetSize
//
//  Purpose:
//    Return size of IpprtTriangleAccel
//  Parameters:
//    pTrnglAccelSize - pointer to the resulting size of the structure
//                                  IpprtTriangleAccel
//  Returns:
//    ippStsNoErr        No error.
//    ippStsNullPtrErr   Indicates an error when pointer pTrnglAccelSize is NULL.
*/
IPPAPI(IppStatus, ipprTriangleAccelGetSize,(int* pTrnglAccelSize))

/* ///////////////////////////////////////////////////////////////////////////
  Name:
    ippiKDTreeBuildAlloc
  Purpose:
    Build the k-D tree for the set of triangles using one of predefined construction
    algorithms controlled by service parameters context.
  Parameters:
    pDstKDTree     - address of a pointer to the builded tree;
    pSrcVert       - pointer to the scene element vertices array;
    pSrcTriInx     - pointer to the scene element indexed triangles array;
    SrcVertSize    - size of vertices array;
    SrcTriSize     - size of triangles array;
    pDstKDTreeSize - address of the builded tree size;
    QoS            - fuzzy quality control parameter. takes values from 0.0 to 1.0;
    AlgType        - type of tree construction algorithm
  Returns:
    ippStsNoErr           No error.
    ippStsNullPtrErr      Indicates an error when one of the pointers is NULL.
    ippStsSizeErr         Wrong (negative) size of one of arrays.
    ippStsOutOfRangeErr   QoS is out of [0.0, 1.0] range.
    ippStsNoMemErr        Not enough memory for the tree construction algorithm.
    ippStsBadArgErr       Unknown algorithm type.
*/
IPPAPI(IppStatus, ipprKDTreeBuildAlloc,(
       IpprKDTreeNode     **pDstKDTree,
       const Ipp32f * const pSrcVert,
       const Ipp32s * const pSrcTriInx,
       Ipp32s               SrcVertSize,
       Ipp32s               SrcTriSize,
       Ipp32s              *pDstKDTreeSize,
       const void * const   pBldContext))

/* ///////////////////////////////////////////////////////////////////////////
  Name:
    ippiKDTreeFree
  Purpose:
    Frees memory allocated for the k-D tree during ippiKDTreeBuildAlloc.
  Parameters:
    pSrcKDTree     - a pointer to the k-D tree;
  Returns:
*/
IPPAPI(void, ipprKDTreeFree,(
       IpprKDTreeNode   *pSrcKDTree))


/* /////////////////////////////////////////////////////////////////////////////
//           Ray-scene Intersection Engine
////////////////////////////////////////////////////////////////////////////// */

/* /////////////////////////////////////////////////////////////////////////////
//  Names:             ipprIntersectMO_32f
//  Purpose:   Calculates intersection points of rays with triangles,the indexes those triangles,
//             the distances from origin points to intersection points.
//
// Input Arguments:
//       pOrigin    - array of pointers to a separate coordinates(x,y,z)of planes of the origin points..
//       pDirection - array of pointers to a separate coordinates(x,y,z)of planes of the ray's directions.
//         pContext - pointer to the intersection's context.
//       blockSize  - size of rays' block.
// Input/Output Arguments:
//            pDist - pointer to the distance from origin to intersection point. Else it is input value.
//                    As input pDist[i] should be 0.f if you don't want to process this ray.
// Output Arguments:
//            pHit  - pointer to the local surface parameters( u, v )at hit point in case of intersection was found.
//          pTrngl  - pointer to the Triangle index in case of intersection was found. Else it is -1.
//  Returns:
//  ippStsNoErr      No errors
//  ippStsNoMemErr   The node stack is overfilled.
*/

IPPAPI(IppStatus,ipprIntersectMO_32f,(
       const Ipp32f* const       pOrigin[3],
       const Ipp32f* const    pDirection[3],
       Ipp32f*                        pDist,
       Ipp32f*                      pHit[2],
       Ipp32s*                       pTrngl,
       const IpprIntersectContext* pContext,
       IppiSize blockSize
       ))
/* /////////////////////////////////////////////////////////////////////////////
//  Names:     ipprIntersectEyeSO_32f
//  Purpose:   Calculates intersection points of rays with triangles,the indexes those triangles,
//             the distances from origin points to intersection points.
//
// Input Arguments:
//       originEye  - origin point.All rays have a single origin.
//       pDirection - array of pointers to a separate coordinates(x,y,z)of planes of the ray's directions.
//         pContext - pointer to the intersection's context.
//       blockSize  - size of rays' block.
// Output Arguments:
//            pDist - pointer to the distance from origin to intersection point. Else it is IPP_MAXABS_32F.
//            pHit  - pointer to the local surface parameters( u, v )at hit point in case of intersection was found.
//          pTrngl  - pointer to the Triangle index in case of intersection was found. Else it is -1.
//  Returns:
//  ippStsNoErr      No errors
//  ippStsNoMemErr   The node stack is overfilled.
*/
IPPAPI(IppStatus,ipprIntersectEyeSO_32f,(
       IppPoint3D_32f             originEye,
       const Ipp32f* const    pDirection[3],
       Ipp32f*                        pDist,
       Ipp32f*                      pHit[2],
       Ipp32s*                       pTrngl,
       const IpprIntersectContext* pContext,
       IppiSize blockSize
       ))
/* /////////////////////////////////////////////////////////////////////////////
//  Names:     ipprIntersectAnySO_32f
//  Purpose:   performs occlusion tests for block of rays with single origin.
//
// Input Arguments:
//       originEye  - origin point.All rays have a single origin.
//       pDirection - array of pointers to a separate coordinates(x,y,z)of planes of the ray's directions.
//         pContext - pointer to the intersection's context.
//       blockSize  - size of rays' block.
// Input/Output Arguments:
//            pMask - pointer to the array of the mask. If output pMask[i][j] = 0, occlusion test for this ray is true.
//                    Else it is input value.
//                    As input pMask[i][j] should be 0 if you don't want to process this ray.Else it should be -1.
//          pTrngl  - pointer to the Triangle index in case of intersection was found. Else it is -1.
//  Returns:
//  ippStsNoErr      No errors
//  ippStsNoMemErr   The node stack is overfilled.
*/
IPPAPI(IppStatus,ipprIntersectAnySO_32f,(
       IppPoint3D_32f             originEye,
       const Ipp32f* const    pDirection[3],
       Ipp32s*                    pOccluder,
       Ipp32s*                        pMask,
       IppiSize                   blockSize,
       const IpprIntersectContext* pContext
       ))

/* /////////////////////////////////////////////////////////////////////////////
//           Shaders Support Functions
//////////////////////////////////////////////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////////
//  Name:     ipprMul_32f_C1P3IM
//  Purpose: Purpose: multiplies each element of three vectors of the accumulator (pSrcDst)
//           for which the corresponding element of a vector of a mask more or is equal to zero,
//           by an element of an source vector.
//  Name:     ipprMul_32f_IM
//  Purpose: Multiplies an element of the accumulator (pSrcDst)
//           for which the corresponding element of a vector of a mask more or is equal to zero,
//           by an element of an source vector.
// Input Arguments:
//           pSrc -  pointer to the first source vector
//          pMask -  pointer to the first mask's vector
//          len                  length of the vectors
// Input/Output Arguments:
//          pSrcDst - pointer to the source/destination (accumulator) vectors.
// Returns:
//  ippStsNoErr      No errors
*/
IPPAPI(IppStatus,ipprMul_32f_C1P3IM,( const Ipp32f* pSrc, const Ipp32s* pMask,
       Ipp32f* pSrcDst[3], int len ))

IPPAPI(IppStatus,ipprMul_32f_C1IM,( const Ipp32f* pSrc, const Ipp32s* pMask,
       Ipp32f* pSrcDst, int len ))


/* /////////////////////////////////////////////////////////////////////////////
//  Name:     ipprAddMulMul_32f_AC1P3IM
//  Purpose:  multiplies elements of two triplex source vectors and adds product
//            to triplex of the accumulator vectors ???
//  Input Arguments:
//          point   - source point.
//          pSrc0   - pointer to the first source vector
//          pSrc1   - pointer to the second source vector
//          pMask   - pointer to the mask's vector
//          len     - ength of the vectors
// Output Arguments:
//          pSrcDst - pointer to the source/destination (accumulator) vector.
//  Notes:
//         pSrcDst[0][n] = pSrcDst[0][n] + pSrc1[n] * pSrc2[n] * point[0], n=0,1,2,..len-1.
//         pSrcDst[1][n] = pSrcDst[1][n] + pSrc1[n] * pSrc2[n] * point[1],
//         pSrcDst[2][n] = pSrcDst[2][n] + pSrc1[n] * pSrc2[n] * point[2],
//         E.g for Lambertian cosine low.
//  Returns:
//  ippStsNoErr      No errors
*/
IPPAPI(IppStatus,ipprAddMulMul_32f_AC1P3IM,(
       IppPoint3D_32f point,
       const Ipp32f*  pSrc0,
       const Ipp32f* const pSrc1[3],
       const Ipp32s*  pMask,
       Ipp32f*   pSrcDst[3],
       int len
       ))
/* /////////////////////////////////////////////////////////////////////////////
//  Name:     ipprDiv_32f_C1IM
//  Purpose:   divides an element of the accumulator (pSrcDst) for which the corresponding
//             element of a vector of a mask more or is equal to zero, into an element of an source vector.
//  Input Arguments:
//           pSrc - pointer to the divisor source vector
//          pMask - pointer to the mask vector.
//            len - vector's length, number of items.
//  Input-Output Argument:
//        pSrcDst - pointer to the source/destination (accumulator) vector.
//
//  Returns:
//  ippStsNoErr      No errors
*/
IPPAPI(IppStatus,ipprDiv_32f_C1IM,(const Ipp32f* pSrc, const Ipp32s* pMask, Ipp32f* pSrcDst, int len ))
/* ///////////////////////////////////////////////////////////////////////////
//  Name:
//      ipprDot_32f_P3C1M
//  Purpose:
//      calculates dot product of the incident ray directions and normales of surface.
//  Input Arguments:
// pDirection - pointer to array of pointers to a separate coordinates(x,y,z)of planes of the ray's directions.
//      pDist - pointer to the IpprPointsOnRays_SO structure
//  pSurfNorm - pointer to the surface's normals.
//  Output arguments:
// pSurfDotIn - pointer to the dot product.
//  Returns:
//      ippStsNoErr   No errors
*/
IPPAPI(IppStatus, ipprDot_32f_P3C1M,( const Ipp32f* const pSrc0[3],
       const Ipp32f* const pSrc1[3],const Ipp32s* pMask, Ipp32f* pDot, int len ))
IPPAPI(IppStatus, ipprDotChangeNorm_32f_IM,( const Ipp32f* const pSrc[3],
       const Ipp32s* pMask, Ipp32f* pSrcDst[3],Ipp32f* pDot,int len ))

/* ///////////////////////////////////////////////////////////////////////////
//  Name:
//      ipprDot_32f_M
*/
IPPAPI(IppStatus, ipprDistAttenuationSO_32f_M,( IppPoint3D_32f point, const Ipp32f* const  pSurfHit[3],
       const Ipp32s* pMask, Ipp32f* pDist, int len ))

/* /////////////////////////////////////////////////////////////////////////////
//       Rays' casting
//////////////////////////////////////////////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////////
//  Name:     ipprCastEye_32f
//  Purpose:  to calculate the primary ray's vectors.
// Input Arguments:
//     imPlaneOrg - the coordinate of origin the projection's plane.
//        dW      - a linestep along width  of the projection's plane.
//        dH      - a linestep along height of the projection's plane.
//        wB      - the number of block along width of Image.
//        hB      - the number of block along height of Image.
//        cBlock  - ??????????????????.
//     blockSize  - size of rays' block.
// Input-Output Argument:
//     pDirection - pointer to the destination vector. It is not normalised.
//  Returns:
//  ippStsNoErr      No errors

*/

IPPAPI(IppStatus,ipprCastEye_32f,(
       IppPoint3D_32f imPlaneOrg,
       IppPoint3D_32f dW,
       IppPoint3D_32f dH,
       int wB,int hB,
       IppiSize cBlock,
       Ipp32f* pDirection[3],
       IppiSize blockSize ))

/* /////////////////////////////////////////////////////////////////////////////
//  Names:     ipprCastShadowSO_32f
//  Purpose:   calculates block of shadow rays.  for which the corresponding
//             element of a vector of a pMask more or is equal to zero.
// Input Arguments:
//    pOrigin       - pointer to the origin point.
//   pSurfDotIn     - pointer to the vector of dot products of incident rays and normals
//                    at intersections point.
//   pSurfNorm      - pointer  to array of pointers to a separate coordinates(x,y,z)of planes
//                    of normals at intersections point.
//    pSurfHit      - pointer to array of pointers to a separate coordinates(x,y,z)of planes of the intersection points.
//            pMask - pointer to the mask vector.
//  Output Arguments:
//     pDirection   - pointer to the destination vector. Shouldn't be normalised.
//     pDist        - . Here it is can be 0.f or 1.f
//     pDistRay     - pointer to the vector of distances between intersection's points and the origin point.
//     pDotRay      - pointer to the vector of dot products of shadow rays and normals.
//  Returns:
//  ippStsNoErr      No errors
*/
IPPAPI(IppStatus,ipprCastShadowSO_32f,(
       IppPoint3D_32f           pOrigin,
       const Ipp32f*         pSurfDotIn,
       const Ipp32f* const pSurfNorm[3],
       const Ipp32f* const  pSurfHit[3],
       Ipp32s*                    pMask,
       Ipp32f*                  pDotRay,
       Ipp32f*            pDirection[3],
       int                          len
))
/* /////////////////////////////////////////////////////////////////////////////
//  Names:     ipprCastReflectionRay_32f
//  Purpose:   calculates array of reflected rays, for which the corresponding
//             element of a vector of a mask more or is equal to zero.
//
// Input Arguments:
//    pIncident   - pointer to the array of vectors of incident rays.
//          pMask - pointer to the mask vector.
//   pSurfNorm    - pointer  to array of pointers to a separate coordinates(x,y,z)of planes
//                    of normals at intersections point.
//  Output Arguments:
//       pReflect - pointer to the array of rflected vectors.
//  Returns:
//  ippStsNoErr      No errors
*/
IPPAPI(IppStatus,ipprCastReflectionRay_32f,(
       const Ipp32f* const pInc[3],
       const Ipp32s*        pMask,
       const Ipp32f* const pSurfNorm[3],
       Ipp32f*        pReflect[3],
       int len ))

/* /////////////////////////////////////////////////////////////////////////////
//       Surface properties ( pSurfHit, pSurfNorm, pSurfDotIn )
//////////////////////////////////////////////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////////
//  Name:
//      ipprHitPoint3DS0_32f/ipprHitPoint3DM0_32f
//  Purpose:
//      calculates explicit intersection coordinates in world coordinate system for a block
//      of rays from single/multiple origin.
//  Input Arguments:
//       pDist - generalized distance from origin to intersection point.
//   originEye - origin point. All rays have a single origin.
//     pOrigin - pointer to array of pointers to a separate coordinates(x,y,z)of planes of the origin points.
//  pDirection - pointer to array of pointers to a separate coordinates(x,y,z)of planes of the ray's directions.
//  blockSize  - size of rays' block.
//  Output arguments:
//    pSurfHit - pointer to array of pointers to a separate coordinates(x,y,z)of planes of the intersection points.
//  Returns:
//      ippStsNoErr   No errors
*/
IPPAPI(IppStatus, ipprHitPoint3DEpsS0_32f_M,(
       const IppPoint3D_32f    originEye,
       const Ipp32f* const pDirection[3],
       const Ipp32f*               pDist,
       const Ipp32s*               pMask,
       Ipp32f*               pSurfHit[3],
       int                           len,
       Ipp32f                        eps
       ))
IPPAPI(IppStatus, ipprHitPoint3DEpsM0_32f_M,(
       const Ipp32f* const       pOrigin[3],
       const Ipp32f* const pDirection[3],
       const Ipp32f*               pDist,
       const Ipp32s*               pMask,
       Ipp32f*               pSurfHit[3],
       int                           len,
       Ipp32f                        eps
       ))


/* /////////////////////////////////////////////////////////////////////////////
//  Names:     ipprSurfTriangleNormal_32f
//  Purpose:   calculates the surface's normals from triangles' normals.
//
// Input Arguments:
//    pTrnglNorm    - pointer to the triangles' normal. Interlived
//    pTrngl        - pointer to triangles' indexes
//    pHit          - pointer to the local surface parameters( u, v )at hit point in case of intersection was found.
//    blockSize     - size of rays' block.
//  Output Arguments:
//    pSurfNorm     - pointer to the surface's normals.
//    sameTri ???
//  Returns:
//  ippStsNoErr      No errors
*/
IPPAPI(IppStatus, ipprSurfFlatNormal_32f,(
       const Ipp32f*  pTrnglNorm,
       const Ipp32s*      pTrngl,
       Ipp32f*      pSurfNorm[3],
       int                   len
       ))
IPPAPI(IppStatus, ipprSurfSmoothNormal_32f,(
       const Ipp32f* pVertNorm,
       const Ipp32s* pIndexNorm,
       const Ipp32s* pTrngl,
       const Ipp32f* const pHit[2],
       Ipp32f* pSurfNorm[3], int len, IpprIndexType ippInd
       ))

/* /////////////////////////////////////////////////////////////////////////////
//           Helper Functions
//////////////////////////////////////////////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////////
//  Names:        ipprSetBoundBox_32f
//  Purpose:   Calculates an axis aligned bounding box for the object.
//
// Input Arguments:
//    pVertCoor     - pointer to the coordinates of triangle's vertexes.
//    lenTri        - the number of triangles in the mesh.
// Output Arguments:
//             pBound  - pointer to the axis aligned bounding box of current object.
//  Returns:
//  ippStsNoErr      No errors
*/
IPPAPI(IppStatus, ipprSetBoundBox_32f,(
       const Ipp32f* pVertCoor,
       int             lenTri,
       IppBox3D_32f*   pBound
    ))
/* /////////////////////////////////////////////////////////////////////////////
//  Names:     ipprTriangleNormal_32f
//  Purpose:   calculates triangles' normals from object.
//
// Input Arguments:
//    pTrnglCoor      - pointer to the coordinates of triangle's vertexes.
//    pTrnglIndex     - pointer to the triangle's indexes.
//    lenTri          - the number of triangles in the mesh.
//  Output Arguments:
//    pTrnglNorm      - pointer to the triangles' normals.
//  Returns:
//  ippStsNoErr      No errors
*/
IPPAPI(IppStatus, ipprTriangleNormal_32f,(
       const Ipp32f* pTrnglCoor,
       const int*   pTrnglIndex,
       Ipp32f*       pTrnglNorm,
       int             lenTrngl
    ))

#ifdef __cplusplus
}
#endif

#if defined (_IPP_STDCALL_CDECL)
  #undef  _IPP_STDCALL_CDECL
  #define __stdcall __cdecl
#endif

#endif /* __IPPR_H__ */
/* ////////////////////////////// End of file /////////////////////////////// */

