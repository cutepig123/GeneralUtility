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
	var img = document.createElement('rect');
      img.setAttribute('x', x);
	  img.setAttribute('y', y);
	  img.setAttribute('width', w);
	  img.setAttribute('height', h);
	  img.setAttribute('style', "stroke:#a08071;stroke-width:1");
	  return img;
  },
  
  newText: function(x,y,text){
	var img = document.createElement('text');
      img.setAttribute('x', x);
	  img.setAttribute('y', y);
	  img.innerHTML =text;
	  //img.setAttribute('style', "stroke:#a08071;stroke-width:1");
	  return img;
  },
  
  newG: function(transform){
	var img = document.createElement('g');
      img.setAttribute('transform', transform);
	  return img;
  },
  
  showSvg: function () {
    var svg = document.createElement('svg');
	svg.setAttribute('width', 800);
	svg.setAttribute('height', 800);
	
	document.body.appendChild(svg);
	
	var rect=this.newRect(0,0,600,600);
	var g=this.newG("");
	g.appendChild(rect);
	svg.appendChild(g);
	
    for (var i = 0; i < 10; i++) {
		svg.appendChild(this.newText(0,100*i,'x'+i));
    }
  }
 };

// Run our kitten generation script as soon as the document's DOM is ready.
document.addEventListener('DOMContentLoaded', function () {
  //kittenGenerator.requestKittens();
  mySvgGenerator.showSvg();
});
