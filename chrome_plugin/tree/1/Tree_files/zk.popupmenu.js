/**
 * @configbegin
 * @tooltip:the tooltip of the item
 * @icon:the image's path of the item
 * @iconCls:the image class name of the item
 * @text:the caption of the item
 * @configend
 */
zk.MenuItem = function (ops){
	var mni = this;
		
	this.tooltip = ops.tooltip;
	this.icon = ops.icon;
	this.iconCls = ops.iconCls;
	this.caption = ops.text;
	this.topmenu = null;
	this.pmenu = null;
	this.subMenu = null;
	//readonly
	this.enable = true;	
	this.hasSubitemflag = false;
	//readonly
	this.isSeparate = false;
	this.pitem = null;			
	function iniSubmenu(){
		if (!mni.subMenu){
			mni.subMenu = new zk.PopupMenu(); 
			mni.subMenu.isTop = false;
		}
		
		return mni.subMenu;
	};
	this.additem = function(argmenuitem){	
		argmenuitem.topmenu = mni.topmenu;
		iniSubmenu().additem(argmenuitem);
		argmenuitem.pitem = mni;
		//set the text add a flag
		if (mni.hasSubitemflag) {return;}		
		mni.pmenu.addSubflag(mni);
	};	
	this.showSubMenu = function(){		
		if (!mni.subMenu) {			
			return;
		}			
		mni.subMenu.setEnable(mni.enable);	
		mni.pmenu.showSubmenu(mni);			
	} ;
	this.hideSubMenu = function (argIsForce){
		if (!mni.subMenu) {
			return;
		}
		mni.subMenu.hide(argIsForce);
	};
	this.setEnable = function (argEnable){
		if (argEnable == mni.enable){return;}
		mni.enable = argEnable;
		if (mni.subMenu) {mni.subMenu.setEnable(mni.enable);}		
		var pmenu = mni.pmenu;
		if (pmenu){
			var index = pmenu.getindex(mni);		
			pmenu.setitemStyle(pmenu.items[index].value,mni,false);
		}	
	};
	this.removesitem = function (){
		mni.pmenu.removesitem(mni);
	};
	this.remove = function (){
		mni.topmenu.remove(mni.getKeydex());
	};
	this.removeitemAll = function (){
		mni.topmenu.removeitemAll(mni.getKeydex());
	};
	this.seperate = function (){
		mni.subMenu.seperate();
	};
	this.getKeydex = function (){
		return mni.topmenu.getKeydex(mni);
	};
}

/**
 * @configbegin
 * @width:width of the menuitem in this popupmenu
 * @height:height of the menuitem in this popupmenu
 * @maxtextlength:the max length of the menuitem's text,
 * @configend
 */
