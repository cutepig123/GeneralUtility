Test 2
http://devbean.blog.51cto.com/448512/194031
http://devbean.blog.51cto.com/448512/241186

http://devbean.blog.51cto.com/448512/243546
connection£º
drawLineAction -->drawLineActionTriggered ->changeCurrentShape -->setCurrentShape
drawRectAction -->drawRectActionTriggered ->changeCurrentShape
events£º
mousePressEvent -¡· new Rect -¡·shapeList<<shape shape->setStart(
PaintWidget::mouseMoveEvent --¡· shape->setEnd£¬ update£¨£©
mouseReleaseEvent 
