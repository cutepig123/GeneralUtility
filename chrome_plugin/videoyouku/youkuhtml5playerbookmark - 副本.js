//youku
(function(){
	if(!/youku\.com/.test(window.location.href)){
		return false;
	}
	if(!window.videoId){
		return false;
	}
	var coverCss = [
		'',
		'position:fixed',
		'top:0',
		'left:0',
		'bottom:0',
		'right:0',
		'background-color:rgba(255,255,255,0)',
		'z-index:999999999999',
		'-webkit-transition:background-color 0.2s ease;',
		'-moz-transition:background-color 0.2s ease;',
		'-o-transition:background-color 0.2s ease;',
		'transition:background-color 0.2s ease;',
		'pointer-events:none',
		''
	];
	var videoCss = [
		'',
		'position:absolute',
		'width:610px',
		'height:500px',
		'top:-500px',
		'left:50%',
		'margin-left:-306px',
		'z-index:1000000000000',
		'background:#000',
		'box-shadow:0 0 5px #333',
		'-webkit-transition:top 1s ease;',
		'-moz-transition:top 1s ease;',
		'-o-transition:top 1s ease;',
		'transition:top 1s ease;',
		''
	];
	var aCss = [
		'',
		'position:absolute',
		'bottom:0',
		'left:0',
		'right:0',
		'height:30px;',
		'text-align:center',
		'font-size:14px',
		'pointer-events:auto',
		''
	];
	
	var btnCss = [
		'',
		'position:absolute',
		'top:0',
		'height:30px;',
		'line-height:30px',
		'width:60px',
		'text-align:center',
		'font-size:14px',
		'letter-spacing:-1px',
		'color:#014CCC',
		'cursor:pointer',
		'pointer-events:auto',
		''
	];
	
	var changeBtnCss = [
		'',
		'position:absolute',
		'width:500px',
		'height:40px',
		'top:500px',
		'left:50%',
		'margin-left:-250px',
		'z-index:1000000000000',
		'background:#000',
		'background-image: -webkit-linear-gradient(top,#333,#000)',
		'background-image: -moz-linear-gradient(top,#333,#000)',
		'background-image: -ms-linear-gradient(top,#333,#000)',
		'background-image: -o-linear-gradient(top,#333,#000)',
		'background-image: linear-gradient(top,#333,#000)',
		'box-shadow:0 0 5px #333',
		'text-align:center',
		'color:#eee',
		'border-radius:150px',
		'overflow:hidden',
		''
	];
	
	var changeBtnItemCss = [
		'',
		'display:inline-block',
		'width:90px',
		'height:40px',
		'line-height:40px',
		'font-size:20px',		
		'cursor:pointer',
		''
	];
	
	
	var HTML5Player = function(){
	
		var hd2Src= '/player/getM3U8/vid/'+videoId+'/type/hd2/ts/'+(((new Date()).getTime()/1000).toString()|0)+'/v.m3u8';
		var mp4Src= '/player/getM3U8/vid/'+videoId+'/type/mp4/ts/'+(((new Date()).getTime()/1000).toString()|0)+'/v.m3u8';
		var flvSrc= '/player/getM3U8/vid/'+videoId+'/type/flv/ts/'+(((new Date()).getTime()/1000).toString()|0)+'/v.m3u8';
		var mp4Src2 = 'http://3g.youku.com/pvs?id='+videoId2+'&format=3gphd';
		//var m3u8Src= '/player/getM3U8/vid/'+videoId+'/type/mp4/flv/ts/'+(new Date()).getTime()+'/v.m3u8';
		var cover = document.createElement('div');
		cover.style.cssText += coverCss.join(';');

		var v = document.createElement('video');
		v.setAttribute('height','500');
		v.setAttribute('width','610');
		v.setAttribute('controls','true');
		v.setAttribute('autoplay','true');
		v.style.cssText += videoCss.join(';');
		v.src = hd2Src;
		
		
		var a = document.createElement('a');		
		a.setAttribute('href',mp4Src);
		a.innerHTML = '&#x4E0D;&#x80FD;&#x76F4;&#x63A5;&#x89C2;&#x770B;&#xFF0C;&#x70B9;&#x51FB;&#x8FD9;&#x91CC;&#xFF1A;'+mp4Src;
		a.style.cssText += aCss.join(';');
		cover.appendChild(a);
		
		var off = document.createElement('div');
		off.innerHTML = '&#x9000;&#x51FA;';
		off.style.cssText += btnCss.join(';')+';right:0;';
		cover.appendChild(off);
		var nocover = document.createElement('div');
		nocover.innerHTML = '&#x6253;&#x5F00;&#x8FF7;&#x96FE;';
		nocover.style.cssText += btnCss.join(';')+';right:60px;';;
		cover.appendChild(nocover);
		
		off.addEventListener('click',function(){
			y.HTML5.remove();
			y.flash.add();
			y.flag = false;
		},false);
		
		nocover.addEventListener('click',function(){
			cover.style.backgroundColor = 'rgba(255,255,255,0)';
		},false);
			
		var changeBtn = document.createElement('div');
		changeBtn.style.cssText += changeBtnCss.join(';');
		
				
		var hd2btn = document.createElement('div');
		hd2btn.style.cssText += changeBtnItemCss.join(';');
		hd2btn.style.backgroundColor = '#666';
		hd2btn.style.boxShadow = '0 0 3px #000 inset';
		hd2btn.innerHTML = '&#x8D85;&#x6E05;';
		changeBtn.appendChild(hd2btn);
				
		var mp4btn = document.createElement('div');
		mp4btn.style.cssText += changeBtnItemCss.join(';');
		mp4btn.innerHTML = '&#x9AD8;&#x6E05;';
		changeBtn.appendChild(mp4btn);
		
		var flvbtn = document.createElement('div');
		flvbtn.style.cssText += changeBtnItemCss.join(';');
		flvbtn.innerHTML = '&#x6807;&#x6E05;';
		changeBtn.appendChild(flvbtn);
		
		var mp4btn2 = document.createElement('div');
		mp4btn2.style.cssText += changeBtnItemCss.join(';');
		mp4btn2.innerHTML = 'mp4';
		changeBtn.appendChild(mp4btn2);
		
		hd2btn.addEventListener('click',function(){
			v.src = hd2Src;
			hd2btn.style.backgroundColor = '#666';			
			hd2btn.style.boxShadow = '0 0 3px #000 inset';
			mp4btn.style.backgroundColor = 'transparent';
			mp4btn.style.boxShadow = 'none';
			flvbtn.style.backgroundColor = 'transparent';
			flvbtn.style.boxShadow = 'none';
			mp4btn2.style.backgroundColor = 'transparent';
			mp4btn2.style.boxShadow = 'none';
		},false);
		
		mp4btn.addEventListener('click',function(){
			v.src = mp4Src;
			mp4btn.style.backgroundColor = '#666';
			mp4btn.style.boxShadow = '0 0 3px #000 inset';
			hd2btn.style.backgroundColor = 'transparent';
			hd2btn.style.boxShadow = 'none';
			flvbtn.style.backgroundColor = 'transparent';
			flvbtn.style.boxShadow = 'none';
			mp4btn2.style.backgroundColor = 'transparent';
			mp4btn2.style.boxShadow = 'none';
		},false);
		
		flvbtn.addEventListener('click',function(){
			v.src = flvSrc;
			flvbtn.style.backgroundColor = '#666';
			flvbtn.style.boxShadow = '0 0 3px #000 inset';
			hd2btn.style.backgroundColor = 'transparent';
			hd2btn.style.boxShadow = 'none';
			mp4btn.style.backgroundColor = 'transparent';
			mp4btn.style.boxShadow = 'none';
			mp4btn2.style.backgroundColor = 'transparent';
			mp4btn2.style.boxShadow = 'none';
		},false);
		
		mp4btn2.addEventListener('click',function(){
			v.src = mp4Src2;			
			mp4btn2.style.backgroundColor = '#666';
			mp4btn2.style.boxShadow = '0 0 3px #000 inset';
			hd2btn.style.backgroundColor = 'transparent';
			hd2btn.style.boxShadow = 'none';
			flvbtn.style.backgroundColor = 'transparent';
			flvbtn.style.boxShadow = 'none';
			mp4btn.style.backgroundColor = 'transparent';
			mp4btn.style.boxShadow = 'none';
		},false);
		
		return {
			add : function(){
				document.body.appendChild(cover);
				document.body.appendChild(changeBtn);
				document.body.appendChild(v);
				
				v.addEventListener('canplay',v.play);
				setTimeout(function(){
					cover.style.backgroundColor = 'rgba(255,255,255,0.6)';
					v.style.top = '-1px';
				},100);
			},
			remove : function(){
				v.pause();
				cover.style.backgroundColor = 'rgba(255,255,255,0)';
				v.style.top = '-500px';
				setTimeout(function(){
					cover.parentNode && document.body.removeChild(cover);
					v.parentNode && document.body.removeChild(v);
					changeBtn.parentNode && document.body.removeChild(changeBtn);
				},1100);
			}
		}
	}

	var flashPlayer = function(){
		var flash = document.getElementById('movie_player');
		var flashOut = flash.parentNode;
		return {
			add : function(){
				flashOut.appendChild(flash);
			},
			remove : function(){
				flashOut.parentNode && flashOut.removeChild(flash);
			}
		}
	}	
	
	window.isYoukuHTML5PlayerBookMarkCodeByZythum = window.isYoukuHTML5PlayerBookMarkCodeByZythum || {};
	var y = window.isYoukuHTML5PlayerBookMarkCodeByZythum;
	y.HTML5  = y.HTML5 || HTML5Player();
	y.flash = y.flash || flashPlayer();
	y.flag = y.flag || false;
	if(y.flag === false){
		y.HTML5.add();
		y.flash.remove();
		y.flag = true;
	}else if(y.flag === true){
		y.HTML5.remove();
		y.flash.add();
		y.flag = false;
	}

})();

