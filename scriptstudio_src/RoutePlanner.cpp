
#include "stdafx.h"

#include "RoutePlanner.h"

#pragma optimize( "t", on ) 


CRoutePlanner::CRoutePlanner() : m_pNet(NULL)
{
}

CRoutePlanner::~CRoutePlanner()
{
   if( m_pNet != NULL ) delete [] m_pNet;
}

BOOL CRoutePlanner::Init(int cx, int cy, int iScale)
{
   ATLASSERT(m_pNet==NULL);
   ATLASSERT(m_iScale!=0);

   m_iScale = iScale;
   m_sizeMaze.cx = (cx / m_iScale) + 2;
   m_sizeMaze.cy = (cy / m_iScale) + 2;
   
   m_pNet = new NODE[ m_sizeMaze.cx * m_sizeMaze.cy ];
   if( m_pNet == NULL ) return FALSE;

   DWORD dwSize = m_sizeMaze.cx * m_sizeMaze.cy;
   m_dwByteSize = dwSize * sizeof(NODE);

   // Clear array
   // Block edges so route algorithm stays within borders
   // Note how size of maze is +2 to add these extra wall-nodes
   for( DWORD i = 0; i < dwSize; i++ ) {
      m_pNet[i].lr = NODE_OPEN;
      m_pNet[i].ud = NODE_OPEN;
   }
   for( int x = 0; x < m_sizeMaze.cx; x++ ) {
      m_pNet[x].lr = NODE_EDGE;
      m_pNet[x].ud = NODE_EDGE;
      m_pNet[ x + ((m_sizeMaze.cy - 1) * m_sizeMaze.cx) ].lr = NODE_EDGE;
      m_pNet[ x + ((m_sizeMaze.cy - 1) * m_sizeMaze.cx) ].ud = NODE_EDGE;
   }
   for( int y = 0; y < m_sizeMaze.cy; y++ ) {
      m_pNet[ y * m_sizeMaze.cx ].lr = NODE_EDGE;
      m_pNet[ y * m_sizeMaze.cx ].ud = NODE_EDGE;
      m_pNet[ (m_sizeMaze.cx - 1 ) + (y * m_sizeMaze.cx) ].lr = NODE_EDGE;
      m_pNet[ (m_sizeMaze.cx - 1 ) + (y * m_sizeMaze.cx) ].ud = NODE_EDGE;
   }

   return TRUE;
}

BOOL CRoutePlanner::BlockNode(RECT rcItem, WORD wID)
{
   ATLASSERT(rcItem.left<=rcItem.right);
   ATLASSERT(rcItem.top<=rcItem.bottom);

   rcItem.left /= m_iScale;
   rcItem.right /= m_iScale;
   rcItem.top /= m_iScale;
   rcItem.bottom /= m_iScale;
   
   if( rcItem.left < 0 || rcItem.top < 0 ) return FALSE;
   if( rcItem.right > m_sizeMaze.cx - 2 || rcItem.bottom > m_sizeMaze.cy - 2 ) return FALSE;

   for( int x = rcItem.left - 1; x < rcItem.right; x++ ) {
      if( x >= 0 && x <= m_sizeMaze.cx - 2 ) {
         for( int y = rcItem.top; y <= rcItem.bottom; y++ ) {
            if( y >= 0 && y <= m_sizeMaze.cy - 2 ) {
               int iIndex = ((y + 1) * m_sizeMaze.cx) + (x + 1);
               m_pNet[ iIndex ].lr = wID;
               m_pNet[ iIndex ].ud = wID;
            }
         }
      }
   }
   
   return TRUE;
}

BOOL CRoutePlanner::BlockEdge(RECT rcItem, WORD wID)
{
   if( rcItem.left > rcItem.right ) {
      int iTemp = rcItem.left;
      rcItem.left = rcItem.right;
      rcItem.right = iTemp;
   }
   if( rcItem.top > rcItem.bottom ) {
      int iTemp = rcItem.top;
      rcItem.top = rcItem.bottom;
      rcItem.bottom = iTemp;
   }

   rcItem.left /= m_iScale;
   rcItem.right /= m_iScale;
   rcItem.top /= m_iScale;
   rcItem.bottom /= m_iScale;

   if( rcItem.left < 0 || rcItem.top < 0 ) return FALSE;
   if( rcItem.right >= m_sizeMaze.cx - 2 || rcItem.bottom >= m_sizeMaze.cy - 2 ) return FALSE;

   for( int x = rcItem.left; x <= rcItem.right; x++ ) {
      if( x >= 0 && x <= m_sizeMaze.cx - 2 ) {
         for( int y = rcItem.top; y <= rcItem.bottom; y++ ) {
            if( y >= 0 && y <= m_sizeMaze.cy - 2 ) {
               int iIndex = ((y + 1) * m_sizeMaze.cx) + (x + 1);
               if( rcItem.left != rcItem.right ) m_pNet[ iIndex ].lr = wID;
               if( rcItem.top != rcItem.bottom ) m_pNet[ iIndex ].ud = wID;
            }
         }
      }
   }
   
   return TRUE;
}

