/**
 * the treenode
 * 
 * @configbegin
 * @tree the tree of the node stand in
 * @text caption of the node show on the
 * @key a unique key in the whole tree for this node,and can find this node by
 *      the key
 * @icon the image of this node
 * @expandStyle "always" or "auto"(default),it is useful set to "always" when
 *              this is a lazy-loading node(such as ajax-loading)
 * @type zk.Tree.CHECKBOX or zk.Tree.RADIOBOX
 * @configend
 * @require [base] Tree.js
 */

zk.TreeNode = function(ops) {
	var tree = ops.tree;
	this.tree = tree;

	var node = this;
	// keydex is the index and key of every node,but is not really the index
	node.keydex = null;
	// nbody is the nodebody
	var nbody = zk.cr("span");
	var container = zk.cr("span");
	zk.inade(container, "afterBegin", nbody);
	node.body = nbody;
	node.container = container;

	var STYLE_NODE = "color:#000000;";

	node.first = node.last = node.next = node.prev = null;
	node.caption = ops.text;
	node.tier = null;
	node.parent = null;
	node.hasChild = false;
	node.labstyle = null;
	node.type = ops.type;
	// joker:node.expandStyle:alwalys|auto
	node.expandStyle = ops.expandStyle != null ? ops.expandStyle : "auto";
	node.exIcon = null;
	var icon = new Image();
	icon.align = "absmiddle";
	node.label = null;
	node.icon = icon;
	node.tree = tree;
	node.lineIcon = [];
	node.type = ops.type ? ops.type : zk.Tree.NORMAL;
	node.children = [];
	node.key = ops.key;

	var depth;
	depth = tree.depth;
	node.label = zk.cr("span");
	zk.setStyle(container, "margin:0px;padding:0px;white-space:nowrap;");

	zk.inade(nbody, "beforeEnd", node.label);
	zk
			.setStyle(nbody,
					'margin:0px;cursor:default;text-align:left;color:black;font-size:9pt;');

	if (ops.iconCls) {
		icon.src = zk.Tree.S;
		zk.setCss(icon, ops.iconCls);
	} else {
		if (ops.icon) {
			icon.src = ops.icon;
		} else {
			icon.src = zk.Tree.S;
			zk.setCss(icon, 'zk_tree_icon_default');
		}
	}

	zk.setStyle(node.label, tree.labelStyle);

	zk.inade(nbody, "afterBegin", icon)

	icon.tabIndex = node.label.tabIndex = 1;
	icon.hideFocus = true;

	// Joker:all add param node
	node.select = function() {
		if (!node.isEnable()){
			return;
		}
		tree.doSelect(node);
	};
	node.enable = function(enable){
		node.disabled = false === enable;
	};
	node.isEnable= function(){
		return !node.disabled;
	};
	node.isVisible = function() {
		var str;

		str = node.container.style.display;
		return str && str != "none";
	};

	zk.on(node.label, 'click', node.select);
	zk.on(node.icon, 'click', node.select);
	nbody.oncontextmenu = function(){
		var e;
		if (zk.browser.msie){
			e=window.event;
		}else{
			e=arguments[0];
		}
		if (tree.oncontextmenu) return tree.oncontextmenu(e,node);		
	};

	node.addExIcon = function() {
		if (!node.exIcon) {
			node.exIcon = new Image();
			node.exIcon.align = "absmiddle";
			node.exIcon.src = zk.Tree.S;
			zk.on(node.exIcon, 'click', function() {
						node.expand();
					});
			var o = node.icon.src == '' ? node.label : node.icon;
			zk.inade(o, "beforeBegin", node.exIcon);
		}
	}

	// delete all the child of the node
	node.removeChildren = function() {
		var count = node.getChildrenCount();

		for (var i = 0; node.children[i];) {
			tree.removeNode(node.children[i], true);
		}
	};

	/*
	 * node.setExpandStyle argStyle:["auto"|"always"]:node's expand styles
	 */
	node.setExpandStyle = function(argStyle) {
		if (null == argStyle) {
			argStyle = node.expandStyle;
		}
		if (argStyle == "always") {
			node.addExIcon();
			if (!node.hasChild && node.expanded) {
				node.expanded = false;
			}
		} else if (argStyle == "auto" && node.expandStyle != "auto") {
			if (!node.hasChild && node.exIcon != null && !tree.isLineStyle()) {
				node.body.removeChild(node.exIcon);
				node.exIcon = null;
			}
		}
		node.expandStyle = argStyle;
		tree.setExIcon(node);
	};

	/**
	 * node.expand:expand this node
	 * 
	 * @param isShow
	 *            [true|false]:is expand or collapse
	 * @param incSub
	 *            [true|false]:is the sub node's child node need do the same
	 *            thing
	 */
	node.expand = function(isShow, incSub) {
		// Joker:
		// if(node.children.length==0)return;
		if (node.children.length == 0 && node.expandStyle != "always")
			return;

		if (isShow == null) {
			node.expanded = !node.expanded;
		} else {
			node.expanded = isShow;
		}// end if
		var sh = node.expanded ? "block" : "none";
		var icount = node.children.length;
		for (var i = 0; i < icount; i++) {
			node.children[i].container.style.display = sh;
			// baseFunc.setSingleStyle(node.children[i].container,"display",sh);
			if (incSub && (!(isShow ^ node.children[i].expanded))) {
				node.children[i].expand(node.expanded, true);
			}
		}
		tree.setExIcon(node);

		// Joker:add node Eventarg to node.onexpand
		if (node.expanded) {
			zk.fire(node, 'expand');
		} else {
			zk.fire(node, 'collapse');
		}
	};

	if (zk.Tree.CHECKBOX == node.type) {
		var chkBox = null;

		chkBox = zk.cr("input");
		chkBox.setAttribute("type", "checkbox");
		chkBox.align = "absmiddle";
		chkBox.style.verticalAlign = 'middle';
		zk.inade(node.label, "beforeBegin", chkBox);
		node.checkBox = chkBox;
		chkBox.checked = node.checked = false;
		// fire oncheck event
		zk.on(chkBox, 'click', function() {
					node.checked = chkBox.checked;
					zk.fire(node, 'click');
				});
	} else if (zk.Tree.RADIOBOX == node.type) {
		node.rdobox = null;
		node.checked = false;

		var rdoBox = zk.crradio(null, false);
		rdoBox.style.verticalAlign = 'middle';
		zk.inade(node.label, "beforeBegin", rdoBox);
		// fire oncheck event
		zk.on(rdoBox, 'click', function() {
					node.setRadioCheck();
				});
		node.rdoBox = rdoBox;
	}

	/**
	 * set the check box checked or not,this method can't be called before this
	 * node is added to the tree
	 * 
	 * @param _argChecked
	 *            check or not check
	 * @param _argsetChild
	 *            if set the children of the node checkd as their parent
	 * @param _argFireEvent
	 *            if fire the oncheck event
	 */
	node.setChecked = function(_argChecked, _argsetChild , _argFireEvent) {
		if (zk.Tree.CHECKBOX != node.type) {
			return;
		}
		if (_argFireEvent){
			
		}
		
		// Joker:care this check
		// is necessary or it will cause infinitude recursion
		if (node.checkBox.checked != _argChecked) {
			node.checked = node.checkBox.checked = _argChecked;
			if (_argFireEvent){
				zk.fire(node, 'click');
			}
		}
		if (_argsetChild) {
			for (var i = 0; i < node.getChildrenCount(); i++) {
				node.children[i].setChecked(_argChecked, true,_argFireEvent);
			}
		}
	};

	/**
	 * set the radio checkbox checked,this method can't be called before this
	 * node is added to the tree
	 * 
	 * @param _argFireEvent
	 *            is fire the event
	 */
	node.setRadioCheck = function() {
		if (zk.Tree.RADIOBOX != node.type) {
			throw new Error("this is not a radiobox node.");
		}

		var p;
		var nd;
		var oNode;

		p = node.parent;

		if (p != null && p != tree.root) {
			oNode = p.checkedRdobox;
			for (var i = 0; i < p.getChildrenCount(); i++) {
				nd = p.children[i];
				if (nd.rdoBox && node != nd) {
					nd.checked = nd.rdoBox.checked = false;
				}
			}
			p.checkedRdobox = node;
		}
		node.checked = node.rdoBox.checked = true;
		// fire
		if (node.parent && node.parent != tree.root) {
			if (oNode != node) {
				zk.fire(node.parent, 'rdocheckchanged', {
							preNode : oNode,
							curNode : node
						});
			}
		}

	};

	this.setCaption = function(argCaption) {
		node.caption = argCaption;
		node.label.innerHTML = argCaption;
	};
	node.setCaption(ops.text);

	node.remove = function() {
		tree.removeNode(node);
	}
	node.getChildrenCount = function() {
		return node.children ? node.children.length : 0
	};
	node.getTier = function() {
		return node.tier;
	}

	return node
}