//tudou
(function(){
	if(!/tudou\.com/.test(window.location.href)){
		return false;
	}
	if(!window.iid){
		return false;
	}
	var coverCss = [
		'',
		'position:fixed',
		'top:0',
		'left:0',
		'bottom:0',
		'right:0',
		'background-color:rgba(255,255,255,0)',
		'z-index:999999999999',
		'-webkit-transition:background-color 0.2s ease;',
		'-moz-transition:background-color 0.2s ease;',
		'-o-transition:background-color 0.2s ease;',
		'transition:background-color 0.2s ease;',
		'pointer-events:none',
		''
	];
	var videoCss = [
		'',
		'position:absolute',
		'width:610px',
		'height:500px',
		'top:-500px',
		'left:50%',
		'margin-left:-306px',
		'z-index:1000000000000',
		'background:#000',
		'box-shadow:0 0 5px #333',
		'-webkit-transition:top 1s ease;',
		'-moz-transition:top 1s ease;',
		'-o-transition:top 1s ease;',
		'transition:top 1s ease;',
		''
	];
	var aCss = [
		'',
		'position:absolute',
		'bottom:0',
		'left:0',
		'right:0',
		'height:30px;',
		'text-align:center',
		'font-size:14px',
		'pointer-events:auto',
		''
	];
	
	var btnCss = [
		'',
		'position:absolute',
		'top:0',
		'height:30px;',
		'line-height:30px',
		'width:60px',
		'text-align:center',
		'font-size:14px',
		'letter-spacing:-1px',
		'color:#014CCC',
		'cursor:pointer',
		'pointer-events:auto',
		''
	];
	
	var changeBtnCss = [
		'',
		'position:absolute',
		'width:500px',
		'height:40px',
		'top:500px',
		'left:50%',
		'margin-left:-250px',
		'z-index:1000000000000',
		'background:#000',
		'background-image: -webkit-linear-gradient(top,#333,#000)',
		'background-image: -moz-linear-gradient(top,#333,#000)',
		'background-image: -ms-linear-gradient(top,#333,#000)',
		'background-image: -o-linear-gradient(top,#333,#000)',
		'background-image: linear-gradient(top,#333,#000)',
		'box-shadow:0 0 5px #333',
		'text-align:center',
		'color:#eee',
		'border-radius:150px',
		'overflow:hidden',
		''
	];
	
	var changeBtnItemCss = [
		'',
		'display:inline-block',
		'width:150px',
		'height:40px',
		'line-height:40px',
		'font-size:20px',		
		'cursor:pointer',
		''
	];
	
	var pad = function(num, n) {
        return (new Array(n >(''+num).length ? (n - (''+num).length+1) : 0).join('0') + num);
	}
	
	var HTML5Player = function(){
		//123/554/149/3
		iidStr = pad(iid,9).match(/(\d{3})(\d{3})(\d{3})/);
		var idEncodeed = iidStr[1] + '/' + iidStr[2] + '/' + iidStr[3];

		var t_yuanhuaSrc = 'http://m3u8.tdimg.com/'+idEncodeed+'/'+'99.m3u8';
		var t_360Src = 'http://m3u8.tdimg.com/'+idEncodeed+'/'+'3.m3u8';
		var t_256Src = 'http://m3u8.tdimg.com/'+idEncodeed+'/'+'2.m3u8';

		var cover = document.createElement('div');
		cover.style.cssText += coverCss.join(';');

		var v = document.createElement('video');
		v.setAttribute('height','500');
		v.setAttribute('width','610');
		v.setAttribute('controls','true');
		v.setAttribute('autoplay','true');
		v.style.cssText += videoCss.join(';');
		v.src = t_yuanhuaSrc;
		
		
		var a = document.createElement('span');		
		a.innerHTML = '&#x76EE;&#x524D;&#x53EA;&#x6709;m3u8&#x683C;&#x5F0F;&#x7684;&#x652F;&#x6301;,&#x8C8C;&#x4F3C;&#x53EA;&#x6709;safair&#x624D;&#x80FD;&#x64AD;&#x653E;&#x3002;';
		a.style.cssText += aCss.join(';');
		cover.appendChild(a);
		
		var off = document.createElement('div');
		off.innerHTML = '&#x9000;&#x51FA;';
		off.style.cssText += btnCss.join(';')+';right:0;';
		cover.appendChild(off);
		var nocover = document.createElement('div');
		nocover.innerHTML = '&#x6253;&#x5F00;&#x8FF7;&#x96FE;';
		nocover.style.cssText += btnCss.join(';')+';right:60px;';
		cover.appendChild(nocover);
		
		off.addEventListener('click',function(){
			y.HTML5.remove();
			y.flash.add();
			y.flag = false;
		},false);
		
		nocover.addEventListener('click',function(){
			cover.style.backgroundColor = 'rgba(255,255,255,0)';
		},false);
		
		var changeBtn = document.createElement('div');
		changeBtn.style.cssText += changeBtnCss.join(';');
		
		var t_yuanhua = document.createElement('div');
		t_yuanhua.style.cssText += changeBtnItemCss.join(';');
		t_yuanhua.style.backgroundColor = '#666';
		t_yuanhua.style.boxShadow = '0 0 3px #000 inset';
		t_yuanhua.innerHTML = '&#x539F;&#x753B;';
		changeBtn.appendChild(t_yuanhua);
		
		var t_360 = document.createElement('div');
		t_360.style.cssText += changeBtnItemCss.join(';');
		t_360.innerHTML = '360P';
		changeBtn.appendChild(t_360);
		
		var t_256 = document.createElement('div');
		t_256.style.cssText += changeBtnItemCss.join(';');
		t_256.innerHTML = '256P';
		changeBtn.appendChild(t_256);

		t_yuanhua.addEventListener('click',function(){
			v.src = t_yuanhuaSrc;
			t_yuanhua.style.backgroundColor = '#666';
			t_360.style.backgroundColor = 'transparent';
			t_256.style.backgroundColor = 'transparent';
			t_yuanhua.style.boxShadow = '0 0 3px #000 inset';
			t_360.style.boxShadow = 'none';
			t_256.style.boxShadow = 'none';
		},false);
		
		t_360.addEventListener('click',function(){
			v.src = t_360Src;
			t_360.style.backgroundColor = '#666';
			t_yuanhua.style.backgroundColor = 'transparent';
			t_256.style.backgroundColor = 'transparent';
			t_360.style.boxShadow = '0 0 3px #000 inset';
			t_yuanhua.style.boxShadow = 'none';
			t_256.style.boxShadow = 'none';
		},false);
		
		t_256.addEventListener('click',function(){
			v.src = t_360Src;
			t_256.style.backgroundColor = '#666';
			t_yuanhua.style.backgroundColor = 'transparent';
			t_360.style.backgroundColor = 'transparent';
			t_256.style.boxShadow = '0 0 3px #000 inset';
			t_yuanhua.style.boxShadow = 'none';
			t_360.style.boxShadow = 'none';
		},false);



		return {
			add : function(){
				document.body.appendChild(cover);
				document.body.appendChild(changeBtn);
				document.body.appendChild(v);
				
				v.addEventListener('canplay',v.play);
				setTimeout(function(){
					cover.style.backgroundColor = 'rgba(255,255,255,0.6)';
					v.style.top = '-1px';
				},100);
			},
			remove : function(){
				v.pause();
				cover.style.backgroundColor = 'rgba(255,255,255,0)';
				v.style.top = '-500px';
				setTimeout(function(){
					cover.parentNode && document.body.removeChild(cover);
					v.parentNode && document.body.removeChild(v);
					changeBtn.parentNode && document.body.removeChild(changeBtn);
				},1100);
			}
		}
	}

	var flashPlayer = function(){
		var flash = document.getElementById('playerObject');
		var flashOut = flash.parentNode;
		return {
			add : function(){
				flashOut.appendChild(flash);
			},
			remove : function(){
				flashOut.parentNode && flashOut.removeChild(flash);
			}
		}
	}	
	
	window.isTudouHTML5PlayerBookMarkCodeByZythum = window.isTudouHTML5PlayerBookMarkCodeByZythum || {};
	var y = window.isTudouHTML5PlayerBookMarkCodeByZythum;
	y.HTML5  = y.HTML5 || HTML5Player();
	y.flash = y.flash || flashPlayer();
	y.flag = y.flag || false;
	if(y.flag === false){
		y.HTML5.add();
		y.flash.remove();
		y.flag = true;
	}else if(y.flag === true){
		y.HTML5.remove();
		y.flash.add();
		y.flag = false;
	}

})();

