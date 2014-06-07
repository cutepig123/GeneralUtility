/*
@file 推广位跟踪
@author 曹宇 <caoyu#at#csdn.net>
@version 20130902
*/

(function(definition, undef) {
  var $, exports, global, i;
  global = this;
  $ = global.jQuery;
  exports = {};
  definition(global, exports, $);
  if (global.csdn === undef) {
    global.csdn = exports;
  }
  for (i in exports) {
    global[i] = global.csdn[i] = exports[i];
  }
})(function(global, exports, $, undef) {
  var ads, crossdomainGet, domReadyListener, done, findAllAds, linksContent, protocol, ref, s, trackingAd, viewedHeight;
  protocol = global.location.protocol.substr(0, 4) === 'http' ? '' : 'http:';
  ref = global.document.referrer || '-';
  ads = [];
  viewedHeight = 0;
  domReadyListener = function() {
    var ad, groups, k, n, v, w, _i, _len, _ref, _ref1;
    n = 0;
    w = $(global).scroll(function() {
      if (!ads[0]) {
        return;
      }
      global.clearTimeout(n);
      return n = global.setTimeout(function() {
        var ad, top, _i, _len;
        top = w.scrollTop() + w.height();
        if (top > viewedHeight) {
          viewedHeight = top;
          for (_i = 0, _len = ads.length; _i < _len; _i++) {
            ad = ads[_i];
            if (!(!ad.viewed && top >= ad.top)) {
              continue;
            }
            ad.viewed = true;
            crossdomainGet(ad.data());
          }
        }
      }, 100);
    });
    groups = {};
    _ref = $('.tracking-ad');
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      ad = _ref[_i];
      k = $(ad).attr('data-mod');
      ((_ref1 = groups[k]) != null ? _ref1.push(ad) : void 0) || (groups[k] = [ad]);
    }
    $(document).trigger('trackingAd-ready');
    for (k in groups) {
      v = groups[k];
      trackingAd($(v));
    }
    return w.triggerHandler('scroll');
  };
  /*
  跟踪指定的推广位，包括曝光和点击
  @param {CSS Selector/jQuery Object/DOMElement/DOMElement[]} ele 包含推广链接的推广位容器标签
  @param {Object} opts 选项，如下
  @param {String} opts.pid 产品线标识，可选，默认取当前页域名中foo.csdn.net中的foo，不会处理三级域名的情况
  @param {String} opts.mod 模块id，必须
  @param {String} opts.mtp 模块类型，可选，默认会探测是否有带链接的图片，如果有则是3图片，否则2文字
  @param {String/RegExp} opts.trim 对最终名称做进一步裁剪的正则，可选，始终会先裁剪掉两边的空白字符
  @param {CSS Selector} filter 可选，对跟踪的推广链接进一步过滤，这里将包含那些class="foo"但是不包括class="foo bar"的链接
  */

  trackingAd = exports.trackingAd = function(ele, opts) {
    var filter, mod, mtp, pid, trim, _ref;
    if (opts == null) {
      opts = {};
    }
    if (typeof ele === 'string' || !ele instanceof $) {
      ele = $(ele);
    }
    if (!opts.force && ele.data('has-tracking-ad')) {
      return;
    }
    ele.data('has-tracking-ad', true);
    pid = opts.pid || ele.attr('data-pid') || global.document.body.getAttribute('data-pid') || ((_ref = /(\w+)\.\w+\.\w+$/.exec(global.location.host)) != null ? _ref[1] : void 0);
    mod = opts.mod || ele.attr('data-mod');
    mtp = opts.mtp || ele.attr('data-mtp') || ($('a img', ele)[0] ? 3 : 2);
    trim = opts.trim || ele.attr('data-trim');
    if (typeof trim === 'string' && trim.charAt(0) === '/') {
      trim = trim.split('/');
      trim = new RegExp(trim.slice(1, -1), trim.slice(-1));
    }
    filter = opts.filter || ele.attr('data-filter') || '';
    return findAllAds(ele, filter, function(links) {
      var ad, con;
      if (links.length === 0) {
        ele.removeData('has-tracking-ad');
        return;
      }
      con = linksContent(mtp, trim, links);
      ad = {
        top: ele.offset().top,
        ele: ele,
        viewed: false,
        data: function() {
          var _ref1;
          return {
            uid: ((_ref1 = /(; )?(UserName|_javaeye_cookie_id_)=([^;]+)/.exec(global.document.cookie)) != null ? _ref1[3] : void 0) || '-',
            ref: ref,
            pid: pid,
            mod: mod,
            mtp: mtp,
            con: con,
            ck: '-'
          };
        }
      };
      ads.push(ad);
      if (viewedHeight > ad.top) {
        $(global).triggerHandler('scroll');
      }
      links.each(function() {
        if (this.target === '') {
          return this.target = '_blank';
        }
      });
      return links.click(function() {
        var d;
        if (this.href && this.innerHTML.replace(/^\s+|\s+$/g, '')) {
          d = ad.data();
          d.ck = linksContent(mtp, trim, $(this));
          crossdomainGet(d);
        }
      });
    });
  };
  findAllAds = function(cont, filter, callback) {
    var count, iframes, links;
    links = $('a' + filter, cont);
    iframes = $('iframe', cont);
    if (iframes[0]) {
      count = iframes.length;
      return iframes.each(function() {
        var ifr, listener;
        ifr = $(this);
        listener = function() {
          if (ifr === null) {
            return;
          }
          links = links.add(ifr.contents().find('a' + filter));
          ifr = null;
          count--;
          if (count === 0) {
            return callback(links);
          }
        };
        return ifr.load(listener);
      });
    } else {
      return callback(links);
    }
  };
  linksContent = function(mtp, trim, ele) {
    return ele.map(function() {
      var img, text;
      if (mtp === 3) {
        img = $('img', this)[0];
        if (img) {
          text = img.title || img.alt || img.src;
        }
      }
      if (!text) {
        text = this.innerHTML.replace(/<[^>]+>/g, '').replace(/^\s+|\s+$/g, '') || this.title || this.alt || this.innerHTML.replace(/^\s+|\s+$/g, '');
      }
      if (trim) {
        text = (text + '').replace(trim, '');
      }
      return text + ',' + this.href + ',' + ($(this).attr('strategy') || '-');
    }).get().join(';');
  };
  crossdomainGet = function(data) {
    var i;
    if (!data.con) {
      return;
    }
    data[Math.random() * 10000 | 0] = '';
    data['x-acl-token'] = 'status_js_dkuyqthzbajmncbsb_token';
    i = new Image();
    i.onload = i.onerror = function() {
      i.onload = i.onerror = null;
      i.removeAttribute('src');
      return i = null;
    };
    return i.src = protocol + '//dc.csdn.net/re?' + $.param(data);
  };
  if ($ === undef) {
    done = false;
    s = document.createElement('script');
    s.type = 'text/javascript';
    s.charset = 'utf-8';
    s.src = 'http://www.csdn.net/js/jquery-1.4.2.min.js';
    s.onload = s.onreadystatechange = function() {
      if (!done && (!this.readyState || this.readyState === 'loaded' || this.readyState === 'complete')) {
        done = true;
        global.jQuery.noConflict();
        $ = global.jQuery;
        $(domReadyListener);
        return s.onload = s.onreadystatechange = null;
      }
    };
    document.getElementsByTagName('head')[0].insertBefore(s, null);
  } else {
    $(domReadyListener);
  }
});
