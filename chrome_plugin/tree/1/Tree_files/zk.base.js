Array.prototype.indexOfKey = function(key) {
	for (var i = 0; i < this.length; i++) {
		if (this[i] && this[i].key == key) {
			return i;
		}
	}

	return -1;
};

Array.prototype.indexOf = function(elm) {
	for (var i = 0; i < this.length; i++)
		if (elm == this[i])
			return i;
	return -1;
}

Array.prototype.erase = function(elm) {
	for (var i = 0; i < this.length; i++)
		if (elm == this[i])
			this[i] = null;
}

Array.prototype.get = function(key) {
	var index = this.indexOfKey(key);

	if (-1 == index)
		return null;

	return this[index];
}

Array.prototype.removei = function(index) {
	if (index < 0 || index >= this.length)
		return;
	for (var i = index; i < this.length; i++)
		this[i] = this[i + 1];
	this.length--;
}

// remove the element in the array
Array.prototype.removeo = function(obj) {
	var bl;

	bl = false;
	for (var i = 0; i < this.length; i++) {
		if (obj === this[i] || bl) {
			this[i] = this[i + 1];
			bl = true;
		}
	}
	if (bl) {
		this.length--;
	}
}

if (window.zk) {
	throw new Error("zk has registered.");
}
(function() {
	window.STRING_EMPTY = "";

	var userAgent = navigator.userAgent.toLowerCase();

	window.zk = {
		browser : {
			version : (userAgent.match(/.+(?:rv|it|ra|ie)[\/: ]([\d.]+)/) || [
					0, '0'])[1],
			safari : /webkit/.test(userAgent),
			opera : /opera/.test(userAgent),
			msie : /msie/.test(userAgent) && !/opera/.test(userAgent),
			mozilla : /mozilla/.test(userAgent)
					&& !/(compatible|webkit)/.test(userAgent),
			chrome : /chrome/.test(userAgent)
		},
		get : function(id) {
			return document.getElementById(id);
		},
		setStyle : function(el, styleText) {
			if (this.browser.msie && this.browser.version < 8) {
				el.style.cssText = styleText;
			} else {
				el.setAttribute("style", styleText);
			}
		},
		addCss : function(el, css) {
			if (!css || '' == css) {
				return;
			}
			var ncss = this._getCsss(el);
			var acsss = css.split(/\s+/);
			for (var i = 0; i < acsss.length; i++) {
				ncss[acsss[i]] = acsss[i];
			}
			var cssName = '';
			for (k in ncss) {
				if ('' == k) {
					continue;
				}
				cssName += ' ' + k;
			}
			this.setCss(el, cssName);
		},
		removeCss : function(el, css) {
			if (!css || '' == css) {
				return;
			}
			var ncss = this._getCsss(el);
			var cssName = '';
			for (k in ncss) {
				if ('' == k || css == k) {
					continue;
				}
				cssName += ' ' + k;
			}
			this.setCss(el, cssName);
		},
		_getCsss : function(el) {
			var ncss = {};
			var ocss = this.getCss(el);
			var oacsss = ocss ? ocss.split(/\s+/) : [];
			for (var i = 0; i < oacsss.length; i++) {
				ncss[oacsss[i]] = oacsss[i];
			}

			return ncss;
		},
		getCss : function(el) {
			if (this.browser.msie && this.browser.version < 8) {
				return el.getAttribute("className");
			} else {
				return el.getAttribute("class");
			}
		},
		setCss : function(el, argCssName) {
			if (this.browser.msie && this.browser.version < 8) {
				el.setAttribute("className", argCssName);
			} else {
				el.setAttribute("class", argCssName);
			}
		},
		clearCss : function(el) {
			this.setStyle('');
			if (this.browser.msie && this.browser.version < 8) {
				el.removeAttribute("className");
			} else {
				el.removeAttribute("class");
			}
		},
		/**
		 * get the left of the element
		 */
		getPixLeft : function(argObj) {
			var pixleft;

			pixleft = 0;
			while (argObj) {
				pixleft += argObj.offsetLeft;
				argObj = argObj.offsetParent;
			};

			return pixleft;
		},
		/**
		 * get the top of the element
		 */
		getPixTop : function(argObj) {
			var pixtop;

			pixtop = 0;
			while (argObj) {
				pixtop += argObj.offsetTop;
				argObj = argObj.offsetParent;
			};

			return pixtop;
		},

		remove : function(argObj) {
			if (argObj.parentNode) {
				argObj.parentNode.removeChild(argObj);
			}
		},

		on : function(elem, type, fn) {
			if (elem.addEventListener)
				elem.addEventListener(type, function(e) {
							fn(e);
						}, false);
			else if (elem.attachEvent)
				elem.attachEvent("on" + type, function() {
							fn(window.event);
						});
		},
		/*
		 * un : function(el, type, fn) { $(el).unbind(type, fn); },
		 */
		oe : function(o, type, fn) {
			if (!o.eves)
				o.eves = [];
			if (!o.eves[type])
				o.eves[type] = [];
			o.eves[type][o.eves[type].length] = fn;
		},
		ue : function(o, type, fn) {
			if (!o.eves)
				return;
			if (!o.eves[type])
				return;
			o.eves[type].erase(fn);
		},
		fire : function(o, type, ops) {
			if (!o.eves)
				return;
			if (!o.eves[type])
				return;
			var e = {
				src : o,
				stop : false
			};
			for (var i = 0; i < o.eves[type].length; i++) {
				if (o.eves[type][i]) {
					o.eves[type][i](e, ops);
					if (e.stop)
						return;
				}
			}
		},
		cr : function(tag) {
			return document.createElement(tag);
		},
		crradio : function(argName, argIsChecked) {
			var radio;

			if (this.browser.msie && this.browser.version < 8) {
				var text = "<input type=\"radio\" name=\"" + argName
						+ "\" value=\"checked\" >";
				radio = document.createElement(text);
			} else {
				radio = document.createElement("input");
				radio.setAttribute("type", "radio");
				radio.setAttribute("name", argName);
				radio.setAttribute("value", "checked");
			}
			radio.checked = argIsChecked;

			return radio;
		},
		log : function(msg) {
			if (!this.console) {
				return;
			}
			this.console.value += msg + '\n';
		},
		ok : function(fn) {
			zk.oe(zk, 'ok', fn);
		},
		isnum : function(argNum) {
			if (0 != argNum && !argNum) {
				return false;
			}

			if ("" == argNum) {
				return false;
			}

			return !isNaN(new Number(argNum));
		},

		inade : function(argObj, argStrsomewhere, argNewObj) {
			var parent;

			switch (argStrsomewhere.toLowerCase()) {
				case "beforebegin" :
					argObj.parentNode.insertBefore(argNewObj, argObj);
					break;
				case "afterbegin" :
					var nn = !argObj.childNodes ? null : argObj.childNodes[0];
					if (!nn) {
						nn = null
					}
					;
					argObj.insertBefore(argNewObj, nn);
					break;
				case "beforeend" :
					argObj.appendChild(argNewObj);
					break;
				case "afterend" :
					var nnode = this.nextnode(argObj);
					argObj.parentNode.insertBefore(argNewObj, nnode);
					break;
			}
		},

		nextnode : function(argObj) {
			var parent, obj;

			obj = null;
			parent = argObj.parentNode;
			for (var i = 0; parent.childNodes[i]; i++) {
				if (parent.childNodes[i] === argObj) {
					try {
						obj = parent.childNodes[i + 1];
					} catch (e) {
						obj = null;
					}
					break;
				}
			}
			return obj;
		},
		/**
		 * adjust the x,y point to prevent from oversteping out the window
		 */
		adjustPosition : function(x, y, width, height) {
			var cw;
			var ch;

			cw = document.body.clientWidth;
			ch = document.body.clientHeight;
			// left
			if (x + width > cw) {
				x = cw - width;
			}
			x = Math.max(0, x);
			// zk.log("x : height:document.body.clientWidth" + x +"|" + width +
			// "|" + cw);
			// zk.log("y : height:document.body.clientHeight" + y +"|" + height
			// + "|" + ch);
			// top
			if (y + height > ch) {
				y = ch - height;
			}
			y = Math.max(0, y);

			return {
				"x" : x,
				"y" : y
			}
		}
	};

	zk.Url = function(argOrginUrl) {
		var _this = this;
		var params = {};
		var baseurl = null;

		this.putParam = function(argKey, argValue) {
			if (argKey && argValue) {
				params[argKey] = argValue;
			}
		};
		this.getParam = function(argKey) {
			return params[argKey];
		};

		this.toString = function() {
			var keys = params.keyset;
			var url = baseurl;
			var qs = "?";
			for (k in params) {
				var value = params[k];
				if (value === 0 || value) {
					url = url + qs + k + "=" + value;
				}
				qs = "&";
			}

			return url;
		};

		if (!argOrginUrl || "" == argOrginUrl) {
			throw new Error("orgin url null.");
		}
		var index = argOrginUrl.indexOf("?");
		if (-1 != index) {
			baseurl = argOrginUrl.substring(0, index);
			if (index + 1 < argOrginUrl.length - 1) {
				var querystring = argOrginUrl.substring(index + 1);
				var kvss = querystring.split("&");
				for (var i = 0; i < kvss.length; i++) {
					var kvs = kvss[i].split("=");
					if (1 >= kvs.length) {
						continue;
					}
					_this.putParam(kvs[0], kvs[1]);
				}
			}
		} else {
			baseurl = argOrginUrl;
		}
	};

	zk.on(window, "load", function() {
				zk.fire(zk, 'ok');
			});
})();