//sohu
(function(){
	if(!window.vid){
		return false;
	}
	var coverCss = [
		'',
		'position:fixed',
		'top:0',
		'left:0',
		'bottom:0',
		'right:0',
		'background-color:rgba(255,255,255,0)',
		'z-index:999999999999',
		'-webkit-transition:background-color 0.2s ease;',
		'-moz-transition:background-color 0.2s ease;',
		'-o-transition:background-color 0.2s ease;',
		'transition:background-color 0.2s ease;',
		'pointer-events:none',
		''
	];
	var videoCss = [
		'',
		'position:absolute',
		'width:610px',
		'height:500px',
		'top:-500px',
		'left:50%',
		'margin-left:-306px',
		'z-index:1000000000000',
		'background:#000',
		'box-shadow:0 0 5px #333',
		'-webkit-transition:top 1s ease;',
		'-moz-transition:top 1s ease;',
		'-o-transition:top 1s ease;',
		'transition:top 1s ease;',
		''
	];
	var aCss = [
		'',
		'position:absolute',
		'bottom:0',
		'left:0',
		'right:0',
		'height:30px;',
		'text-align:center',
		'font-size:14px',
		'pointer-events:auto',
		''
	];
	
	var btnCss = [
		'',
		'position:absolute',
		'top:0',
		'height:30px;',
		'line-height:30px',
		'width:60px',
		'text-align:center',
		'font-size:14px',
		'letter-spacing:-1px',
		'color:#014CCC',
		'cursor:pointer',
		'pointer-events:auto',
		''
	];
	
	var changeBtnCss = [
		'',
		'position:absolute',
		'width:500px',
		'height:40px',
		'top:500px',
		'left:50%',
		'margin-left:-250px',
		'z-index:1000000000000',
		'background:#000',
		'box-shadow:0 0 5px #333',
		'text-align:center',
		'color:#eee',
		'border-radius:150px',
		'overflow:hidden',
		''
	];
	
	var changeBtnItemCss = [
		'',
		'display:inline-block',
		'width:150px',
		'height:40px',
		'line-height:40px',
		'font-size:20px',		
		'cursor:pointer',
		''
	];
	
	var HTML5Player = function(){

		var m3u8 = vid || '';
		m3u8 = m3u8 ? 'http://hot.vrs.sohu.com/ipad'+m3u8+'.m3u8' : 0;
		var cover = document.createElement('div');
		cover.style.cssText += coverCss.join(';');

		var v = document.createElement('video');
		v.setAttribute('height','500');
		v.setAttribute('width','610');
		v.setAttribute('controls','true');
		v.setAttribute('autoplay','true');
		v.style.cssText += videoCss.join(';');
		v.src = m3u8;
		
		
		var a = document.createElement('span');		
		a.innerHTML = '目前只有m3u8格式的支持,貌似只有safair才能播放。';
		a.style.cssText += aCss.join(';');
		cover.appendChild(a);
		
		var off = document.createElement('div');
		off.innerHTML = '&#x9000;&#x51FA;';
		off.style.cssText += btnCss.join(';')+';right:0;';
		cover.appendChild(off);
		var nocover = document.createElement('div');
		nocover.innerHTML = '&#x6253;&#x5F00;&#x8FF7;&#x96FE;';
		nocover.style.cssText += btnCss.join(';')+';right:60px;';
		cover.appendChild(nocover);
		
		off.addEventListener('click',function(){
			y.HTML5.remove();
			y.flash.add();
			y.flag = false;
		},false);
		
		nocover.addEventListener('click',function(){
			cover.style.backgroundColor = 'rgba(255,255,255,0)';
		},false);
		
		var changeBtn = document.createElement('div');
		changeBtn.style.cssText += changeBtnCss.join(';');
		return {
			add : function(){
				document.body.appendChild(cover);
				// document.body.appendChild(changeBtn);
				document.body.appendChild(v);
				
				v.addEventListener('canplay',v.play);
				setTimeout(function(){
					cover.style.backgroundColor = 'rgba(255,255,255,0.6)';
					v.style.top = '-1px';
				},100);
			},
			remove : function(){
				v.pause();
				cover.style.backgroundColor = 'rgba(255,255,255,0)';
				v.style.top = '-500px';
				setTimeout(function(){
					cover.parentNode && document.body.removeChild(cover);
					v.parentNode && document.body.removeChild(v);
					changeBtn.parentNode && document.body.removeChild(changeBtn);
				},1100);
			}
		}
	}

	var flashPlayer = function(){
		var flashOut = document.getElementById('sohuplayer');
		var flash = document.getElementById('player');
		return {
			add : function(){
				flashOut.appendChild(flash);
			},
			remove : function(){
				flashOut.parentNode && flashOut.removeChild(flash);
			}
		}
	}	
	
	window.isSohuHTML5PlayerBookMarkCodeByZythum = window.isSohuHTML5PlayerBookMarkCodeByZythum || {};
	var y = window.isSohuHTML5PlayerBookMarkCodeByZythum;
	y.HTML5  = y.HTML5 || HTML5Player();
	y.flash = y.flash || flashPlayer();
	y.flag = y.flag || false;
	if(y.flag === false){
		y.HTML5.add();
		y.flash.remove();
		y.flag = true;
	}else if(y.flag === true){
		y.HTML5.remove();
		y.flash.add();
		y.flag = false;
	}

})();

