/**
 * 加载内容的主要函数
 * @param path 这个path一定要是url中的pathname部分，否则匹配不了模板
 * @param first 是否首次加载，是的话就要用 replaceState
 */
var $ = function ( path , first ) {
    "use strict";
    var name, temp = $.templateMap, template;

    // 循环模板对象
    //    for ( name in temp ) {
    //        if ( temp.hasOwnProperty( name ) && temp[ name ].test( path ) ) {
    //            template = $.template[ name ];
    //        }
    //    }

    // 根据正则匹配模板，一旦匹配上就退出
    temp.some( function ( v ) {
        if ( v.regex.test( path ) ) {
            if ( !v.template ) {

                // 这里使用同步的方式加载模板
                v.template = $.ajax( {
                    url : v.href ,
                    async : false
                } ).responseText;
            }
            template = v.template;
            return true;
        }
    } );

    $.ajax( {

        // 虽然准备通过判断请求头的方式来决定返回什么内容
        // 但我发现请求头的方式会返回缓存的结果，所以还是用带参的方式吧
        url : path + '?ajax=1' ,
        change : function ( e ) {
            var html, action, state, data;
            if ( 4 === this.readyState ) {
                action = first ? 'replaceState' : 'pushState';
                data = JSON.parse( this.responseText );
                switch ( this.status ) {
                    case 404:
                        if ( !$.template.error404.template ) {
                            $.template.error404.template = $.ajax( {
                                url : $.template.error404.href ,
                                async : false
                            } ).responseText;
                        }
                        template = $.template.error404.template;
                        break;
                }

                if ( !template ) {
                    if ( !$.template.blog.template ) {
                        $.template.blog.template = $.ajax( {
                            url : $.template.blog.href ,
                            async : false
                        } ).responseText;
                    }
                    template = $.template.blog.template;
                }

                html = ejs.render( template , data );
                state = {
                    content : html ,
                    title : data.title + ' - 一个简陋的博客'
                };
                document.title = state.title;
                history[ action ]( state , '' , path );
                $.content.innerHTML = html;
            }
        }
    } );
};

/**
 * 一个更高级的用于替换 typeof 运算符的方法
 * 根据 [[Class]] 标识返回类型
 * 由于$.extend依赖于它，所以优先定义
 * @param test
 * @returns {string}
 */
$.type = function ( test ) {
    "use strict";
    return {}.toString.call( test ).slice( 8 , -1 ).toLowerCase();
};

/**
 * 深复制的方法
 * @returns {*}
 */
$.extend = function () {
    'use strict';

    // 第一个参数是目标对象
    var target ,

    // 读取参数个数
        l = arguments.length,

    // 循环用变量
        i ,

    // 这个变量用于临时的保存参数对象
        temp,

    // 临时的键值
        name,

    // 缓存函数
        type = $.type,

    //用于保存临时属性
        attr;

    // 如果参数的个数只有一个
    if ( l === 1 ) {

        // 则把 this 作为被扩展的对象
        target = this;
        i = 0;
    } else {
        target = arguments[0];
        i = 1;
    }

    /*
     * 循环变量
     * 从第二个参数开始循环，排在后面的对象会覆盖前面对象的同名属性
     * 之所以在初始化条件里写一个 i ，是为了不让 JSLint 报错。。。
     * */
    for ( i ; i < l ; i += 1 ) {

        // 临时保存参数
        temp = arguments[i];

        // 每个参数都应该是一个对象，所以使用 for...in 进行枚举
        for ( name in temp ) {

            // 这里没有检测原型链，原因在于 混入 模式
            attr = temp[name];

            // 检测这个属性是否对象
            switch ( type( attr ) ) {

                //如果这个属性是对象或者数组
                case 'object':

                    // 则递归的调用自身把属性给添加进去
                    $.extend( target[name] = {} , attr );
                    break;

                case 'array':
                    $.extend( target[name] = [] , attr );
                    break;

                default :

                    //否则直接复制
                    target[name] = attr;
                    break;
            }
        }
    }
    return target;
};

