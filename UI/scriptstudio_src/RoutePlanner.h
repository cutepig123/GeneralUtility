#if !defined(AFX_ROUTEPLANNER_H__20040815_5334_84CC_17A1_0080AD509054__INCLUDED_)
#define AFX_ROUTEPLANNER_H__20040815_5334_84CC_17A1_0080AD509054__INCLUDED_

#pragma once


#define NODE_EDGE    0xFFFF
#define NODE_BLOCKED 0xFFFF
#define NODE_TARGET  0xEEEE
#define NODE_OPEN    0x8000


class CRoutePlanner
{
public:
   enum
   {
      ROUTE_ID_OFFSET = 0x4000,
   };

   typedef struct tagNODE
   {
      WORD lr;
      WORD ud;
   } NODE;

   CRoutePlanner();
   ~CRoutePlanner();

   BOOL Init(int cx, int cy, int iScale);
   BOOL BlockNode(RECT rcItem, WORD wID = NODE_BLOCKED);
   BOOL BlockEdge(RECT rcItem, WORD wID = NODE_BLOCKED);
   BOOL BlockRoute(POINT* pPoints, int nPoints, WORD wID);
   int PlanRoute(POINT ptSrc, POINT ptDest, WORD wDestID, LPPOINT pResult, int cchMax);

   void Dump();

   NODE* m_pNet;
   int m_iScale;
   SIZE m_sizeMaze;
   DWORD m_dwByteSize;
};


#endif // !defined(AFX_ROUTEPLANNER_H__20040815_5334_84CC_17A1_0080AD509054__INCLUDED_)