//iqiyi
(function(){
	if(!/iqiyi\.com/.test(window.location.href)){
		return false;
	}
	if(!window.info){
		return false;
	}
	if(!window.info.videoId){
		return false;
	}

	var coverCss = [
		'',
		'position:fixed',
		'top:0',
		'left:0',
		'bottom:0',
		'right:0',
		'background-color:rgba(255,255,255,0)',
		'z-index:999999999999',
		'-webkit-transition:background-color 0.2s ease;',
		'-moz-transition:background-color 0.2s ease;',
		'-o-transition:background-color 0.2s ease;',
		'transition:background-color 0.2s ease;',
		'pointer-events:none',
		''
	];
	var videoCss = [
		'',
		'position:absolute',
		'width:610px',
		'height:500px',
		'top:-500px',
		'left:50%',
		'margin-left:-306px',
		'z-index:1000000000000',
		'background:#000',
		'box-shadow:0 0 5px #333',
		'-webkit-transition:top 1s ease;',
		'-moz-transition:top 1s ease;',
		'-o-transition:top 1s ease;',
		'transition:top 1s ease;',
		''
	];
	var aCss = [
		'',
		'position:absolute',
		'bottom:0',
		'left:0',
		'right:0',
		'height:30px;',
		'text-align:center',
		'font-size:14px',
		'pointer-events:auto',
		''
	];
	
	var btnCss = [
		'',
		'position:absolute',
		'top:0',
		'height:30px;',
		'line-height:30px',
		'width:60px',
		'text-align:center',
		'font-size:14px',
		'letter-spacing:-1px',
		'color:#014CCC',
		'cursor:pointer',
		'pointer-events:auto',
		''
	];
	
	var changeBtnCss = [
		'',
		'position:absolute',
		'width:500px',
		'height:40px',
		'top:500px',
		'left:50%',
		'margin-left:-250px',
		'z-index:1000000000000',
		'background:#000',
		'box-shadow:0 0 5px #333',
		'text-align:center',
		'color:#eee',
		'border-radius:150px',
		'overflow:hidden',
		''
	];
	
	var changeBtnItemCss = [
		'',
		'display:inline-block',
		'width:150px',
		'height:40px',
		'line-height:40px',
		'font-size:20px',		
		'cursor:pointer',
		''
	];
	
	var HTML5Player = function(){
		var vid = window.info.videoId;
		var cover = document.createElement('div');
		cover.style.cssText += coverCss.join(';');

		var v = document.createElement('video');
		v.setAttribute('height','500');
		v.setAttribute('width','610');
		v.setAttribute('controls','true');
		v.setAttribute('autoplay','true');
		v.style.cssText += videoCss.join(';');
		
		var scr = document.createElement('script');
		scr.src = 'http://cache.video.qiyi.com/m/201971/'+vid+'/';
		document.body.appendChild(scr);
		var timer;
		timer = setInterval(function(){
			if(window.ipadUrl){
				clearInterval(timer);
				v.src = ipadUrl.data.url;
			}
		},100);
		
		var a = document.createElement('span');		
		a.innerHTML = '目前只有m3u8格式的支持,貌似只有safair才能播放。';
		a.style.cssText += aCss.join(';');
		cover.appendChild(a);
		
		var off = document.createElement('div');
		off.innerHTML = '&#x9000;&#x51FA;';
		off.style.cssText += btnCss.join(';')+';right:0;';
		cover.appendChild(off);
		var nocover = document.createElement('div');
		nocover.innerHTML = '&#x6253;&#x5F00;&#x8FF7;&#x96FE;';
		nocover.style.cssText += btnCss.join(';')+';right:60px;';
		cover.appendChild(nocover);
		
		off.addEventListener('click',function(){
			y.HTML5.remove();
			y.flash.add();
			y.flag = false;
		},false);
		
		nocover.addEventListener('click',function(){
			cover.style.backgroundColor = 'rgba(255,255,255,0)';
		},false);
		
		var changeBtn = document.createElement('div');
		changeBtn.style.cssText += changeBtnCss.join(';');
		return {
			add : function(){
				document.body.appendChild(cover);
				document.body.appendChild(v);
				
				v.addEventListener('canplay',v.play);
				setTimeout(function(){
					cover.style.backgroundColor = 'rgba(255,255,255,0.6)';
					v.style.top = '-1px';
				},100);
			},
			remove : function(){
				v.pause();
				cover.style.backgroundColor = 'rgba(255,255,255,0)';
				v.style.top = '-500px';
				setTimeout(function(){
					cover.parentNode && document.body.removeChild(cover);
					v.parentNode && document.body.removeChild(v);
					changeBtn.parentNode && document.body.removeChild(changeBtn);
				},1100);
			}
		}
	}

	var flashPlayer = function(){		
		var flash = document.getElementById('flashbox');
		var flashOut = flash.parentNode;
		return {
			add : function(){
				flashOut.appendChild(flash);
			},
			remove : function(){
				flashOut.parentNode && flashOut.removeChild(flash);
			}
		}
	}	
	
	window.isQiyiHTML5PlayerBookMarkCodeByZythum = window.isQiyiHTML5PlayerBookMarkCodeByZythum || {};
	var y = window.isQiyiHTML5PlayerBookMarkCodeByZythum;
	y.HTML5  = y.HTML5 || HTML5Player();
	y.flash = y.flash || flashPlayer();
	y.flag = y.flag || false;
	if(y.flag === false){
		y.HTML5.add();
		y.flash.remove();
		y.flag = true;
	}else if(y.flag === true){
		y.HTML5.remove();
		y.flash.add();
		y.flag = false;
	}

})();

