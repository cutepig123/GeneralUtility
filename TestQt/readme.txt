Test 2
http://devbean.blog.51cto.com/448512/194031
http://devbean.blog.51cto.com/448512/241186

http://devbean.blog.51cto.com/448512/243546
connection：
drawLineAction -->drawLineActionTriggered ->changeCurrentShape -->setCurrentShape
drawRectAction -->drawRectActionTriggered ->changeCurrentShape
events：
mousePressEvent -》 new Rect -》shapeList<<shape shape->setStart(
PaintWidget::mouseMoveEvent --》 shape->setEnd， update（）
mouseReleaseEvent 

//主意：貌似mainwindow.h/cpp不可以删掉，一旦删掉就编译不到