$.extend( {

    /**
     * 将对象转换为 HTTP请求字符串
     * 例如 传入{ test : '测试' , test2 : 'good job' } 则返回 'test=测试&test2=good job'
     * 若参数为字符串则直接返回参数
     * 多用于 $.ajax 函数
     * @param data
     * @param {boolean=} convertToObj 当第一个参数是字符串时，是否将字符串转换成对象
     * @returns {*}
     */
    encode : function ( data , convertToObj ) {
        'use strict';
        var name, result;

        //如果参数是对象，就转换成字符串；如果是字符串就转换成对象
        switch ( this.type( data ) ) {
            case 'object':
                result = [];

                for ( name in data ) {
                    if ( data.hasOwnProperty( name ) ) {
                        result.push( name + '=' + data[name] );
                    }
                }

                // bugfix：中文不编码导致有道翻译中文时失败
                return encodeURI( result.join( '&' ) );

            case  'string':

                //如果是字符串且第二个参数是true，则返回对象，否则直接返回参数
                if ( convertToObj ) {
                    result = {};

                    data.split( '&' ).forEach( function ( v ) {
                        var kv = v.split( '=' );
                        result[kv[0]] = kv[1];
                    } );
                } else {
                    result = data;
                }

                return result;

            default :
                throw new TypeError( '$.encode()：参数必须是对象或者字符串！' );
        }
    } ,

    /**
     * 封装请求函数
     * @param requestObj
     * @param {String} requestObj.url
     * @param {Object} requestObj.data
     * @param {Object} requestObj.headers
     * @param {String=} requestObj.method
     * @param {bool=} requestObj.async
     * @param {Function=} requestObj.change
     * @param{Function=} requestObj.error
     * @returns {Object}
     */
    ajax : function ( requestObj ) {
        'use strict';
        var x = $.extend( {
            method : 'GET' ,
            async : true ,
            data : ''
        } , requestObj ), r = new XMLHttpRequest(), sender = null, temp, headers = requestObj.headers;

        // 将对象编码
        x.data = this.encode( x.data );

        //        console.log( x.data );

        //GET请求的参数写在 url 后面
        if ( x.method === 'GET' && x.data ) {
            x.url += '?' + x.data;
        }

        r.open( x.method , x.url , x.async );

        if ( x.method === 'POST' ) {
            r.setRequestHeader( "Content-Type" , "application/x-www-form-urlencoded" );
            sender = x.data;
        }

        if ( headers ) {
            for ( temp in headers ) {
                if ( headers.hasOwnProperty( temp ) ) {
                    r.setRequestHeader( temp , headers[ temp] );
                }
            }
        }

        if ( x.async && requestObj.change ) {

            // 不要直接将事件直接设为 callback ，方便外部函数使用 bind 方法自定义回调函数内部的 this
            r.onreadystatechange = requestObj.change;
        }

        if ( requestObj.error ) {
            r.onerror = requestObj.error;
        }

        //如果是 POST 请求，则发送 data
        r.send( sender );
        if ( x.sync ) {
            return this;
        } else {
            return r;
        }
    } ,

    template : {

        // 404页面模板
        'error404' : {
            type : 'error404' ,
            href : '/theme/new/build/ejs/404.ejs'
        } ,

        // 博客内容模板，这是默认的模板
        'blog' : {
            type : 'blog' ,
            href : '/theme/new/build/ejs/blog.ejs'
        }
    } ,

    templateMap : [
        {
            type : 'index_list' ,
            regex : /^\/$/ ,
            href : '/theme/new/build/ejs/index_list.ejs'
        }
    ] ,

    content : document.getElementById( 'content' )
} );

// 如果当前浏览器支持 HTML5 的History API，则使用pajx方式，否则使用传统的同步加载方式
if ( history.replaceState ) {

    // 因为第一次加载时会返回完整内容，所以下面这句不需要了
    //$( location.pathname , true );

    // 但是需要把当前的状态替换一下，否则后退按钮不会改变内容
    history.replaceState( {
        content : $.content.innerHTML ,
        title : document.title
    } , '' );

    // 为每个链接添加一个事件
    document.addEventListener( 'click' , function ( e ) {
        "use strict";
        var t = e.target, path = t.href, origin = location.origin || (location.protocol + '//' + location.host);

        // 卧槽，出错的原因在于 ie 11 & 10 里面没有 location.origin 这个属性
        // 设置了 data-ajax="0" 的链接不要处理
        if ( 'A' === t.nodeName && 0 === path.indexOf( origin ) && '0' !== t.getAttribute( 'data-ajax' ) ) {

            e.preventDefault();
            $( path.replace( origin , '' ) );
        }
    } );

    onpopstate = function ( e ) {
        "use strict";
        var state = e.state;
        //    console.dir( e );
        if ( state ) {
            $.content.innerHTML = state.content;
            document.title = state.title;
        }
    };
}
