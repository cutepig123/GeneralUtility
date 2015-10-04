// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * Global variable containing the query we'd like to pass to Flickr. In this
 * case, kittens!
 *
 * @type {string}
 */
var QUERY = 'kittens';

var kittenGenerator = {
  /**
   * Flickr URL that will give us lots and lots of whatever we're looking for.
   *
   * See http://www.flickr.com/services/api/flickr.photos.search.html for
   * details about the construction of this URL.
   *
   * @type {string}
   * @private
   */
  searchOnFlickr_: 'https://secure.flickr.com/services/rest/?' +
      'method=flickr.photos.search&' +
      'api_key=90485e931f687a9b9c2a66bf58a3861a&' +
      'text=' + encodeURIComponent(QUERY) + '&' +
      'safe_search=1&' +
      'content_type=1&' +
      'sort=interestingness-desc&' +
      'per_page=20',

  /**
   * Sends an XHR GET request to grab photos of lots and lots of kittens. The
   * XHR's 'onload' event is hooks up to the 'showPhotos_' method.
   *
   * @public
   */
  requestKittens: function() {
    var req = new XMLHttpRequest();
    req.open("GET", this.searchOnFlickr_, true);
    req.onload = this.showPhotos_.bind(this);
    req.send(null);
  },

  /**
   * Handle the 'onload' event of our kitten XHR request, generated in
   * 'requestKittens', by generating 'img' elements, and stuffing them into
   * the document for display.
   *
   * @param {ProgressEvent} e The XHR ProgressEvent.
   * @private
   */
  showPhotos_: function (e) {
    var kittens = e.target.responseXML.querySelectorAll('photo');
    for (var i = 0; i < kittens.length; i++) {
      var img = document.createElement('img');
      img.src = this.constructKittenURL_(kittens[i]);
      img.setAttribute('alt', kittens[i].getAttribute('title'));
      document.body.appendChild(img);
    }
  },

  /**
   * Given a photo, construct a URL using the method outlined at
   * http://www.flickr.com/services/api/misc.urlKittenl
   *
   * @param {DOMElement} A kitten.
   * @return {string} The kitten's URL.
   * @private
   */
  constructKittenURL_: function (photo) {
    return "http://farm" + photo.getAttribute("farm") +
        ".static.flickr.com/" + photo.getAttribute("server") +
        "/" + photo.getAttribute("id") +
        "_" + photo.getAttribute("secret") +
        "_s.jpg";
  }
};

var svgNS = "http://www.w3.org/2000/svg";

var mySvgGenerator={
  
  showHtml: function () {
    
    for (var i = 0; i < 10; i++) {
      var img = document.createElement('div');
      img.innerText = "xx" + i.toString();
    //  img.setAttribute('alt', kittens[i].getAttribute('title'));
      document.body.appendChild(img);
    }
  },
  
  newRect: function(x,y,w,h,color){
	
	var img = document.createElementNS(svgNS,'rect');
      img.setAttribute('x', x);
	  img.setAttribute('y', y);
	  img.setAttribute('width', w);
	  img.setAttribute('height', h);
	  img.setAttribute('style', "stroke:#a08071;stroke-width:1");
	  return img;
  },
  
  newText: function(x,y,text,color){
	var img = document.createElementNS(svgNS,'text');
      img.setAttribute('x', x);
	  img.setAttribute('y', y);
	  img.textContent =text;
	  img.setAttribute('fill', "red");
	  return img;
  },
  
  newG: function(){
	var img = document.createElementNS(svgNS,'g');
      //img.setAttribute('transform', transform);
	  img.setAttribute('fill', 'none');
	  return img;
  },
  
  showSvg: function () {
	var mySvg=document.getElementById("mySvg");
	
    var svg = document.createElementNS(svgNS,'svg');
	svg.setAttribute('width', 600);
	svg.setAttribute('height', 600);
	mySvg.appendChild(svg);
	
	//var g=this.newG();
	//svg.appendChild(g);
	var g=svg;
	
	var text=this.newText(522,20,"Log Path = newText","red");
	g.appendChild(text);
	
    for (var i = 0; i < 10; i++) {
		//svg.appendChild(this.newText(0,100*i,'x'+i));
    }
  }
 };

// Run our kitten generation script as soon as the document's DOM is ready.
/*document.addEventListener('DOMContentLoaded', function () {
  //kittenGenerator.requestKittens();
  mySvgGenerator.showSvg();
});*/

function test(){
	var xmlhttp = new XMLHttpRequest(); 
	xmlhttp.open("GET","post.xml",false); 
	xmlhttp.send(""); 
	xmlDoc = xmlhttp.responseXML; 
	//var tablink2 = xmlDoc.getElementsByTagName("alias")[0].childNodes[0].nodeValue;
	alert(xmlDoc);
}

function loadXMLDoc()
{
	var xmlhttp;
	xmlhttp=new XMLHttpRequest();
	  
	xmlhttp.onreadystatechange=function(){
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{
			document.getElementById("myDiv").innerText=xmlhttp.responseText;
			//alert(xmlhttp.responseText);
		}
		else
			document.getElementById("myDiv").innerText="Error open file!";
	};
	  
	xmlhttp.open("GET","post.xml",true);
	xmlhttp.send();
}

function show_coords(event)
{
var x=event.clientX;
var y=event.clientY;
var id=document.getElementById("Position");
id.innerText =("X:" + x + ", Y:" + y);
}

document.addEventListener('DOMContentLoaded', function () {
 loadXMLDoc();
 mySvgGenerator.showSvg();
});

document.addEventListener('mousedown', function (e) {
  show_coords(e);
});
