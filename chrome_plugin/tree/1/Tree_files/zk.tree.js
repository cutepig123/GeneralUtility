zk.Tree = function(ops) {
	var tree = this;

	var defaultTheme = {
		zk_tree_icon_blank : null,
		zk_tree_icon_leaf_top : null,
		zk_tree_icon_leaf : null,
		zk_tree_icon_twig : null,
		zk_tree_icon_collapse : null,
		zk_tree_icon_expand : null,
		zk_tree_icon_collapse_top : null,
		zk_tree_icon_expand_top : null,
		zk_tree_icon_collapse_mid : null,
		zk_tree_icon_expand_mid : null,
		zk_tree_icon_collapse_end : null,
		zk_tree_icon_expand_end : null
	};

	if (!zk.Tree.SI) {
		zk.Tree.SI = new Image();
		zk.Tree.SI.src = zk.Tree.S
	}
	var COLOR_SELECT_BG = "highlight";

	var labelStyle = ops.labelStyle
			? labelStyle
			: "padding:0;margin-left:2;vertical-align:middle;text-align:left;"
	this.labelStyle = labelStyle;
	tree.depth = 0;
	var count = 0, nodes = [];

	// is show line
	this.showline = ops.showline;

	// checkbox
	var colChkNode = [];
	// radiobox
	var colrdoNode = [];

	// root
	// inner object: root
	var root = {};
	root.children = [];
	root.expanded = true;
	root.getTier = function() {
		return 0;
	}

	tree.selectedNode = null;
	// return the object's index in the array,return -1 if not found

	var isExpandable = function(srcNode) {
		return srcNode.expandStyle == "always" || srcNode.hasChild;
	};// end md

	var isFirstTop = function(srcNode) {
		return srcNode.parent == root && root.children[0] == srcNode;
	};// end md

	var isLeaf = function(srcNode) {
		return srcNode.next != null;
	};// end md

	var isTwig = function(srcNode) {
		return srcNode.next == null;
	};// end md

	// inner function :
	/**
	 * set the icon of the expandable node
	 * 
	 * @param srcNode
	 */
	var setExIcon = function(srcNode) {
		var strImgkey;
		var expanded = srcNode.expanded;

		if (!isExpandable(srcNode)) {
			// not expandable
			if (!tree.showline) {
				// if (srcNode.exIcon) {
				// zk.remove(srcNode.exIcon);
				// return;
				// }
				strImgkey = 'zk_tree_icon_blank';
			} else {
				if (isFirstTop(srcNode)) {
					// is first top,this is special
					if (isTwig(srcNode)) {
						// has no sister
						if (srcNode.exIcon) {
							strImgkey = 'zk_tree_icon_blank';
						}
					} else {
						strImgkey = 'zk_tree_icon_leaf_top';
					}
				} else if (isLeaf(srcNode)) {
					strImgkey = 'zk_tree_icon_leaf';
				} else {
					strImgkey = 'zk_tree_icon_twig';
				}
			}
		} else {
			// need show expand
			if (!tree.showline) {
				strImgkey = expanded?'zk_tree_icon_collapse_noline':'zk_tree_icon_expand_noline';
			} else {
				if (isFirstTop(srcNode)) {
					if (isTwig(srcNode)) {
						strImgkey = expanded
								? "zk_tree_icon_collapse"
								: "zk_tree_icon_expand";
					} else {
						strImgkey = expanded
								? "zk_tree_icon_collapse_top"
								: "zk_tree_icon_expand_top";
					}
				} else if (isLeaf(srcNode)) {
					strImgkey = expanded
							? "zk_tree_icon_collapse_mid"
							: "zk_tree_icon_expand_mid";
				} else {
					strImgkey = expanded
							? "zk_tree_icon_collapse_end"
							: "zk_tree_icon_expand_end";
				}
			}
		}
		// if (tree.showline && !srcNode.exIcon) {
		// srcNode.addExIcon();
		// }
		if (!srcNode.exIcon) {
			srcNode.addExIcon();
		}
		if (srcNode.exIcon) {
			srcNode.icon.style.marginLeft = "0";
			zk.setCss(srcNode.exIcon, strImgkey);
		} else {
			if (!tree.showline) {
				srcNode.icon.style.marginLeft = zk.Tree.SI.width;
			}
		}

		return;
	}
	this.setExIcon = setExIcon;

	/*
	 * setLine :set the "|" line icon
	 */
	var setLine = function(srcNode, idx) {
		// if (!tree.showline)
		// return;

		if (srcNode.hasChild) {
			for (var i = 0; i < srcNode.children.length; i++) {
				zk.setCss(srcNode.children[i].lineIcon[idx], !tree.showline
								? 'zk_tree_icon_blank'
								: 'zk_tree_icon_line');
				setLine(srcNode.children[i], idx);
			}
		}
	}
	this.setLine = setLine;

	var doSelect = function(srcNode) {
		// reset previous selected node
		if (tree.selectedNode != null) {
			tree.selectedNode.label.style.background = tree.selectedNode.label._background;
			tree.selectedNode.label.style.color = tree.selectedNode.label._color;
		}
		srcNode.label._background = srcNode.label.style.background
				? srcNode.label.style.background
				: 'none';
		srcNode.label._color = srcNode.label.style.color
				? srcNode.label.style.color
				: 'black';
		srcNode.label.style.background = COLOR_SELECT_BG;
		srcNode.label.style.color = "highlighttext";

		tree.selectedNode = srcNode;
		zk.fire(tree, 'select');
	}
	this.doSelect = doSelect;

	this.enable = function(enable) {
		var enable = false !== enable;

		for (var i = 0; i < count; i++) {
			nodes[i].enable(enable);
		}
	};

	var addNode = function(toNode, node) {
		node.tier = toNode.getTier() + 1;
		toNode.children[toNode.children.length] = node;
		var o = toNode == root ? tree.body : toNode.container;
		zk.inade(o, "beforeEnd", node.container);
		node.parent = toNode;
		// set the brother:can optimize
		if (toNode.hasChild) {
			node.prev = toNode.last;
			toNode.last.next = node;
			toNode.last = node;
		} else {
			toNode.first = toNode.last = node;
		}
		// can opt
		node.keydex = toNode.getChildrenCount();

		node.parent.hasChild = true;
		if (tree.depth < node.tier)
			tree.depth = node.tier;
		node.container.style.display = node.parent.expanded ? "block" : "none";
		var indent = ops.indent ? ops.indent : 16;
		// if (!tree.showline) {
		// node.body.style.textIndent = indent * (node.tier - 1) + "px";
		// }
		// is the branch icon show
		// if (tree.showline) {
		// node.addExIcon();
		// }
		// set the line
		if (true) {
			// if (tree.showline) {
			for (var i = node.tier - 2; i >= 0; i--) {
				var img = new Image();
				img.align = "absmiddle";
				img.src = zk.Tree.S;
				zk.inade(node.body, "afterBegin", img);
				node.lineIcon[i] = img;
			}
			if (node.prev != null) {
				tree.setLine(node.prev, node.tier - 1);
			}
			var n = node.parent;
			var i = node.tier - 2;
			while (n != root && i >= 0) {
				if (n.next != null) {
					zk.setCss(node.lineIcon[i], !tree.showline
									? 'zk_tree_icon_blank'
									: 'zk_tree_icon_line');
					// node.lineIcon[i].src=tree.icons["line"].src;
				}
				n = n.parent;
				i--;
			}
		}
		// Joker:add expand image with the style judgement
		var ma = node.parent;
		if (ma != root) {
			if (ma.exIcon == null) {
				// note:the real expand icon is set when add child
				ma.exIcon = new Image();
				ma.exIcon.src = zk.Tree.S;
				ma.exIcon.align = "absmiddle";
				zk.on(ma.exIcon, 'click', function() {
							ma.expand();
						});
				var o = ma.icon.src == "" ? ma.label : ma.icon;
				zk.inade(o, "beforeBegin", ma.exIcon);
			}

			// Joker:
			tree.setExIcon(ma);
		}
		node.expanded = true;
		// set the left margin
		if (!isExpandable(node)) {
			node.icon.style.marginLeft = zk.Tree.SI.width;
		}
		node.expanded = true;
		node.setExpandStyle();
		if (node.prev && tree.showline) {
			tree.setExIcon(node.prev);
		}
		if (node.key && node.key != '') {
			tree.nodes[node.key] = node;
		}
		tree.nodes[count] = node;
		count++;

		return node;
	}
	this.addNode = addNode;

	// remove
	var remove = function(srcNode) {
		zk.remove(srcNode.container);
		nodes.removeo(srcNode);
	}

	/**
	 * remove all the nodes of the tree
	 */
	this.removeAll = function() {
		var count;
		var nds;

		nds = tree.root.children;
		count = nds.length;
		for (var i = 0; i < count;) {
			if (nds[i]) {
				tree.removeNode(nds[i], true);
			} else {
				i++;
			}
		}
	};

	/**
	 * @param srcNode
	 * @param argIsRecursion
	 *            is this is the recurse invoke,need this to enhance performance
	 */
	this.removeNode = function(srcNode, argIsRecursion) {
		if (!srcNode) {
			return;
		}// end if

		if (!srcNode.hasChild) {
			remove(srcNode);
			var ma = srcNode.parent, jj = srcNode.prev, mm = srcNode.next;
			if (!argIsRecursion) {
				// set the state
				if (ma.first == srcNode && ma.last == srcNode && ma != root) {
					ma.hasChild = false;
					// when the only child been removed,if it's not expand style
					// of "always"
					// it shuold remove the expand icon,but I don't known why
					// "alai" use this method to remove
					// I change to remove the icon
					// document.createElement("div").insertAdjacentElement("afterBegin",ma.exIcon);
					if (ma.expandStyle != "always") {
						zk.remove(ma.exIcon);
					} else {
						ma.expanded = false;
					}

					setExIcon(ma);
					ma.first = ma.last = null;
				} else {
					if (jj != null) {
						jj.next = mm;
						setExIcon(jj);
					} else {
						ma.first = mm;
					}// end if
					if (mm != null) {
						mm.prev = jj;
						setExIcon(mm);
						// hold the keydex state
						var dd;
						dd = mm;
						while (dd) {
							dd.keydex--;
							dd = dd.next;
						}
					} else {
						ma.last = jj;
					}
				}
			}

			// do not forget to remove the obj from the children
			if (ma != root) {
				ma.children.removeo(srcNode);
				if (ma.children.length < 1) {
					ma.hasChild = false;
				}
			}
			count--;
			srcNode = null;
		} else {
			var count = srcNode.children.length;
			for (var i = 0; i < count;) {
				if (srcNode.children[i]) {
					tree.removeNode(srcNode.children[i], true);
				} else {
					i++;
				}
			}
			tree.removeNode(srcNode, argIsRecursion);
		}
	}

	/**
	 * tree.locate locate the node by its keydex
	 * 
	 * @param argKeydex
	 *            the keydex of the node
	 */
	var locate = function(argKeydex) {
		var strsKeydexs;
		var index;
		var tNode;

		// the "." represent the root
		if ("." == argKeydex) {
			return tree.root;
		}
		tNode = tree.root;
		strsKeydexs = argKeydex.split(".");
		for (var i = 0; i < strsKeydexs.length; i++) {
			if ("" != strsKeydexs[i]) {
				index = new Number(strsKeydexs[i]);
				tNode = tNode.children[index - 1];
			}
		}
		// if a wrong argKeydex ,return null
		if (tree.root == tNode) {
			return null;
		}
		return tNode;
	}
	this.locate = locate;

	/**
	 * get the node's keydex path,note the keydex is start from 1
	 * 
	 * @param argNode
	 */
	this.getNodeKeydex = function(argNode) {
		var strKeydex;
		var pNode;

		strKeydex = argNode != root ? argNode.keydex : "";
		pNode = argNode.parent;
		while (root != pNode) {
			strKeydex = pNode.keydex + "." + strKeydex;
			pNode = pNode.parent;
		}
		strKeydex = "." + strKeydex;

		return strKeydex;
	}

	// getChildrenCount
	root.getChildrenCount = function() {
		return root.children != null ? root.children.length : 0;
	}

	this.getNodesByTier = function(num) {
		var col = [];
		for (var i = 0; i < count; i++) {
			if (nodes[i].getTier() == num) {
				col[col.length] = nodes[i];
			}
		}
		return col;
	}

	/**
	 * the param is the same as the expand
	 */
	this.expandAll = function(isShow, argIncsub) {
		isShow = isShow == null ? !root.expanded : isShow;
		for (var i = 0; i < count; i++) {
			nodes[i].expand(isShow, argIncsub);
			root.expanded = isShow;
		}
	}

	/**
	 * @param num
	 *            the number of the tier
	 * @param argIsneedCollapse
	 *            is need collapse all first [for optimize]
	 */
	this.expandToTier = function(num) {
		// collapse all first

		for (var i = 0; i < count; i++) {
			// collapse when is this tier
			if (nodes[i].getTier() == num && nodes[i].expanded) {
				nodes[i].expand(false, false);
			}

			if (nodes[i].getTier() < num && !nodes[i].expanded) {
				nodes[i].expand(true, false);
			}
		}
	};
	this.body = zk.cr("div")

	this.count = function() {
		return count;
	}
	this.root = root;
	this.nodes = nodes;

	zk.inade(ops.to, 'afterBegin', tree.body);
}

zk.Tree.NORMAL = 0;
zk.Tree.CHECKBOX = 1;
zk.Tree.RADIOBOX = 2;