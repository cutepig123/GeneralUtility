<?xml version="1.0"?>

<!-- 
SimpStyler v.0.2
This is a simple xsl stylesheet for TDL. 

Change Log, 4 Dec 2007:
- Better Documentation
- When EXTERNALID="NBR" (No BRacket), do not print [ ] brackets on open tasks.
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:strip-space elements="*" />

<xsl:template match="TODOLIST">
<html>
<head>
    <title><xsl:value-of select="@PROJECTNAME" /></title>
    
    <!-- STYLESHEET. Use stylesheet to setup font type, font size, margins, etc. -->
    <style>
    body
					{
						font-family: Tahoma, serif;
						margin: 5px 5px 5px 5px;
						background-color : #C0C0C0;
						font-size: 11px;
					}
          
    .done {
        color: #808080;
    }
    .alldone {
        color: #808080;
        <!-- text-decoration: line-through;-->
    }
    .comment {
        color: #606060;
    }
    body {
        font-family: Arial;
    }
    </style>
</head>
<body>
    <!-- TITLE. Too big? Too Small? Change the h2 to the size you want. -->
    <h2>Project: <xsl:value-of select="@PROJECTNAME" /></h2>
    <xsl:value-of select="@REPORTDATE" />
    <ol>
    <xsl:apply-templates />
    </ol>
</body>
</html>
</xsl:template>

<xsl:template match="TASK">
    <xsl:choose>
        <xsl:when test="TASK">
            <xsl:call-template name="do-task" />
            <ul>
            <xsl:apply-templates />
            </ul>
        </xsl:when>
        <xsl:otherwise>
            <xsl:call-template name="do-task" />
            <xsl:apply-templates />
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="do-task">
<!-- FORMAT. This is where the we put the [ ] and [x] brackets, and format. -->
    <xsl:choose>
    <!-- This automatically selects the "MASTER" Task -->
        <xsl:when test="parent::TODOLIST">
          <xsl:choose>
          <!-- Format Completed Master Task -->
            <xsl:when test="@DONEDATESTRING">
              <BR></BR>
              <span class="alldone"><b><xsl:value-of select="@TITLE" /></b></span>
              <xsl:text> (</xsl:text><xsl:value-of select="@DONEDATESTRING" /><xsl:text>)</xsl:text>
              <BR></BR>
            </xsl:when>
          <!-- Format Open Masters Task -->
            <xsl:otherwise>
              <BR></BR>
              <b><xsl:value-of select="@TITLE" /></b>
            </xsl:otherwise>

            </xsl:choose>
        </xsl:when>
   
   <!-- This automatically selects the "SUB" Tasks -->
    <xsl:otherwise>    
        <xsl:choose>
            <!-- Format Completed Sub Task -->
            <xsl:when test="@DONEDATESTRING">
                <span class="alldone">[x]<xsl:value-of select="@TITLE" /> (<xsl:value-of select="@CREATIONDATESTRING" /> -> <xsl:value-of select="@DONEDATESTRING" />)</span>
                <BR></BR>
            </xsl:when>
            <!-- Format Completed Sub Task -->
            <xsl:otherwise>
                <xsl:choose>
                
                  <xsl:when test="@EXTERNALID='NBR'">
                  <xsl:value-of select="@TITLE" />
                  <BR></BR>
                  </xsl:when>
                  
                  <xsl:otherwise>
                  <xsl:text>[</xsl:text>&#160;&#160;<xsl:text>] </xsl:text>
                  <xsl:value-of select="@TITLE" /> (<xsl:value-of select="@CREATIONDATESTRING" /> -> <xsl:value-of select="@DONEDATESTRING" />)
                  <BR></BR>
                  </xsl:otherwise>
                  
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>  
     </xsl:otherwise>
        
    </xsl:choose>
    
    </xsl:template>
</xsl:stylesheet>