//升级提示
(function(){
	var msg      = '<b style="font-size:18px">&#x798F;&#x5229;&#xFF1A;</b> &#x7AE5;&#x978B;&#x4EEC;&#x597D;&#xFF0C;&#x6211;&#x662F;&#x4F5C;&#x8005;&#x6731;&#x4E00;&#xFF0C;&#x8C22;&#x8C22;&#x5404;&#x4F4D;&#x5BF9;&#x4E8E;&#x5988;&#x5988;&#x518D;&#x4E5F;&#x4E0D;&#x7528;&#x62C5;&#x5FC3;&#x6211;&#x7684;macbook&#x53D1;&#x70EB;&#x8BA1;&#x5212;&#x7684;&#x4E00;&#x5982;&#x5BC4;&#x671B;&#x7684;&#x652F;&#x6301;&#x3002;&#x6211;&#x77E5;&#x9053;&#x4F60;&#x4EEC;&#x4E0D;&#x662F;&#x652F;&#x6301;&#xFF0C;&#x53EA;&#x662F;&#x6CA1;&#x4E8B;&#x4E00;&#x76F4;&#x7528;&#x7740;&#x5BF9;&#x628A;&#x3002;&#x5728;2013&#x5E74;&#x8FC7;&#x5E74;&#x7684;&#x8FD9;&#x4E2A;&#x6863;&#x53E3;&#xFF0C;&#x8BA1;&#x5212;&#x6709;&#x53D8;&#x52A8;&#x3002;&#x5988;&#x5988;&#x518D;&#x4E5F;&#x4E0D;&#x7528;&#x62C5;&#x5FC3;&#x6211;&#x7684;macbook&#x53D1;&#x70EB;&#x8BA1;&#x5212;&#x6709;&#x65B0;&#x7684;&#x8FDB;&#x5C55;&#x3002;&#x5230;&#x70B9;&#x51FB;&#x540E;&#x9762;&#x7684;&#x94FE;&#x63A5;&#xFF0C;&#x5FEB;&#x70B9;&#x554A;&#xFF01;&#xFF01;<a style="color:blue" target="_blank" href="http://zythum.sinaapp.com/youkuhtml5playerbookmark">http://zythum.sinaapp.com/youkuhtml5playerbookmark</a>'
	var msgblock = document.createElement('div');
	msgblock.style.cssText += 'color:#333;box-shadow:0 0 3px #000;padding:10px;margin:0;position:fixed;left:0;right:0;bottom:0;z-index:1000000000000;background:orange;font-size:14px;';
	msgblock.innerHTML = msg;
	document.body.appendChild(msgblock);
})();