zk.PopupMenu = function (ops){
	if (!ops) ops = {};
	var MAXTEXTLENGTH = ops.maxtextlength?ops.maxtextlength:24;
	//the style of the table
	var TBSTYLE = "font-size:9pt;border-collapse:collapse;table-layout:fixed;";
	var STYLE_TXTCELL = "height:24;padding-left:5;width:100%;";
	var STYLE_FLAGCELL = "width:12;";
	var STYLE_IMGCELL = "width:26;background-color:buttonface;height:24;text-align:center;";
	var	STYLE_SEPERATE = "height:24;background-color:white;color:gray;";
	
	var	STYLE_SEPERATE_TR = "boder-color:gray;height:1px;background-color:gray;color:gray;";
	var MARGIN = 2;
	var WIDTH = 200;
	var HEIGHT = 24;
	var HIDERATE = 3/4;

	var pom = this;
	var layer;
	var isOut = true;
	//field
	var items = [];
	var allitems = [];	
	//the table
	var itemtable = null;
	var isTop = true;		
	this.owner = null;
	this.allitems = allitems;
	this.items = items;
	this.isTop = isTop;
	this.bgColor = "white";
	//the table's style
	this.itemStyle = "background-color:white;color:black;";
	this.itemHovStyle = "background-color:highlight;color:highlighttext;";
	this.itemDisableStyle = "background-color:white;color:gray;";
	this.itmwidth = ops.width?ops.width:WIDTH;
	this.itmheight = ops.height?ops.height:HEIGHT;
	this.visible = false;
	this.enable = true;
	this.x = null;
	this.y = null;
	this.cancelHide = false;
	
	layer = zk.cr("DIV");
		
	var setValue = function (argObj,argValue){		
		if (argValue!=null) {argObj = argValue;}
	};	
	
	//method
	var setTabStyle = function(){
		setValue(itemtable.style.bgColor,pom.bgColor);		 
		itemtable.cellPadding = "0";
		itemtable.cellSpaceing = "0";		
		itemtable.style.cssText = TBSTYLE;
		if (!(zk.browser.chrome || zk.browser.safari )){
			itemtable.style.border='1px solid black';			
		}
		itemtable.style.width =	pom.itmwidth + MARGIN;
		itemtable.style.height = pom.itmheight*pom.countItems() + MARGIN;
	};
	
	var setitemStyle = function (newrow,argItem,argIshover){		
		var imgcell = newrow.cells[0];
		var txtcell = newrow.cells[1];
		var flagcell = newrow.cells[2];
		
		if (argItem.isSeparate){
			zk.setStyle(newrow,STYLE_SEPERATE_TR);
			zk.setStyle(txtcell,STYLE_SEPERATE_TR);
			
			return;
		}	
		imgcell.style.cssText = STYLE_IMGCELL;
		txtcell.style.cssText = STYLE_TXTCELL;
		flagcell.style.cssText = STYLE_FLAGCELL;
		
		if (!argItem.enable){
			newrow.style.cssText = pom.itemDisableStyle;
		}else if (!argIshover){
			newrow.style.cssText = pom.itemStyle;		
		}else{
			newrow.style.cssText = pom.itemHovStyle;
		}
	};
	
	var renderItem = function (argRow,argItem){
		var txt;

		argRow.insertCell(-1);
		argRow.insertCell(-1);
		argRow.insertCell(-1);
		var imgcell = argRow.cells[0];
		var txtcell = argRow.cells[1];		
		
		if (argItem.iconCls){
			var img = zk.cr("div");
			img.innerHTML='&nbsp;';
			zk.setCss(img,argItem.iconCls);
			imgcell.appendChild(img);
		}else{
			if (argItem.icon){			
				var img = zk.cr("IMG");
				img.src = argItem.icon;
				imgcell.appendChild(img);				
			}else{
				imgcell.innerHTML = "&nbsp;";
			}
		}
		
		if (!argItem.isSeparate) {
			if (argItem.tooltip){
				argRow.title = argItem.tooltip;
			}else{
				argRow.title = argItem.caption;
			}	
			txt = argItem.caption.toString();
			if (txt && txt.split(STRING_EMPTY).length>MAXTEXTLENGTH) {
				txt = txt.substring(0,MAXTEXTLENGTH-2)+"..."
			}		
			txtcell.innerHTML = txt;
		}else{
			txtcell.colSpan = 2;
			imgcell.innerHTML = txtcell.innerHTML = window.STRING_EMPTY;
		}		
		setitemStyle(argRow,argItem,false);		
	};
	
	var hideOtherSubmenu = function (argMenuItem){
		var count = pom.getCount();
		var index = pom.getindex(argMenuItem);
		for (var i = 0 ; i < count ; i++){
			if (!items[i]||i == index) {continue;}
			items[i].key.hideSubMenu();
		}
	};
		
	var m_click = function (argRow,argItem){
		if (argItem.enable) {			
			zk.fire(argItem,'click');
			zk.fire(argItem.topmenu,'click',{item:argItem});
		}		
//		pom.hide(true);
		argItem.topmenu.hide(true);
	};
	
	var m_over = function (argRow,argItem){		
		setitemStyle(argRow,argItem,true);
		if (argItem.enable){
			zk.fire(argItem,'show');
			hideOtherSubmenu(argItem);
			argItem.showSubMenu();
		}
	};
	
	var m_out = function (argRow,argItem){
		setitemStyle(argRow,argItem,false);		
	};
	
	var lay_onover = function (){
		isOut = false;
	};
	
	var lay_onout = function (){
		isOut = true;
	};
	
	var addhideedobj = function (){
		
	};
		
	this.getWidth = function(){
		return layer.offsetWidth;
	};
	
	this.getHeight = function(){
		return layer.offsetHeight;
	};
	
	/**
		count the not separate item in the menu
	*/
	this.countItems = function(){		
		var c=0;
		var size = items.length;
		for(var i=0;i<size;i++){
			var item=items[i].key;
			if (item.isSeparate) continue;
			c++;
		}
		return c;
	};
		
	this.adjustposition = function (x,y){
		if (!x || !y){
			x = pom.x;
			y = pom.y;
		}
		if (!pom.visible){
			return;
		}
		var xy = zk.adjustPosition(x,y,pom.getWidth(),pom.getHeight());
		var l,t;
		
		l = xy.x;
		t = xy.y;
		pom.x = l;
		pom.y = t;
		layer.style.left = l;
		layer.style.top = t;
		//backup the hided select 
//		zk.showDivOverElement(layer);
//		zk.hideDivOverElement(null,layer);		
	};
	
	layer.onmouseout = lay_onout;
	layer.onmouseover = lay_onover;
	
	itemtable = zk.cr("TABLE");	
	layer.appendChild(itemtable);		 
	
	this.setitemStyle = setitemStyle;	
	
	/**
	 * @configbegin
	 * @x:the left point of the popup
	 * @y:the top point of the popup
	 * @event:the event object,if x and y is not specified,event is required
	 * @configend
	 */
	this.show = function (ops){		
		setTabStyle();
		
		w=ops.width;
		h=ops.height;
		l=ops.x;
		t=ops.y;
				
		if (l==null) {l = ops.event.clientX + document.body.scrollLeft;}
		if (t==null) {t = ops.event.clientY + document.body.scrollTop;}
		//adjust
		var xy = zk.adjustPosition(l,t,w,h);
		l = xy.x;
		t = xy.y;
		pom.x = l;
		pom.y = t;
		l = l + "px";
		t = t + "px";	
		
		//for chrome,the right boder of the table is unvisible,but border of layer is visible
		if (zk.browser.chrome || zk.browser.safari){
			zk.setStyle(layer,'border:1px solid black');
		}		
		layer.style.background='buttonface';
		layer.style.fontSize='12px';			
		layer.style.zIndex = "98";
		setValue(layer.style.width,w);
		setValue(layer.style.height,h);
		layer.style.position = "absolute";
		layer.style.display = "inline";		
		layer.style.left = l;
		layer.style.top = t;
		layer.oncontextmenu = function (){return false;}
		document.body.appendChild(layer);
		//backup the hided select 
//		zk.showDivOverElement(layer);
//		zk.hideDivOverElement(null,layer);	
		
		pom.visible = true;
	};	

	//private
	this.showSubmenu = function (argMenuitem){
		var px,py;
		var index,scount=0;
		var smenu;

		smenu = argMenuitem.subMenu;
		if (!smenu){
			return;
		}
		index = pom.getindex(argMenuitem);		
		if (-1 == index) {
			throw new Error("the item is not the subitem of the menu!");
		}
		px = zk.getPixLeft(items[index].value) + WIDTH;
		py = zk.getPixTop(items[index].value);
		//adjust sunmenu
		var cw;
		var ch;
		var ah;
		
		cw = document.body.clientWidth;
		ch = document.body.clientHeight;
		ah = HEIGHT*smenu.countItems();
		//left
		if (px + WIDTH*HIDERATE > cw) {
			px = px - WIDTH*2; 
		}
		//top		
		if (py + ah > ch){
			py = py - ah + HEIGHT;
		}		
		smenu.show({x:px,y:py});	
	};
	
	/**
		add a has subitem flag to the show text
		@param argMenuitem the need refreshed menuitem
	*/
	this.addSubflag = function (argMenuitem){
		if (argMenuitem.hasSubitemflag){
			return;
		}
		var index = pom.getindex(argMenuitem);
		if (-1 == index){
			throw "the item is not the subitem of the menu!";
		}
		var ar = zk.cr('div'); 
		zk.addCss(ar, "ui-icon ui-icon-triangle-1-e");
		ar.innerHTML = "&nbsp;";
		items[index].value.cells[2].appendChild(ar);
		argMenuitem.hasSubitemflag = true;
	};	
	
	/**
		contain the separate item in the menu,not contain the sub items of the submenu
		@return the count of the items in the menu 		 
	*/
	this.getCount = function (){
		return items.length;
	};
	
	/**
		get the index of the menuitem 
		@param argItem the item
		@return if not found return -1  
  	*/
	this.getindex = function (argItem){
		var index = -1;
		var count = pom.getCount();
		for (var i = 0;i < count;i++){
			if (items[i] && items[i].key == argItem) {
				index = i;
			}
		}
		
		return index;
	};
	
	/**
		return the menu item of by the index
		this is not directly the item of the items
	*/	
	this.getitem = function (argIndex){
		var item = items[argIndex];
		
		if (item) {
			return item.key;
		}		
	};
	
	this.getKeydex = function (argItem){
		if (!pom.isTop){
			throw "this menu is not the top menu.";
		}
		return allitems.indexOf(argItem);
	};
	
	this.getItemByKeydex = function(argKeydex){
		return allitems[argKeydex];
	};	
	
	/**
		add MenuItem to the menu
		@param argItem the MenuItem of the menu 
 	*/
	this.additem = function (argItem){	
		var newrow = itemtable.insertRow(-1);	
		var newitem = {key:argItem,value:newrow};		
		items[items.length] = newitem;
		argItem.pmenu = pom;
		renderItem(newrow,argItem);	
		//see the allitems only store the menuitem while the items store the 
		//menuitem and a tr row else
		//for separate ,it's only need to added to the parent menu
		var tm;
		if (argItem.isSeparate){
			return;
		}		
				
		if (pom.isTop) {
			argItem.topmenu = pom;
			allitems[allitems.length] = argItem;
		}else{
			tm = argItem.topmenu;						
			tm.allitems[tm.allitems.length] = argItem;
		}			
		newrow.onclick = function (){
			m_click(newrow,argItem);
		};
		newrow.onmouseover = function (){
			m_over(newrow,argItem);
		};
		newrow.onmouseout = function (){
			m_out(newrow,argItem);
		};
	
	};
	
	/**
	
	*/
	this.removesitem = function (mitem){
		var pmenu = mitem.pmenu;
		var index = pmenu.getindex(mitem); 
		var item = pmenu.items[index];

		//current		
		//dom		
		zk.remove(item.value);
		//items		
		pmenu.items.removei(index);
	};
	
	/**
		top menu method
	*/
	this.removeitemAll = function (argKeydex){
		if (!isTop){
			throw new Error("this menu is not the top menu.");
		}
		if (null != argKeydex){
			var mitem = pom.getItemByKeydex(argKeydex);
			var submenu;
			var keydex;
			if (!mitem){
				return;
			}
			submenu = mitem.subMenu;
		}else{
			submenu = pom;
		}
		
		if (!submenu){
			return;
		}	
		var count = submenu.getCount();
		for (var i = 0 ; i < count;){
			var item = submenu.items[i];
			if (item){
				var mit = item.key;
				if (mit.isSeparate){
					mit.removesitem();
				}else{
					pom.remove(pom.getKeydex(mit));
				}	
			}else{
				i++;
			}						
		}		
	};
	
	/**
		top menu method
	*/
	this.remove = function (argKeydex){
		if (!isTop){
			throw new Error("this menu is not the top menu.");
		}
		if (-1 == argKeydex || allitems.length <= argKeydex){			
			throw new Error("index " + argKeydex + " out of range");
		}
		var mitem = pom.getItemByKeydex(argKeydex);
		if (!mitem){
			return;
		}		

		//submenu
		pom.removeitemAll(argKeydex);
		mitem.removesitem();		
		//allitems		
		allitems.removei(argKeydex);
		mitem = null;		
		//set table style
		setTabStyle();			
	};
	
	/**
	
	*/
	this.seperate = function (){
		var item = new zk.MenuItem(window.STRING_EMPTY);		
		item.isSeparate = true;		
		item.setEnable(false);
		pom.additem(item);
		
		return item;
	};
		
	this.getCount = function(){
		return items.length;
	};
	
	/**
		set the enable of the menu
	*/
	this.setEnable = function (argEnable){
		if (argEnable == pom.enable) {
			return;
		}
		pom.enable = argEnable;
		var count = pom.getCount();
		for (var i = 0 ; i < count ; i++){
			if (!items[i]) {continue;}
			items[i].key.setEnable(argEnable);
			setitemStyle(items[i].value,items[i].key,false);
		}
		//sub style		
	};
	
	/**
		@param argIsForce is forced to hide 
 	*/
	this.hide = function (argIsForce){		
		var count 
				
//		alert(event.srcElement.innerHTML + "|" + pom.cancelHide + "|" + event.srcElement.tagName);
		//for MF to shield to fire the event onclick when oncontextmenu
		if (pom.cancelHide || !isOut && (argIsForce != true)) {
			pom.cancelHide = false;
					
			return;
		}
		layer.style.display = "none";		
		//backup the hided select 
		//zk.showDivOverElement(layer);
		
		count = pom.getCount();
		for (var i = 0;i < count;i++){
			if (items[i]) {
				//need backup status
				setitemStyle(items[i].value,items[i].key,false);
				//hide submenu
				items[i].key.hideSubMenu(argIsForce);
			}
		}		
		pom.visible = false;
	};
	
	zk.on(document,"mousedown",pom.hide);
}