BOOL CRoutePlanner::BlockRoute(POINT* pPoints, int nPoints, WORD wID)
{
   ATLASSERT(pPoints);

   if( wID == 0 ) return FALSE;
   if( nPoints < 3 ) return FALSE;

   wID += ROUTE_ID_OFFSET;

   for( int i = 1; i < nPoints - 2; i++ ) {
      RECT rc = { pPoints[i].x, pPoints[i].y, pPoints[i + 1].x, pPoints[i + 1].y };
      BlockEdge(rc, wID);
   }

   return TRUE;
}

int CRoutePlanner::PlanRoute(POINT ptSrc, POINT ptDest, WORD wDestID, LPPOINT pResult, int cchMax)
{
   wDestID += ROUTE_ID_OFFSET;

   POINT ptS;
   ptS.x = (ptSrc.x / m_iScale);
   ptS.y = (ptSrc.y / m_iScale);

   POINT ptD;
   ptD.x = (ptDest.x / m_iScale) - 2;
   ptD.y = (ptDest.y / m_iScale);
  
   if( ptS.x < 0 || ptS.x > m_sizeMaze.cx - 2 ) return 0;
   if( ptS.y < 0 || ptS.y > m_sizeMaze.cy - 2 ) return 0;
   if( ptD.x < 0 || ptD.x > m_sizeMaze.cx - 2 ) return 0;
   if( ptD.y < 0 || ptD.y > m_sizeMaze.cy - 2 ) return 0;

   NODE* pNet = new NODE[ m_sizeMaze.cx * m_sizeMaze.cy ];
   if( pNet == NULL ) return 0;
   ::CopyMemory(pNet, m_pNet, m_dwByteSize);

   int iIndex;

   iIndex = ((ptS.y + 1) * m_sizeMaze.cx) + (ptS.x + 1);
   pNet[ iIndex ].lr = 0;
   pNet[ iIndex ].ud = 0;
   iIndex = ((ptD.y + 1) * m_sizeMaze.cx) + (ptD.x + 1);
   pNet[ iIndex ].lr = NODE_TARGET;
   pNet[ iIndex ].ud = NODE_TARGET;

   typedef struct tagQUEUE
   {
      WORD x;
      WORD y;
   } QUEUE;

   const int iMaxQueueSize = m_sizeMaze.cx * m_sizeMaze.cy * 4;
   QUEUE* q = new QUEUE[iMaxQueueSize];
   q[0].x = (WORD)( ptS.x + 1 );
   q[0].y = (WORD)( ptS.y + 1 );

#define explore_lr(i, j) \
      iIndex = (i) + m_sizeMaze.cx * (j); \
      v = pNet[iIndex].lr; \
      if( v == NODE_TARGET ) \
        bFound = true; \
      else if( v == NODE_OPEN || v == wDestID ) { \
        pNet[iIndex].lr = (WORD) (d + 1U); \
        q[qt].x = (WORD) (i); \
        q[qt].y = (WORD) (j); \
        qt++; \
      }

#define explore_ud(i, j) \
      iIndex = (i) + m_sizeMaze.cx * (j); \
      v = pNet[iIndex].ud; \
      if( v == NODE_TARGET ) \
        bFound = true; \
      else if( v == NODE_OPEN || v == wDestID ) { \
        pNet[iIndex].ud = (WORD) (d + 1U); \
        q[qt].x = (WORD) (i); \
        q[qt].y = (WORD) (j); \
        qt++; \
      }

   // Wave Propagation
   bool bFound = false;
   WORD d = 0;
   WORD dt = 0;
   int qh = 0;
   int qt = 1;
   int i = 0;
   int j = 0;
   WORD v = 0;
   while( !bFound 
          && qh < qt 
          && qt < iMaxQueueSize - 4 ) 
   {
      i = q[qh].x;
      j = q[qh].y;
      qh++;
      d = pNet[i + m_sizeMaze.cx * j].lr;
      dt = pNet[i + m_sizeMaze.cx * j].ud;
      if( dt < d ) d = dt;
      explore_lr(i + 1, j);
      explore_lr(i - 1, j);
      explore_ud(i, j + 1);
      explore_ud(i, j - 1);
   }

   CSimpleArray<POINT> aPoints;

   // Collect shortest route
   if( bFound ) 
   {
      POINT pt = { i, j };
      char cDir = 0;
      char cLastDir = 0;
      int old_i = 0;
      int old_j = 0;
      while( d > 0 ) {
         old_i = i;
         old_j = j;
         if( pNet[(i - 1) + m_sizeMaze.cx * j].lr == d - 1 ) {
            i--, d--, cDir = 'h';
         }
         else if( pNet[(i - 1) + m_sizeMaze.cx * j].ud == d - 1 ) {
            i--, d--, cDir = 'h';
         }
         else if( pNet[(i + 1) + m_sizeMaze.cx * j].lr == d - 1 ) {
            i++, d--, cDir = 'h';
         }
         else if( pNet[(i + 1) + m_sizeMaze.cx * j].ud == d - 1 ) {
            i++, d--, cDir = 'h';
         }
         else if( pNet[i + m_sizeMaze.cx * (j + 1)].lr == d - 1 ) {
            j++, d--, cDir = 'v';
         }
         else if( pNet[i + m_sizeMaze.cx * (j + 1)].ud == d - 1 ) {
            j++, d--, cDir = 'v';
         }
         else if( pNet[i + m_sizeMaze.cx * (j - 1)].lr == d - 1 ) {
            j--, d--, cDir = 'v';
         }
         else if( pNet[i + m_sizeMaze.cx * (j - 1)].ud == d - 1 ) {
            j--, d--, cDir = 'v';
         }
         else {
            // No route to be found...
            break;
         }
         if( cDir != cLastDir ) {
            // Change of dircetion = new edge
            pt.x = old_i;
            pt.y = old_j;
            aPoints.Add(pt);
            cLastDir = cDir;
         }
      }
   }

   delete [] q;
   delete [] pNet;

   int nPoints = 0;

   // Support-points: Start- and end-point + the first leg of the start-line!
   const int NUM_SUPPORT_POINTS = 3;

   if( !bFound || d > 0 || aPoints.GetSize() > cchMax - NUM_SUPPORT_POINTS ) 
   {
      // Fails...
   }
   else 
   {
      POINT ptFirst = { i, j };
      pResult[nPoints++] = ptSrc;
      pResult[nPoints++] = ptFirst;
      for( int j = aPoints.GetSize() - 1; j >= 0; j-- ) pResult[nPoints++] = aPoints[j];     
      pResult[nPoints++] = ptDest;

      // Scale coords back to view-coords
      for( int i = 1; i < nPoints - 1; i++ ) {
         POINT& pt = pResult[i];
         pt.x = ((pt.x - 1) * m_iScale) + (m_iScale / 2);
         pt.y = ((pt.y - 1) * m_iScale) + (m_iScale / 2);
      }

      // Adjust the y-coord for the first few points so
      // they align with the start-point.
      if( nPoints >= 3 ) {
         for( int i = 1; i <= 2; i++ ) {
            if( abs(pResult[i].y - ptSrc.y) < 4 ) {
               pResult[i].y = ptSrc.y;
            }
         }
      }
   }

   return nPoints;
}

void CRoutePlanner::Dump()
{
#ifdef _DEBUG
   FILE* f = fopen("C:\\temp\\route.txt", "wt");
   for( int xx = 0; xx < 40; xx ++ ) {
      for( int yy = 0; yy < 40; yy ++ ) {
         char szData[64];
         ::wsprintf(szData, "%02X,%02X ", 
            m_pNet[xx + yy * m_sizeMaze.cx].ud & 0xFF,
            m_pNet[xx + yy * m_sizeMaze.cx].lr & 0xFF);
         fputs(szData, f);
      }
      fputs("\n", f);
   }
   fclose(f);
#endif
}


#pragma optimize( "t", off )
